#pragma once

// Taken from the default template for Xaml and Direct3D 11 apps.

#include "DeviceResources.h"
#include "NearestPointRenderer.h"
#include "IGraph.h"

// Renders Direct2D and 3D content on the screen.
namespace GraphControl::DX
{
    ref class RenderMain sealed : public IDeviceNotify
    {
    public:
        virtual ~RenderMain();

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

        internal : RenderMain(Windows::UI::Xaml::Controls::SwapChainPanel ^ panel);

        property std::shared_ptr<Graphing::IGraph> Graph
        {
            void set(std::shared_ptr<Graphing::IGraph> graph);
        }

        property Windows::UI::Color BackgroundColor
        {
            void set(Windows::UI::Color color);
        }

        property bool DrawNearestPoint
        {
            void set(bool value);
        }

        property Windows::Foundation::Point PointerLocation
        {
            void set(Windows::Foundation::Point location);
        }

        void CreateWindowSizeDependentResources();

        void RunRenderPass();

        property Windows::Foundation::Point TraceValue
        {
            Windows::Foundation::Point get()
            {
                return m_TraceValue;
            }
        }

        property bool Tracing
        {
            bool get()
            {
                return m_Tracing;
            }
        }

    private:
        bool Render();

        // Loaded/Unloaded
        void OnLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        // Dependent event registration
        void RegisterEventHandlers();
        void UnregisterEventHandlers();

        // Window event handlers.
        void OnVisibilityChanged(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::VisibilityChangedEventArgs ^ args);

        // DisplayInformation event handlers.
        void OnDpiChanged(Windows::Graphics::Display::DisplayInformation ^ sender, Platform::Object ^ args);
        void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation ^ sender, Platform::Object ^ args);
        void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation ^ sender, Platform::Object ^ args);

        // Other event handlers.
        void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel ^ sender, Object ^ args);
        void OnSizeChanged(Platform::Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e);

    private:
        DX::DeviceResources m_deviceResources;
        NearestPointRenderer m_nearestPointRenderer;

        // Cached Graph object with Renderer property.
        std::shared_ptr<Graphing::IGraph> m_graph = nullptr;

        // Track current input pointer position.
        bool m_drawNearestPoint = false;
        Windows::Foundation::Point m_pointerLocation;

        float m_backgroundColor[4];

        // The SwapChainPanel^ surface.
        Windows::UI::Xaml::Controls::SwapChainPanel ^ m_swapChainPanel = nullptr;
        Windows::Foundation::EventRegistrationToken m_tokenLoaded;
        Windows::Foundation::EventRegistrationToken m_tokenCompositionScaleChanged;
        Windows::Foundation::EventRegistrationToken m_tokenSizeChanged;

        // Cached references to event notifiers.
        Platform::Agile<Windows::UI::Core::CoreWindow> m_coreWindow = nullptr;
        Windows::Foundation::EventRegistrationToken m_tokenVisibilityChanged;

        Windows::Graphics::Display::DisplayInformation ^ m_displayInformation = nullptr;
        Windows::Foundation::EventRegistrationToken m_tokenDpiChanged;
        Windows::Foundation::EventRegistrationToken m_tokenOrientationChanged;
        Windows::Foundation::EventRegistrationToken m_tokenDisplayContentsInvalidated;

        // Track our independent input on a background worker thread.
        Windows::Foundation::IAsyncAction ^ m_inputLoopWorker = nullptr;
        Windows::UI::Core::CoreIndependentInputSource ^ m_coreInput = nullptr;

        // What is the current trace value
        Windows::Foundation::Point& m_TraceValue;

        // Are we currently showing the tracing value
        bool m_Tracing;
    };
}
