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

    private:
        void OnViewModePropertyChanged(AppViewMode oldValue, AppViewMode newValue);
        void WindowSizeChanged(_In_ Platform::Object ^ sender, _In_ Windows::UI::Core::WindowSizeChangedEventArgs ^ e);
        void UpdateTrigger();
    private:
        bool _isWindows10X;
        Windows::Foundation::EventRegistrationToken m_windowSizeEventToken;
    };
}
