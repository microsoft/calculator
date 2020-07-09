// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common/Utils.h"

namespace CalculatorApp
{
    namespace ViewModel
    {
        [Windows::UI::Xaml::Data::Bindable] public ref class FinanceCalculatorViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:

            FinanceCalculatorViewModel();

            OBSERVABLE_OBJECT();
            OBSERVABLE_PROPERTY_RW(bool, isCompoundMode);
            OBSERVABLE_PROPERTY_RW(bool, isSplitBill);
        };
    }
}
