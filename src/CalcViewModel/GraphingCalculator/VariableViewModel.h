// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "../Common/Utils.h"
#include "CalcViewModel/Common/LocalizationStringUtil.h"
#include "EquationViewModel.h"

namespace CalculatorApp::ViewModel
{

inline constexpr int DefaultMinMaxRange = 10;

public
    value struct VariableChangedEventArgs sealed
    {
        Platform::String ^ variableName;
        double newValue;
    };

    [Windows::UI::Xaml::Data::Bindable] public ref class VariableViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        VariableViewModel(Platform::String ^ name, GraphControl::Variable ^ variable)
            : m_Name(name)
            , m_variable{ variable }
            , m_SliderSettingsVisible(false)
        {
        }

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_R(Platform::String ^, Name);
        OBSERVABLE_PROPERTY_RW(bool, SliderSettingsVisible);

        property double Min
        {
            double get()
            {
                return m_variable->Min;
            }
            void set(double value)
            {
                if (m_variable->Min != value)
                {
                    if (value >= m_variable->Max)
                    {
                        m_variable->Max = value + DefaultMinMaxRange;
                        RaisePropertyChanged("Max");
                    }

                    m_variable->Min = value;
                    RaisePropertyChanged("Min");
                }
            }
        }

        property double Step
        {
            double get()
            {
                return m_variable->Step;
            }
            void set(double value)
            {
                if (m_variable->Step != value)
                {
                    m_variable->Step = value;
                    RaisePropertyChanged("Step");
                }
            }
        }

        property double Max
        {
            double get()
            {
                return m_variable->Max;
            }
            void set(double value)
            {
                if (m_variable->Max != value)
                {
                    if (value <= m_variable->Min)
                    {
                        m_variable->Min = value - DefaultMinMaxRange;
                        RaisePropertyChanged("Min");
                    }

                    m_variable->Max = value;
                    RaisePropertyChanged("Max");
                }
            }
        }

        event Windows::Foundation::EventHandler<VariableChangedEventArgs> ^ VariableUpdated;

        property double Value
        {
            double get()
            {
                return m_variable->Value;
            }
            void set(double value)
            {
                if (value < m_variable->Min)
                {
                    m_variable->Min = value;
                    RaisePropertyChanged(L"Min");
                }
                else if (value > m_variable->Max)
                {
                    m_variable->Max = value;
                    RaisePropertyChanged(L"Max");
                }

                if (m_variable->Value != value)
                {
                    m_variable->Value = value;
                    VariableUpdated(this, VariableChangedEventArgs{ Name, value });
                    RaisePropertyChanged(L"Value");
                }
            }
        }

        property Platform::String ^ VariableAutomationName
        {
            Platform::String ^ get()
            {
                return CalculatorApp::ViewModel::Common::LocalizationStringUtil::GetLocalizedString(
                           CalculatorApp::ViewModel::Common::AppResourceProvider::GetInstance()->GetResourceString(L"VariableListViewItem"), Name);
            }
        }

    private:
        GraphControl::Variable ^ m_variable;
    };
}
