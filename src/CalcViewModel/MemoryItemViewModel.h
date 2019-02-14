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
        [Windows::UI::Xaml::Data::Bindable]
        public ref class MemoryItemViewModel sealed :
            public Windows::UI::Xaml::Data::INotifyPropertyChanged,
            Windows::UI::Xaml::Data::ICustomPropertyProvider
        {
        public:
            MemoryItemViewModel(StandardCalculatorViewModel^ calcVM) : m_Position(-1), m_calcVM(calcVM) {}
            OBSERVABLE_OBJECT();
            OBSERVABLE_PROPERTY_RW(int, Position);
            OBSERVABLE_PROPERTY_RW(Platform::String^, Value);

            virtual Windows::UI::Xaml::Data::ICustomProperty^ GetCustomProperty(Platform::String^ name)
            {
                return nullptr;
            }

            virtual Windows::UI::Xaml::Data::ICustomProperty^ GetIndexedProperty(Platform::String^ name, Windows::UI::Xaml::Interop::TypeName type)
            {
                return nullptr;
            }

            virtual property Windows::UI::Xaml::Interop::TypeName Type
            {
                Windows::UI::Xaml::Interop::TypeName get()
                {
                    return this->GetType();
                }
            }

            virtual Platform::String^ GetStringRepresentation()
            {
                return Value;
            }

            void Clear();
            void MemoryAdd();
            void MemorySubtract();

        private:
            StandardCalculatorViewModel^ m_calcVM;
        };
    }
}
