// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp::Views::StateTriggers
{
    public ref class ControlSizeTrigger sealed : public Windows::UI::Xaml::StateTriggerBase
    {
    public:
        ControlSizeTrigger();

        DEPENDENCY_PROPERTY_OWNER(ControlSizeTrigger);

        DEPENDENCY_PROPERTY_WITH_CALLBACK(Windows::UI::Xaml::FrameworkElement^, Source);

        DEPENDENCY_PROPERTY_WITH_DEFAULT(double, MinHeight, -1);

        DEPENDENCY_PROPERTY_WITH_DEFAULT(double, MinWidth, -1);

    private:
        ~ControlSizeTrigger();

        void OnSourcePropertyChanged(Windows::UI::Xaml::FrameworkElement^ oldValue, Windows::UI::Xaml::FrameworkElement^ newValue);

        void RegisterSizeChanged(Windows::UI::Xaml::FrameworkElement^ element);
        void UnregisterSizeChanged(Windows::UI::Xaml::FrameworkElement^ element);
        void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);

        void UpdateIsActive(Windows::Foundation::Size sourceSize);

    private:
        Windows::Foundation::EventRegistrationToken m_sizeChangedToken;
    };
}
