// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "RenderMain.h"
#include "DirectXHelper.h"

using namespace Concurrency;
using namespace Graphing;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace
{
    constexpr unsigned int s_RedChannelIndex = 0;
    constexpr unsigned int s_GreenChannelIndex = 1;
    constexpr unsigned int s_BlueChannelIndex = 2;
    constexpr unsigned int s_AlphaChannelIndex = 3;
    constexpr float s_MaxChannelValue = 255.0f;

    constexpr float nearestPointRadius = 3;
}

namespace GraphControl::DX
{
    RenderMain::RenderMain(SwapChainPanel ^ panel)
        : m_deviceResources{ panel }
        , m_nearestPointRenderer{ &m_deviceResources }
        , m_backgroundColor{ {} }
        , m_swapChainPanel{ panel }        
        , m_TraceLocation(Point(0, 0))
        , m_Tracing(false)
    {
        // Register to be notified if the Device is lost or recreated
        m_deviceResources.RegisterDeviceNotify(this);

        RegisterEventHandlers();

        m_drawActiveTracing = false;
    }

    RenderMain::~RenderMain()
    {
        UnregisterEventHandlers();
    }

    void RenderMain::Graph::set(shared_ptr<IGraph> graph)
    {
        m_graph = move(graph);

        if (m_graph)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                float dpi = m_deviceResources.GetDpi();
                renderer->SetDpi(dpi, dpi);

                renderer->SetGraphSize(static_cast<unsigned int>(m_swapChainPanel->ActualWidth), static_cast<unsigned int>(m_swapChainPanel->ActualHeight));
            }
        }
    }

    void RenderMain::BackgroundColor::set(Windows::UI::Color backgroundColor)
    {
        m_backgroundColor[s_RedChannelIndex] = static_cast<float>(backgroundColor.R) / s_MaxChannelValue;
        m_backgroundColor[s_GreenChannelIndex] = static_cast<float>(backgroundColor.G) / s_MaxChannelValue;
        m_backgroundColor[s_BlueChannelIndex] = static_cast<float>(backgroundColor.B) / s_MaxChannelValue;
        m_backgroundColor[s_AlphaChannelIndex] = static_cast<float>(backgroundColor.A) / s_MaxChannelValue;

        RunRenderPass();
    }

    void RenderMain::DrawNearestPoint::set(bool value)
    {
        if (m_drawNearestPoint != value)
        {
            m_drawNearestPoint = value;
            if (!m_drawNearestPoint)
            {
                m_Tracing = false;
            }
        }
    }

    void RenderMain::PointerLocation::set(Point location)
    {
        if (m_pointerLocation != location)
        {
            m_pointerLocation = location;

            bool wasPointRendered = m_Tracing;
            if (CanRenderPoint() || wasPointRendered)
            {
                RunRenderPassAsync();
            }
        }
    }

    void RenderMain::ActiveTracing::set(bool value)
    {
        if (m_drawActiveTracing != value)
        {
            m_drawActiveTracing = value;

            bool wasPointRendered = m_Tracing;
            if (CanRenderPoint() || wasPointRendered)
            {
                RunRenderPassAsync();
            }
        }
    }

    bool RenderMain::ActiveTracing::get()
    {
        return m_drawActiveTracing;
    }

    // Updates application state when the window size changes (e.g. device orientation change)
    void RenderMain::CreateWindowSizeDependentResources()
    {
        // TODO: Replace this with the sizedependent initialization of your app's content.
        RunRenderPass();

        if (m_swapChainPanel != nullptr)
        {
            // Initialize the active tracing location to just above and to the right of the center of the graph area
            m_activeTracingPointerLocation.X = m_swapChainPanel->ActualWidth / 2 + 40;
            m_activeTracingPointerLocation.Y = m_swapChainPanel->ActualHeight / 2 - 40;
        }
    }

    bool RenderMain::CanRenderPoint()
    {
        if (m_graph && (m_drawNearestPoint || m_drawActiveTracing))
        {
            Point trackPoint = m_pointerLocation;

            if (m_drawActiveTracing)
            {
                trackPoint = m_activeTracingPointerLocation;
            }

            if (!m_criticalSection.try_lock())
            {
                return false;
            }

            m_criticalSection.unlock();

            critical_section::scoped_lock lock(m_criticalSection);

            int formulaId = -1;
            double outNearestPointValueX, outNearestPointValueY;
            float outNearestPointLocationX, outNearestPointLocationY;
            double rhoValueOut, thetaValueOut, tValueOut;
            
            double xAxisMin, xAxisMax, yAxisMin, yAxisMax;
            m_graph->GetRenderer()->GetDisplayRanges(xAxisMin, xAxisMax, yAxisMin, yAxisMax);
            double precision = this->GetPrecision(xAxisMax, xAxisMin);     

            m_Tracing = m_graph->GetRenderer()->GetClosePointData(
                            trackPoint.X,
                            trackPoint.Y,
                            precision,
                            formulaId,
                            outNearestPointLocationX,
                            outNearestPointLocationY,
                            outNearestPointValueX,
                            outNearestPointValueY,
                            rhoValueOut,
                            thetaValueOut,
                            tValueOut)
                        == S_OK;
            m_Tracing = m_Tracing && !isnan(outNearestPointLocationX) && !isnan(outNearestPointLocationY);
        }
        else
        {
            m_Tracing = false;
        }

        return m_Tracing;
    }

    /// <summary>
    /// Gets the precision value by computing the max and min
    /// through this formula:
    /// 10^(floor(log(max-min))-3)
    /// https://github.com/microsoft/calculator/issues/998
    /// </summary>
    /// <param name="maxAxis">max axis</param>
    /// <param name="minAxis">min axis</param>
    /// <returns>the precision value</returns>
    double RenderMain::GetPrecision(const double maxAxis, const double minAxis)
    {
        double exponent = static_cast<double>(floor(log10(maxAxis - minAxis)) - 3);
        double precision = pow(10, exponent);
        return precision;
    }   

    void RenderMain::SetPointRadius(float radius)
    {
        m_nearestPointRenderer.SetRadius(radius);
    }

    bool RenderMain::RunRenderPass()
    {
        // Non async render passes cancel if they can't obtain the lock immediatly
        if (!m_criticalSection.try_lock())
        {
            return false;
        }

        m_criticalSection.unlock();

        critical_section::scoped_lock lock(m_criticalSection);

        return RunRenderPassInternal();
    }

    IAsyncAction ^ RenderMain::RunRenderPassAsync(bool allowCancel)
    {
        // Try to cancel the renderPass that is in progress
        if (m_renderPass != nullptr && m_renderPass->Status == ::AsyncStatus::Started)
        {
            m_renderPass->Cancel();
        }

        auto device = m_deviceResources;
        auto workItemHandler = ref new WorkItemHandler([this, allowCancel](IAsyncAction ^ action) {
            critical_section::scoped_lock lock(m_criticalSection);

            // allowCancel is passed as false when the grapher relies on the render pass to validate that an equation can be succesfully rendered.
            // Passing false garauntees that another render pass doesn't cancel this one.
            if (allowCancel && action->Status == ::AsyncStatus::Canceled)
            {
                return;
            }

            RunRenderPassInternal();
        });

        m_renderPass = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::None);

        return m_renderPass;
    }

    bool RenderMain::RunRenderPassInternal()
    {
        // We are accessing Direct3D resources directly without Direct2D's knowledge, so we
        // must manually acquire and apply the Direct2D factory lock.
        ID2D1Multithread* m_D2DMultithread;
        m_deviceResources.GetD2DFactory()->QueryInterface(IID_PPV_ARGS(&m_D2DMultithread));
        m_D2DMultithread->Enter();

        bool succesful = Render();

        if (succesful)
        {
            m_deviceResources.Present();
        }

        // It is absolutely critical that the factory lock be released upon
        // exiting this function, or else any consequent Direct2D calls will be blocked.
        m_D2DMultithread->Leave();

        m_isRenderPassSuccesful = succesful;
        return m_isRenderPassSuccesful;
    }

    // Renders the current frame according to the current application state.
    // Returns true if the frame was rendered and is ready to be displayed.
    bool RenderMain::Render()
    {
        bool successful = true;

        // Must call BeginDraw before any draw commands.
        ID2D1Factory3* pFactory = m_deviceResources.GetD2DFactory();
        ID2D1DeviceContext* pRenderTarget = m_deviceResources.GetD2DDeviceContext();

        auto context = m_deviceResources.GetD3DDeviceContext();

        // Clear the back buffer and set the background color.
        context->ClearRenderTargetView(m_deviceResources.GetBackBufferRenderTargetView(), m_backgroundColor);

        if (m_graph)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                pRenderTarget->BeginDraw();

                bool hasMissingData = false;
                m_HResult = renderer->DrawD2D1(pFactory, pRenderTarget, hasMissingData);

                successful = SUCCEEDED(m_HResult);

                // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
                // is lost. It will be handled during the next call to Present.
                HRESULT endDraw = pRenderTarget->EndDraw();
                if (endDraw != D2DERR_RECREATE_TARGET)
                {
                    DX::ThrowIfFailed(endDraw);
                }

                if (successful)
                {
                    if (m_drawNearestPoint || m_drawActiveTracing)
                    {
                        Point trackPoint = m_pointerLocation;
                        if (m_drawActiveTracing)
                        {
                            // Active tracing takes over for draw nearest point input from the mouse pointer.
                            trackPoint = m_activeTracingPointerLocation;
                        }

                        int formulaId = -1;
                        double outNearestPointValueX, outNearestPointValueY;                        
                        double rhoValueOut, thetaValueOut, tValueOut;
                        float outNearestPointLocationX, outNearestPointLocationY;
                        double xAxisMin, xAxisMax, yAxisMin, yAxisMax;
                        renderer->GetDisplayRanges(xAxisMin, xAxisMax, yAxisMin, yAxisMax);
                        double precision = this->GetPrecision(xAxisMax, xAxisMin);   
                        if (renderer->GetClosePointData(
                                trackPoint.X,
                                trackPoint.Y,
                                precision,
                                formulaId,
                                outNearestPointLocationX,
                                outNearestPointLocationY,
                                outNearestPointValueX,
                                outNearestPointValueY,
                                rhoValueOut,
                                thetaValueOut,
                                tValueOut)
                            == S_OK)
                        {
                            if (!isnan(outNearestPointLocationX) && !isnan(outNearestPointLocationY))
                            {
                                auto lineColors = m_graph->GetOptions().GetGraphColors();

                                if (formulaId >= 0 && static_cast<unsigned int>(formulaId) < lineColors.size())
                                {
                                    auto dotColor = lineColors[formulaId];
                                    m_nearestPointRenderer.SetColor(D2D1::ColorF(dotColor.R * 65536 + dotColor.G * 256 + dotColor.B, 1.0));
                                }

                                m_TraceLocation = Point(outNearestPointLocationX, outNearestPointLocationY);
                                m_nearestPointRenderer.Render(m_TraceLocation);
                                m_Tracing = true;
                                m_TraceLocation = Point(outNearestPointLocationX, outNearestPointLocationY);                                
                                m_XTraceValue = outNearestPointValueX;
                                m_YTraceValue = outNearestPointValueY;
                            }
                            else
                            {
                                m_Tracing = false;
                            }
                        }
                        else
                        {
                            m_Tracing = false;
                        }
                    }
                }
            }
        }

        return successful;
    }

    HRESULT RenderMain::GetRenderError()
    {
        return m_HResult;
    }

    void RenderMain::OnLoaded(Object ^ sender, RoutedEventArgs ^ e)
    {
        RunRenderPass();
    }

    void RenderMain::RegisterEventHandlers()
    {
        UnregisterEventHandlers();

        // Register event handlers for control lifecycle.
        m_coreWindow = Agile<CoreWindow>(Window::Current->CoreWindow);
        if (m_coreWindow != nullptr)
        {
            m_tokenVisibilityChanged = m_coreWindow->VisibilityChanged +=
                ref new TypedEventHandler<CoreWindow ^, VisibilityChangedEventArgs ^>(this, &RenderMain::OnVisibilityChanged);
        }

        m_displayInformation = DisplayInformation::GetForCurrentView();
        if (m_displayInformation != nullptr)
        {
            m_tokenDpiChanged = m_displayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation ^, Object ^>(this, &RenderMain::OnDpiChanged);

            m_tokenOrientationChanged = m_displayInformation->OrientationChanged +=
                ref new TypedEventHandler<DisplayInformation ^, Object ^>(this, &RenderMain::OnOrientationChanged);
        }

        m_tokenDisplayContentsInvalidated = DisplayInformation::DisplayContentsInvalidated +=
            ref new TypedEventHandler<DisplayInformation ^, Object ^>(this, &RenderMain::OnDisplayContentsInvalidated);

        if (m_swapChainPanel != nullptr)
        {
            m_tokenLoaded = m_swapChainPanel->Loaded += ref new RoutedEventHandler(this, &RenderMain::OnLoaded);

            m_tokenCompositionScaleChanged = m_swapChainPanel->CompositionScaleChanged +=
                ref new TypedEventHandler<SwapChainPanel ^, Object ^>(this, &RenderMain::OnCompositionScaleChanged);

            m_tokenSizeChanged = m_swapChainPanel->SizeChanged += ref new SizeChangedEventHandler(this, &RenderMain::OnSizeChanged);
        }
    }

    void RenderMain::UnregisterEventHandlers()
    {
        if (m_coreWindow != nullptr)
        {
            if (m_tokenVisibilityChanged.Value != 0)
            {
                m_coreWindow->VisibilityChanged -= m_tokenVisibilityChanged;
                m_tokenVisibilityChanged.Value = 0;
            }
            m_coreWindow = nullptr;
        }

        if (m_displayInformation != nullptr)
        {
            if (m_tokenDpiChanged.Value != 0)
            {
                m_displayInformation->DpiChanged -= m_tokenDpiChanged;
                m_tokenDpiChanged.Value = 0;
            }
            if (m_tokenOrientationChanged.Value != 0)
            {
                m_displayInformation->OrientationChanged -= m_tokenOrientationChanged;
                m_tokenOrientationChanged.Value = 0;
            }
            m_displayInformation = nullptr;
        }

        if (m_tokenDisplayContentsInvalidated.Value != 0)
        {
            DisplayInformation::DisplayContentsInvalidated -= m_tokenDisplayContentsInvalidated;
            m_tokenDisplayContentsInvalidated.Value = 0;
        }

        if (m_swapChainPanel != nullptr)
        {
            if (m_tokenLoaded.Value != 0)
            {
                m_swapChainPanel->Loaded -= m_tokenLoaded;
                m_tokenLoaded.Value = 0;
            }
            if (m_tokenCompositionScaleChanged.Value != 0)
            {
                m_swapChainPanel->CompositionScaleChanged -= m_tokenCompositionScaleChanged;
                m_tokenCompositionScaleChanged.Value = 0;
            }
            if (m_tokenSizeChanged.Value != 0)
            {
                m_swapChainPanel->SizeChanged -= m_tokenSizeChanged;
                m_tokenSizeChanged.Value = 0;
            }
        }
    }

    void RenderMain::OnVisibilityChanged(CoreWindow ^ sender, VisibilityChangedEventArgs ^ args)
    {
        if (args->Visible)
        {
            RunRenderPass();
        }
    }

    void RenderMain::OnDpiChanged(DisplayInformation ^ sender, Object ^ args)
    {
        // Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
        // if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
        // you should always retrieve it using the GetDpi method.
        // See DeviceResources.cpp for more details.
        m_deviceResources.SetDpi(sender->LogicalDpi);

        if (m_graph)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                float dpi = m_deviceResources.GetDpi();
                renderer->SetDpi(dpi, dpi);
            }
        }

        CreateWindowSizeDependentResources();
    }

    void RenderMain::OnOrientationChanged(DisplayInformation ^ sender, Object ^ args)
    {
        m_deviceResources.SetCurrentOrientation(sender->CurrentOrientation);
        CreateWindowSizeDependentResources();
    }

    void RenderMain::OnDisplayContentsInvalidated(DisplayInformation ^ sender, Object ^ args)
    {
        m_deviceResources.ValidateDevice();
    }

    void RenderMain::OnCompositionScaleChanged(SwapChainPanel ^ sender, Object ^ args)
    {
        m_deviceResources.SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
        CreateWindowSizeDependentResources();
    }

    void RenderMain::OnSizeChanged(Object ^ sender, SizeChangedEventArgs ^ e)
    {
        m_deviceResources.SetLogicalSize(e->NewSize);

        if (m_graph)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                const auto& newSize = e->NewSize;
                renderer->SetGraphSize(static_cast<unsigned int>(newSize.Width), static_cast<unsigned int>(newSize.Height));
            }
        }

        CreateWindowSizeDependentResources();
    }

    // Notifies renderers that device resources need to be released.
    void RenderMain::OnDeviceLost()
    {
        m_nearestPointRenderer.ReleaseDeviceDependentResources();
    }

    // Notifies renderers that device resources may now be recreated.
    void RenderMain::OnDeviceRestored()
    {
        m_nearestPointRenderer.CreateDeviceDependentResources();
    }
}
