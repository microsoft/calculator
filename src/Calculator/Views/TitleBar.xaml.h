// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/TitleBar.g.h"
#include "CalcViewModel\ApplicationViewModel.h"

namespace CalculatorApp
{
    /// <summary>
    /// Standalone control managing the title bar of the application.
    /// Display a transparent custom title bar when high-contrast is off and the native title bar when on.
    /// Automatically react to color changes, tablet mode, etc...
    /// </summary>
public
    ref class TitleBar sealed
    {
    public:
        TitleBar();

        DEPENDENCY_PROPERTY_OWNER(TitleBar);
        DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, IsAlwaysOnTopMode);

        event Windows::UI::Xaml::RoutedEventHandler ^ AlwaysOnTopClick;
    private:
        void OnLoaded(_In_ Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnUnloaded(_In_ Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        void SetTitleBarVisibility(bool forceDisplay);
        void SetTitleBarHeightAndPadding();
        void SetTitleBarControlColors();
        void ColorValuesChanged(_In_ Windows::UI::ViewManagement::UISettings ^ sender, _In_ Platform::Object ^ e);
        void OnHighContrastChanged(Windows::UI::ViewManagement::AccessibilitySettings ^ sender, Platform::Object ^ args);
        void OnWindowActivated(Platform::Object ^ sender, Windows::UI::Core::WindowActivatedEventArgs ^ e);
        void OnIsAlwaysOnTopModePropertyChanged(bool oldValue, bool newValue);

        Platform::Agile<Windows::ApplicationModel::Core::CoreApplicationViewTitleBar ^> m_coreTitleBar;
        Windows::Foundation::EventRegistrationToken m_layoutChangedToken;
        Windows::Foundation::EventRegistrationToken m_visibilityChangedToken;
        Windows::Foundation::EventRegistrationToken m_colorValuesChangedToken;
        Windows::Foundation::EventRegistrationToken m_windowActivatedToken;
        Windows::Foundation::EventRegistrationToken m_accessibilitySettingsToken;
        Windows::UI::ViewManagement::UISettings ^ m_uiSettings;
        Windows::UI::ViewManagement::AccessibilitySettings ^ m_accessibilitySettings;
        void AlwaysOnTopButton_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    };
}
