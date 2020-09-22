// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common/Utils.h"

namespace CalculatorApp
{
    namespace ViewModel
    {
        ref class StandardCalculatorViewModel;

        /// <summary>
        /// Model representation of a single item in the Memory list
        /// </summary>
        [Windows::UI::Xaml::Data::Bindable] public ref class MemoryItemViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            MemoryItemViewModel(StandardCalculatorViewModel ^ calcVM)
                : m_Position(-1)
                , m_calcVM(calcVM)
            {
            }
            OBSERVABLE_OBJECT();
            OBSERVABLE_PROPERTY_RW(int, Position);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, Value);

            void Clear();
            void MemoryAdd();
            void MemorySubtract();

        private:
            StandardCalculatorViewModel ^ m_calcVM;
        };
    }
}
