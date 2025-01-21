// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DeviceResources.h"
#include "DirectXHelper.h"

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Platform;

namespace DisplayMetrics
{
    // High resolution displays can require a lot of GPU and battery power to render.
    // High resolution phones, for example, may suffer from poor battery life if
    // games attempt to render at 60 frames per second at full fidelity.
    // The decision to render at full fidelity across all platforms and form factors
    // should be deliberate.
    static constexpr bool SupportHighResolutions = false;

    // The default thresholds that define a "high resolution" display. If the thresholds
    // are exceeded and SupportHighResolutions is false, the dimensions will be scaled
    // by 50%.
    static constexpr float DpiThreshold = 192.0f;        // 200% of standard desktop display.
    static constexpr float WidthThreshold = 1920.0f;    // 1080p width.
    static constexpr float HeightThreshold = 1080.0f;    // 1080p height.
};

// Constants used to calculate screen rotations.
namespace ScreenRotation
{
    // 0-degree Z-rotation
    static constexpr XMFLOAT4X4 Rotation0(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    // 90-degree Z-rotation
    static constexpr XMFLOAT4X4 Rotation90(
        0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    // 180-degree Z-rotation
    static constexpr XMFLOAT4X4 Rotation180(
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    // 270-degree Z-rotation
    static constexpr XMFLOAT4X4 Rotation270(
        0.0f, -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );
};

namespace GraphControl::DX
{
    // Constructor for DeviceResources.
    DeviceResources::DeviceResources(SwapChainPanel^ panel) :
        m_screenViewport(),
        m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
        m_d3dRenderTargetSize(),
        m_outputSize(),
        m_logicalSize(),
        m_nativeOrientation(DisplayOrientations::None),
        m_currentOrientation(DisplayOrientations::None),
        m_dpi(-1.0f),
        m_effectiveDpi(-1.0f),
        m_compositionScaleX(1.0f),
        m_compositionScaleY(1.0f),
        m_deviceNotify(nullptr)
    {
        CreateDeviceIndependentResources();
        CreateDeviceResources();
        SetSwapChainPanel(panel);
    }

    // Configures resources that don't depend on the Direct3D device.
    void DeviceResources::CreateDeviceIndependentResources()
    {
        // Initialize Direct2D resources.
        D2D1_FACTORY_OPTIONS options;
        ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
        // If the project is in a debug build, enable Direct2D debugging via SDK Layers.
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

        // Initialize the Direct2D Factory.
        DX::ThrowIfFailed(
            D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED,
                __uuidof(ID2D1Factory3),
                &options,
                &m_d2dFactory
            )
        );

        // Initialize the DirectWrite Factory.
        DX::ThrowIfFailed(
            DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory3),
                &m_dwriteFactory
            )
        );

        // Initialize the Windows Imaging Component (WIC) Factory.
        DX::ThrowIfFailed(
            CoCreateInstance(
                CLSID_WICImagingFactory2,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&m_wicFactory)
            )
        );
    }

    // Configures the Direct3D device, and stores handles to it and the device context.
    void DeviceResources::CreateDeviceResources()
    {
        // This flag adds support for surfaces with a different color channel ordering
        // than the API default. It is required for compatibility with Direct2D.
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
        if (DX::SdkLayersAvailable())
        {
            // If the project is in a debug build, enable debugging via SDK Layers with this flag.
            creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
        }
#endif

        // This array defines the set of DirectX hardware feature levels this app will support.
        // Note the ordering should be preserved.
        // Don't forget to declare your application's minimum required feature level in its
        // description.  All applications are assumed to support 9.1 unless otherwise stated.
        static constexpr std::array featureLevels =
        {
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        // Create the Direct3D 11 API device object and a corresponding context.
        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;

        HRESULT hr = D3D11CreateDevice(
            nullptr,                                 // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_HARDWARE,                // Create a device using the hardware graphics driver.
            0,                                       // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
            creationFlags,                           // Set debug and Direct2D compatibility flags.
            featureLevels.data(),                    // List of feature levels this app can support.
            static_cast<UINT>(featureLevels.size()), // Size of the list above.
            D3D11_SDK_VERSION,                       // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            &device,                                 // Returns the Direct3D device created.
            &m_d3dFeatureLevel,                      // Returns feature level of device created.
            &context                                 // Returns the device immediate context.
        );

        if (FAILED(hr))
        {
            // If the initialization fails, fall back to the WARP device.
            // For more information on WARP, see: 
            // https://go.microsoft.com/fwlink/?LinkId=286690
            DX::ThrowIfFailed(
                D3D11CreateDevice(
                    nullptr,
                    D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
                    0,
                    creationFlags,
                    featureLevels.data(),
                    static_cast<UINT>(featureLevels.size()),
                    D3D11_SDK_VERSION,
                    &device,
                    &m_d3dFeatureLevel,
                    &context
                )
            );
        }

        // Store pointers to the Direct3D 11.3 API device and immediate context.
        DX::ThrowIfFailed(
            device.As(&m_d3dDevice)
        );

        DX::ThrowIfFailed(
            context.As(&m_d3dContext)
        );

        // Create the Direct2D device object and a corresponding context.
        ComPtr<IDXGIDevice3> dxgiDevice;
        DX::ThrowIfFailed(
            m_d3dDevice.As(&dxgiDevice)
        );

        DX::ThrowIfFailed(
            m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice)
        );

        DX::ThrowIfFailed(
            m_d2dDevice->CreateDeviceContext(
                D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                &m_d2dContext
            )
        );
    }

    // These resources need to be recreated every time the window size is changed.
    void DeviceResources::CreateWindowSizeDependentResources()
    {
        // Clear the previous window size specific context.
        static constexpr std::array<ID3D11RenderTargetView*, 1> nullViews = { nullptr };
        m_d3dContext->OMSetRenderTargets(static_cast<UINT>(nullViews.size()), &nullViews[0], nullptr);
        m_d3dRenderTargetView = nullptr;
        m_d2dContext->SetTarget(nullptr);
        m_d2dTargetBitmap = nullptr;
        m_d3dDepthStencilView = nullptr;
        m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

        UpdateRenderTargetSize();

        m_d3dRenderTargetSize.Width = m_outputSize.Width;
        m_d3dRenderTargetSize.Height = m_outputSize.Height;

        if (m_swapChain != nullptr)
        {
            // If the swap chain already exists, resize it.
            HRESULT hr = m_swapChain->ResizeBuffers(
                2, // Double-buffered swap chain.
                lround(m_d3dRenderTargetSize.Width),
                lround(m_d3dRenderTargetSize.Height),
                DXGI_FORMAT_B8G8R8A8_UNORM,
                0);

            if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            {
                // If the device was removed for any reason, a new device and swap chain will need to be created.
                HandleDeviceLost();

                // Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method
                // and correctly set up the new device.
                return;
            }
            else
            {
                DX::ThrowIfFailed(hr);
            }
        }
        else
        {
            // Otherwise, create a new one using the same adapter as the existing Direct3D device.
            DXGI_SCALING scaling = DisplayMetrics::SupportHighResolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

            swapChainDesc.Width = lround(m_d3dRenderTargetSize.Width);        // Match the size of the window.
            swapChainDesc.Height = lround(m_d3dRenderTargetSize.Height);
            swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;                // This is the most common swap chain format.
            swapChainDesc.Stereo = false;
            swapChainDesc.SampleDesc.Count = 1;                                // Don't use multi-sampling.
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = 2;                                    // Use double-buffering to minimize latency.
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;    // All Windows Store apps must use _FLIP_ SwapEffects.
            swapChainDesc.Flags = 0;
            swapChainDesc.Scaling = scaling;
            swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

            // This sequence obtains the DXGI factory that was used to create the Direct3D device above.
            ComPtr<IDXGIDevice3> dxgiDevice;
            DX::ThrowIfFailed(
                m_d3dDevice.As(&dxgiDevice)
            );

            ComPtr<IDXGIAdapter> dxgiAdapter;
            DX::ThrowIfFailed(
                dxgiDevice->GetAdapter(&dxgiAdapter)
            );

            ComPtr<IDXGIFactory4> dxgiFactory;
            DX::ThrowIfFailed(
                dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
            );

            // When using XAML interop, the swap chain must be created for composition.
            ComPtr<IDXGISwapChain1> swapChain;
            DX::ThrowIfFailed(
                dxgiFactory->CreateSwapChainForComposition(
                    m_d3dDevice.Get(),
                    &swapChainDesc,
                    nullptr,
                    &swapChain
                )
            );

            DX::ThrowIfFailed(
                swapChain.As(&m_swapChain)
            );

            // Associate swap chain with SwapChainPanel
            // UI changes will need to be dispatched back to the UI thread
            m_swapChainPanel->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([=]()
            {
                // Get backing native interface for SwapChainPanel
                ComPtr<ISwapChainPanelNative> panelNative;
                DX::ThrowIfFailed(
                    reinterpret_cast<IUnknown*>(m_swapChainPanel)->QueryInterface(IID_PPV_ARGS(&panelNative))
                );

                DX::ThrowIfFailed(
                    panelNative->SetSwapChain(m_swapChain.Get())
                );
            }, CallbackContext::Any));

            // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
            // ensures that the application will only render after each VSync, minimizing power consumption.
            DX::ThrowIfFailed(
                dxgiDevice->SetMaximumFrameLatency(1)
            );
        }

        // Set the proper orientation for the swap chain, and generate 2D and
        // 3D matrix transformations for rendering to the rotated swap chain.
        // Note the rotation angle for the 2D and 3D transforms are different.
        // This is due to the difference in coordinate spaces.  Additionally,
        // the 3D matrix is specified explicitly to avoid rounding errors.

        m_orientationTransform2D = Matrix3x2F::Identity();
        m_orientationTransform3D = ScreenRotation::Rotation0;

        // Setup inverse scale on the swap chain
        DXGI_MATRIX_3X2_F inverseScale = { 0 };
        inverseScale._11 = 1.0f / m_effectiveCompositionScaleX;
        inverseScale._22 = 1.0f / m_effectiveCompositionScaleY;
        ComPtr<IDXGISwapChain2> spSwapChain2;
        DX::ThrowIfFailed(
            m_swapChain.As<IDXGISwapChain2>(&spSwapChain2)
        );

        DX::ThrowIfFailed(
            spSwapChain2->SetMatrixTransform(&inverseScale)
        );

        // Create a render target view of the swap chain back buffer.
        ComPtr<ID3D11Texture2D1> backBuffer;
        DX::ThrowIfFailed(
            m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))
        );

        DX::ThrowIfFailed(
            m_d3dDevice->CreateRenderTargetView1(
                backBuffer.Get(),
                nullptr,
                &m_d3dRenderTargetView
            )
        );

        // Create a depth stencil view for use with 3D rendering if needed.
        CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            lround(m_d3dRenderTargetSize.Width),
            lround(m_d3dRenderTargetSize.Height),
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL
        );

        ComPtr<ID3D11Texture2D1> depthStencil;
        DX::ThrowIfFailed(
            m_d3dDevice->CreateTexture2D1(
                &depthStencilDesc,
                nullptr,
                &depthStencil
            )
        );

        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
        DX::ThrowIfFailed(
            m_d3dDevice->CreateDepthStencilView(
                depthStencil.Get(),
                &depthStencilViewDesc,
                &m_d3dDepthStencilView
            )
        );

        // Set the 3D rendering viewport to target the entire window.
        m_screenViewport = CD3D11_VIEWPORT(
            0.0f,
            0.0f,
            m_d3dRenderTargetSize.Width,
            m_d3dRenderTargetSize.Height
        );

        m_d3dContext->RSSetViewports(1, &m_screenViewport);

        // Create a Direct2D target bitmap associated with the
        // swap chain back buffer and set it as the current target.
        D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                m_dpi,
                m_dpi
            );

        ComPtr<IDXGISurface2> dxgiBackBuffer;
        DX::ThrowIfFailed(
            m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))
        );

        DX::ThrowIfFailed(
            m_d2dContext->CreateBitmapFromDxgiSurface(
                dxgiBackBuffer.Get(),
                &bitmapProperties,
                &m_d2dTargetBitmap
            )
        );

        m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
        m_d2dContext->SetDpi(m_effectiveDpi, m_effectiveDpi);

        // Grayscale text anti-aliasing is recommended for all Windows Store apps.
        m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    }

    // Determine the dimensions of the render target and whether it will be scaled down.
    void DeviceResources::UpdateRenderTargetSize()
    {
        m_effectiveDpi = m_dpi;
        m_effectiveCompositionScaleX = m_compositionScaleX;
        m_effectiveCompositionScaleY = m_compositionScaleY;

        // To improve battery life on high resolution devices, render to a smaller render target
        // and allow the GPU to scale the output when it is presented.
        if (!DisplayMetrics::SupportHighResolutions && m_dpi > DisplayMetrics::DpiThreshold)
        {
            float width = DX::ConvertDipsToPixels(m_logicalSize.Width, m_dpi);
            float height = DX::ConvertDipsToPixels(m_logicalSize.Height, m_dpi);

            // When the device is in portrait orientation, height > width. Compare the
            // larger dimension against the width threshold and the smaller dimension
            // against the height threshold.
            if (max(width, height) > DisplayMetrics::WidthThreshold && min(width, height) > DisplayMetrics::HeightThreshold)
            {
                // To scale the app we change the effective DPI. Logical size does not change.
                m_effectiveDpi /= 2.0f;
                m_effectiveCompositionScaleX /= 2.0f;
                m_effectiveCompositionScaleY /= 2.0f;
            }
        }

        // Calculate the necessary render target size in pixels.
        m_outputSize.Width = DX::ConvertDipsToPixels(m_logicalSize.Width, m_effectiveDpi);
        m_outputSize.Height = DX::ConvertDipsToPixels(m_logicalSize.Height, m_effectiveDpi);

        // Prevent zero size DirectX content from being created.
        m_outputSize.Width = max(m_outputSize.Width, 1.0f);
        m_outputSize.Height = max(m_outputSize.Height, 1.0f);
    }

    // This method is called when the XAML control is created (or re-created).
    void DeviceResources::SetSwapChainPanel(SwapChainPanel^ panel)
    {
        DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

        m_swapChainPanel = panel;
        m_logicalSize = Windows::Foundation::Size(static_cast<float>(panel->ActualWidth), static_cast<float>(panel->ActualHeight));
        m_nativeOrientation = currentDisplayInformation->NativeOrientation;
        m_currentOrientation = currentDisplayInformation->CurrentOrientation;
        m_compositionScaleX = panel->CompositionScaleX;
        m_compositionScaleY = panel->CompositionScaleY;
        m_dpi = currentDisplayInformation->LogicalDpi;
        m_d2dContext->SetDpi(m_dpi, m_dpi);

        CreateWindowSizeDependentResources();
    }

    // This method is called in the event handler for the SizeChanged event.
    void DeviceResources::SetLogicalSize(Windows::Foundation::Size logicalSize)
    {
        if (m_logicalSize != logicalSize)
        {
            m_logicalSize = logicalSize;
            CreateWindowSizeDependentResources();
        }
    }

    // This method is called in the event handler for the DpiChanged event.
    void DeviceResources::SetDpi(float dpi)
    {
        if (dpi != m_dpi)
        {
            m_dpi = dpi;
            m_d2dContext->SetDpi(m_dpi, m_dpi);
            CreateWindowSizeDependentResources();
        }
    }

    // This method is called in the event handler for the OrientationChanged event.
    void DeviceResources::SetCurrentOrientation(DisplayOrientations currentOrientation)
    {
        if (m_currentOrientation != currentOrientation)
        {
            m_currentOrientation = currentOrientation;
            CreateWindowSizeDependentResources();
        }
    }

    // This method is called in the event handler for the CompositionScaleChanged event.
    void DeviceResources::SetCompositionScale(float compositionScaleX, float compositionScaleY)
    {
        if (m_compositionScaleX != compositionScaleX || m_compositionScaleY != compositionScaleY)
        {
            m_compositionScaleX = compositionScaleX;
            m_compositionScaleY = compositionScaleY;
            CreateWindowSizeDependentResources();
        }
    }

    // This method is called in the event handler for the DisplayContentsInvalidated event.
    void DeviceResources::ValidateDevice()
    {
        // The D3D Device is no longer valid if the default adapter changed since the device
        // was created or if the device has been removed.

        // First, get the information for the default adapter from when the device was created.

        ComPtr<IDXGIDevice3> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        ComPtr<IDXGIAdapter> deviceAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(&deviceAdapter));

        ComPtr<IDXGIFactory2> deviceFactory;
        DX::ThrowIfFailed(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));

        ComPtr<IDXGIAdapter1> previousDefaultAdapter;
        DX::ThrowIfFailed(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));

        DXGI_ADAPTER_DESC1 previousDesc;
        DX::ThrowIfFailed(previousDefaultAdapter->GetDesc1(&previousDesc));

        // Next, get the information for the current default adapter.

        ComPtr<IDXGIFactory4> currentFactory;
        DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));

        ComPtr<IDXGIAdapter1> currentDefaultAdapter;
        DX::ThrowIfFailed(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));

        DXGI_ADAPTER_DESC1 currentDesc;
        DX::ThrowIfFailed(currentDefaultAdapter->GetDesc1(&currentDesc));

        // If the adapter LUIDs don't match, or if the device reports that it has been removed,
        // a new D3D device must be created.

        if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
            previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
            FAILED(m_d3dDevice->GetDeviceRemovedReason()))
        {
            // Release references to resources related to the old device.
            dxgiDevice = nullptr;
            deviceAdapter = nullptr;
            deviceFactory = nullptr;
            previousDefaultAdapter = nullptr;

            // Create a new device and swap chain.
            HandleDeviceLost();
        }
    }

    // Recreate all device resources and set them back to the current state.
    void DeviceResources::HandleDeviceLost()
    {
        m_swapChain = nullptr;

        if (m_deviceNotify != nullptr)
        {
            m_deviceNotify->OnDeviceLost();
        }

        CreateDeviceResources();
        m_d2dContext->SetDpi(m_dpi, m_dpi);
        CreateWindowSizeDependentResources();

        if (m_deviceNotify != nullptr)
        {
            m_deviceNotify->OnDeviceRestored();
        }
    }

    // Register our DeviceNotify to be informed on device lost and creation.
    void DeviceResources::RegisterDeviceNotify(DX::IDeviceNotify^ deviceNotify)
    {
        m_deviceNotify = deviceNotify;
    }

    // Call this method when the app suspends. It provides a hint to the driver that the app
    // is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
    void DeviceResources::Trim()
    {
        ComPtr<IDXGIDevice3> dxgiDevice;
        m_d3dDevice.As(&dxgiDevice);

        dxgiDevice->Trim();
    }

    // Present the contents of the swap chain to the screen.
    void DeviceResources::Present()
    {
        // The first argument instructs DXGI to block until VSync, putting the application
        // to sleep until the next VSync. This ensures we don't waste any cycles rendering
        // frames that will never be displayed to the screen.
        DXGI_PRESENT_PARAMETERS parameters = { 0 };
        HRESULT hr = m_swapChain->Present1(1, 0, &parameters);

        // Discard the contents of the render target.
        // This is a valid operation only when the existing contents will be entirely
        // overwritten. If dirty or scroll rects are used, this call should be modified.
        m_d3dContext->DiscardView1(m_d3dRenderTargetView.Get(), nullptr, 0);

        // Discard the contents of the depth stencil.
        m_d3dContext->DiscardView1(m_d3dDepthStencilView.Get(), nullptr, 0);

        // If the device was removed either by a disconnection or a driver upgrade, we
        // must recreate all device resources.
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            HandleDeviceLost();
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
}
