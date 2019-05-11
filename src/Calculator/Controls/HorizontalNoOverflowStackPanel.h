// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// HorizontalNoOverflowStackPanel.h
// Declaration of the HorizontalNoOverflowStackPanel class
//

#pragma once
#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        ref class HorizontalNoOverflowStackPanel : public Windows::UI::Xaml::Controls::Panel
        {
            DEPENDENCY_PROPERTY_OWNER(HorizontalNoOverflowStackPanel);
            // Prioritize the last item over all other items (except the first one)
            internal : HorizontalNoOverflowStackPanel()
            {
            }

        protected:
            virtual Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size availableSize) override;
            virtual Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size finalSize) override;
            virtual bool ShouldPrioritizeLastItem();
        };
    }
}
