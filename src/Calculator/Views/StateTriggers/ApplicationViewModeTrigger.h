// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp::Views::StateTriggers
{
public
    enum class AppViewMode
    {
        Normal = 0,
        CompactOverlay = 1,
        DoublePortrait = 2,
        DoubleLandscape = 3
    };

public
    ref class ApplicationViewModeTrigger sealed : public Windows::UI::Xaml::StateTriggerBase
    {
    public:
        ApplicationViewModeTrigger();
        DEPENDENCY_PROPERTY_OWNER(ApplicationViewModeTrigger);

        /* The view mode that will cause the trigger to fire. */
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(AppViewMode, ViewMode, AppViewMode::Normal);

        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, MinWindowHeight, -1);

        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, MinWindowWidth, -1);
    private:
        void OnViewModePropertyChanged(AppViewMode oldValue, AppViewMode newValue);
        void OnMinWindowHeightPropertyChanged(double oldValue, double newValue);
        void OnMinWindowWidthPropertyChanged(double oldValue, double newValue);
        void WindowSizeChanged(_In_ Platform::Object ^ sender, _In_ Windows::UI::Core::WindowSizeChangedEventArgs ^ e);
        void UpdateTrigger();
    private:
        Windows::Foundation::EventRegistrationToken m_windowSizeEventToken;
    };
}
