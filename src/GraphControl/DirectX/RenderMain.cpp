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
        , m_TraceValue(Point(0, 0))
        , m_TraceLocation(Point(0, 0))
        , m_Tracing(false)
        , m_ActiveTracingPointRenderer{ &m_deviceResources }
    {
        // Register to be notified if the Device is lost or recreated
        m_deviceResources.RegisterDeviceNotify(this);

        RegisterEventHandlers();

        m_drawActiveTracing = false;
        m_activeTracingPointerLocation.X = 50;
        m_activeTracingPointerLocation.Y = 50;
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
            RunRenderPass();
        }
    }

    void RenderMain::PointerLocation::set(Point location)
    {
        if (m_pointerLocation != location)
        {
            m_pointerLocation = location;
            RunRenderPass();
        }
    }

    void RenderMain::ActiveTracing::set(bool value)
    {
        if (m_drawActiveTracing != value)
        {
            m_drawActiveTracing = value;
            RunRenderPass();
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
    }

    bool RenderMain::RunRenderPass()
    {
        bool succesful = Render();

        if (succesful)
        {
            m_deviceResources.Present();
        }

        return succesful;
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
                successful = SUCCEEDED(renderer->DrawD2D1(pFactory, pRenderTarget, hasMissingData));

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

                            m_ActiveTracingPointRenderer.Render(m_activeTracingPointerLocation);
                        }

                        int formulaId;
                        Point nearestPointLocation;
                        pair<float, float> nearestPointValue;
                        renderer->GetClosePointData(
                            trackPoint.X,
                            trackPoint.Y,
                            formulaId,
                            nearestPointLocation.X,
                            nearestPointLocation.Y,
                            nearestPointValue.first,
                            nearestPointValue.second);

                        if (!isnan(nearestPointLocation.X) && !isnan(nearestPointLocation.Y))
                        {
                            auto lineColors = m_graph->GetOptions().GetGraphColors();

                            if (formulaId >= 0 && formulaId < lineColors.size())
                            {
                                auto dotColor = lineColors[formulaId];
                                m_nearestPointRenderer.SetColor(D2D1::ColorF(dotColor.R * 65536 + dotColor.G * 256 + dotColor.B, 1.0));
                            }

                            m_nearestPointRenderer.Render(nearestPointLocation);
                            m_Tracing = true;
                            m_TraceLocation = Point(nearestPointLocation.X, nearestPointLocation.Y);
                            m_TraceValue = Point(nearestPointValue.first, nearestPointValue.second);
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
