// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Common
    {
        class TitleBarHelper
        {
        public:
            static std::unique_ptr<TitleBarHelper> CreateTitleBarHelperIfNotDocked(
                _In_ Windows::UI::Xaml::FrameworkElement^ customTitleBar);

            // Prefer CreateTitleBarHelper over constructing your own instance,
            // because Create* will nullcheck the parameters.
            static std::unique_ptr<TitleBarHelper> CreateTitleBarHelper(
                _In_ Windows::UI::Xaml::FrameworkElement^ customTitleBar);

            TitleBarHelper(
                _In_ Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ coreTitleBar,
                _In_ Windows::UI::Xaml::FrameworkElement^ customTitleBar);
            ~TitleBarHelper();

            void SetTitleBarHeight(double height);
            void SetTitleBarVisibility(bool isVisible);

        private:
            void RegisterForLayoutChanged();
            void RegisterForVisibilityChanged();
            void SetCustomTitleBar();

            Platform::Agile<Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^> m_coreTitleBar;
            Windows::UI::Xaml::FrameworkElement^ m_customTitleBar;
            Windows::Foundation::EventRegistrationToken m_layoutChangedToken;
            Windows::Foundation::EventRegistrationToken m_visibilityChangedToken;
        };
    }
}
