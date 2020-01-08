// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// App.xaml.h
// Declaration of the App class.
//

#pragma once

#include "App.g.h"
#include "WindowFrameService.h"

namespace CalculatorApp
{
    namespace ApplicationResourceKeys
    {
        extern Platform::StringReference AppMinWindowHeight;
        extern Platform::StringReference AppMinWindowWidth;
    }

    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    ref class App sealed
    {
    public:
        App();
        virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs ^ args) override;
        virtual void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs ^ args) override;

    internal:
        void RemoveWindow(_In_ WindowFrameService ^ frameService);
        void RemoveSecondaryWindow(_In_ WindowFrameService ^ frameService);

    private:
        static Windows::UI::Xaml::Controls::Frame ^ CreateFrame();
        static void SetMinWindowSizeAndActivate(Windows::UI::Xaml::Controls::Frame ^ rootFrame, Windows::Foundation::Size minWindowSize);

        void OnAppLaunch(Windows::ApplicationModel::Activation::IActivatedEventArgs ^ args, Platform::String ^ argument);
        void DismissedEventHandler(Windows::ApplicationModel::Activation::SplashScreen ^ sender, Platform::Object ^ e);
        void RegisterDependencyProperties();
        void OnSuspending(Platform::Object ^ sender, Windows::ApplicationModel::SuspendingEventArgs ^ args);


        class SafeFrameWindowCreation final
        {
        public:
            SafeFrameWindowCreation(_In_ WindowFrameService ^ frameService, App ^ parent)
                : m_frameService(frameService)
                , m_frameOpenedInWindow(false)
                , m_parent(parent)
            {
            }

            void SetOperationSuccess(bool success)
            {
                m_frameOpenedInWindow = success;
            }

            ~SafeFrameWindowCreation()
            {
                if (!m_frameOpenedInWindow)
                {
                    // Close the window as the navigation to the window didn't succeed
                    // and this is not visible to the user.
                    m_parent->RemoveWindowFromMap(m_frameService->GetViewId());
                }
            }

        private:
            WindowFrameService ^ m_frameService;
            bool m_frameOpenedInWindow;
            App ^ m_parent;
        };

    private:
        concurrency::reader_writer_lock m_windowsMapLock;
        std::unordered_map<int, WindowFrameService ^> m_secondaryWindows;

        concurrency::task<void> SetupJumpList();
        concurrency::task<void> HandleViewReleaseAndRemoveWindowFromMap(_In_ WindowFrameService ^ frameService);
        void AddWindowToMap(_In_ WindowFrameService ^ frameService);
        WindowFrameService ^ GetWindowFromMap(int viewId);
        void RemoveWindowFromMap(int viewId);
        int m_mainViewId;
        bool m_preLaunched;

        Windows::UI::Xaml::Controls::Primitives::Popup ^ m_aboutPopup;
    };
}
