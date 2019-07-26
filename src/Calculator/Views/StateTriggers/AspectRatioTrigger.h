// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* The AspectRatioTrigger class is a custom trigger for use with a VisualState. The trigger is designed to fire when the
   height/width of the source FrameworkElement is greater than a specified threshold. In order to be a flexible class, it
   exposes a NumeratorAspect property that can be either Height or Width. The property chosen will be the numerator when
   calculating the ratio between the two properties. Additionally, users can configure whether the ratio must be strictly
   greater than the threshold, or if equal should be considered acceptable for the state to trigger. */

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp::Views::StateTriggers
{
public
    enum class Aspect
    {
        Height,
        Width
    };

public
    ref class AspectRatioTrigger sealed : public Windows::UI::Xaml::StateTriggerBase
    {
    public:
        AspectRatioTrigger();

        DEPENDENCY_PROPERTY_OWNER(AspectRatioTrigger);

        /* The source for which this class will respond to size changed events. */
        DEPENDENCY_PROPERTY_WITH_CALLBACK(Windows::UI::Xaml::FrameworkElement ^, Source);

        /* Either Height or Width. The property will determine which aspect is used as the numerator when calculating
           the aspect ratio. */
        DEPENDENCY_PROPERTY(Aspect, NumeratorAspect);

        /* The threshold that will cause the trigger to fire when the aspect ratio exceeds this value. */
        DEPENDENCY_PROPERTY_WITH_DEFAULT(double, Threshold, 0.0);

        /* If true, the trigger will fire if the aspect ratio is greater than or equal to the threshold. */
        DEPENDENCY_PROPERTY_WITH_DEFAULT(bool, ActiveIfEqual, false);

    private:
        ~AspectRatioTrigger();

        void OnSourcePropertyChanged(Windows::UI::Xaml::FrameworkElement ^ oldValue, Windows::UI::Xaml::FrameworkElement ^ newValue);

        void RegisterSizeChanged(Windows::UI::Xaml::FrameworkElement ^ element);
        void UnregisterSizeChanged(Windows::UI::Xaml::FrameworkElement ^ element);
        void OnSizeChanged(Platform::Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e);

        void UpdateIsActive(Windows::Foundation::Size sourceSize);

    private:
        Windows::Foundation::EventRegistrationToken m_sizeChangedToken;
    };
}
