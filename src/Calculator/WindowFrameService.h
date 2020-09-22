// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
public
    ref class WindowFrameService sealed
    {
    public:
        Windows::UI::Xaml::Controls::Page ^ GetCurrentPage();
        void SetNewFrame(_In_ Windows::UI::Xaml::Controls::Frame ^ frame);

        internal :
            // createdByUs means any window that we create.
            // !createdByUs means the main window
            static WindowFrameService
            ^ CreateNewWindowFrameService(_In_ Windows::UI::Xaml::Controls::Frame ^ viewFrame, bool createdByUs, Platform::WeakReference parent);

        Windows::UI::Core::CoreDispatcher ^ GetCoreDispatcher();
        int GetViewId();

        void RegisterOnWindowClosingHandler(std::function<void()> onWindowClosingHandler);
        concurrency::task<void> HandleViewRelease();

        // Throws InvalidArgumentException if a service is already registered with the specified id
        void RegisterRuntimeWindowService(Windows::UI::Xaml::Interop::TypeName serviceId, _In_opt_ Platform::Object ^ service);

        // Returns false if no service was registered with the specified id
        bool RemoveRuntimeWindowService(Windows::UI::Xaml::Interop::TypeName serviceId);

        // Throws InvalidArgumentException if no service is registered with the specified id
        Platform::Object ^ ResolveRuntimeWindowService(Windows::UI::Xaml::Interop::TypeName serviceId);

        // Returns nullptr if no service is registered with the specified id
        _Ret_maybenull_ Platform::Object ^ TryResolveRuntimeWindowService(Windows::UI::Xaml::Interop::TypeName serviceId);

        Windows::UI::Xaml::Controls::Frame ^ GetFrame();
        void InvokeWindowClosingHandlers();

    private:
        WindowFrameService(_In_ Windows::UI::Xaml::Controls::Frame ^ viewFrame, Platform::WeakReference parent);
        void InitializeFrameService(bool createdByUs);

        void
        OnConsolidated(_In_ Windows::UI::ViewManagement::ApplicationView ^ sender, _In_ Windows::UI::ViewManagement::ApplicationViewConsolidatedEventArgs ^ e);
        void OnClosed(_In_ Windows::UI::Core::CoreWindow ^ sender, _In_ Windows::UI::Core::CoreWindowEventArgs ^ args);

    private:
        Platform::Agile<Windows::UI::Core::CoreWindow ^> m_currentWindow;
        Platform::Agile<Windows::UI::Core::CoreDispatcher ^> m_coreDispatcher;
        Windows::UI::Xaml::Controls::Frame ^ m_frame;
        int m_viewId;
        Platform::WeakReference m_parent;

        std::unordered_map<Platform::String ^, Platform::Object ^> m_runtimeServicesMap;
        std::vector<std::function<void()>> m_onWindowClosingHandlers;
    };
}
