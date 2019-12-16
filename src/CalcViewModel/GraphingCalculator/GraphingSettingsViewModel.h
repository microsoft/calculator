// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "../Common/Utils.h"

namespace CalculatorApp::ViewModel
{
#pragma once
    [Windows::UI::Xaml::Data::Bindable] public ref class GraphingSettingsViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_R(bool, YError);
        OBSERVABLE_PROPERTY_R(bool, XError);
        OBSERVABLE_PROPERTY_R(GraphControl::Grapher ^, Graph);

        GraphingSettingsViewModel();

        property double XMin
        {
            double get()
            {
                return m_XMin;
            }
            void set(double value)
            {
                if (m_XMin == value)
                {
                    return;
                }
                m_XMin = value;
                if (m_Graph != nullptr)
                {
                    m_Graph->XAxisMin = value;
                }
                XError = m_XMin >= m_XMax;
                RaisePropertyChanged("XMin");
                UpdateDisplayRange();
            }
        }

        property double XMax
        {
            double get()
            {
                return m_XMax;
            }
            void set(double value)
            {
                if (m_XMax == value)
                {
                    return;
                }
                m_XMax = value;
                if (m_Graph != nullptr)
                {
                    m_Graph->XAxisMax = value;
                }
                XError = m_XMin >= m_XMax;
                RaisePropertyChanged("XMax");
                UpdateDisplayRange();
            }
        }

        property double YMin
        {
            double get()
            {
                return m_YMin;
            }
            void set(double value)
            {
                if (m_YMin == value)
                {
                    return;
                }
                m_YMin = value;
                if (m_Graph != nullptr)
                {
                    m_Graph->YAxisMin = value;
                }
                YError = m_YMin >= m_YMax;
                RaisePropertyChanged("YMin");
                UpdateDisplayRange();
            }
        }

        property double YMax
        {
            double get()
            {
                return m_YMax;
            }
            void set(double value)
            {
                if (m_YMax == value)
                {
                    return;
                }
                m_YMax = value;
                if (m_Graph != nullptr)
                {
                    m_Graph->YAxisMax = value;
                }
                YError = m_YMin >= m_YMax;
                RaisePropertyChanged("YMax");
                UpdateDisplayRange();
            }
        }

        property int TrigUnit
        {
            int get()
            {
                return m_Graph == nullptr ? (int)Graphing::EvalTrigUnitMode::Invalid : m_Graph->TrigUnitMode;
            }
            void set(int value)
            {
                if (m_Graph == nullptr)
                {
                    return;
                }
                m_Graph->TrigUnitMode = value;
                RaisePropertyChanged(L"TrigUnit");
            }
        }

        property bool TrigModeRadians
        {
            bool get()
            {
                return m_Graph != nullptr && m_Graph->TrigUnitMode == (int)Graphing::EvalTrigUnitMode::Radians;
            }
            void set(bool value)
            {
                if (value && m_Graph != nullptr && m_Graph->TrigUnitMode != (int)Graphing::EvalTrigUnitMode::Radians)
                {
                    m_Graph->TrigUnitMode = (int)Graphing::EvalTrigUnitMode::Radians;
                    RaisePropertyChanged(L"TrigModeRadians");
                    RaisePropertyChanged(L"TrigModeDegrees");
                    RaisePropertyChanged(L"TrigModeGradians");
                }
            }
        }

        property bool TrigModeDegrees
        {
            bool get()
            {
                return m_Graph != nullptr && m_Graph->TrigUnitMode == (int)Graphing::EvalTrigUnitMode::Degrees;
            }
            void set(bool value)
            {
                if (value && m_Graph != nullptr && m_Graph->TrigUnitMode != (int)Graphing::EvalTrigUnitMode::Degrees)
                {
                    m_Graph->TrigUnitMode = (int)Graphing::EvalTrigUnitMode::Degrees;
                    RaisePropertyChanged(L"TrigModeDegrees");
                    RaisePropertyChanged(L"TrigModeRadians");
                    RaisePropertyChanged(L"TrigModeGradians");
                }
            }
        }

        property bool TrigModeGradians
        {
            bool get()
            {
                return m_Graph != nullptr && m_Graph->TrigUnitMode == (int)Graphing::EvalTrigUnitMode::Grads;
            }
            void set(bool value)
            {
                if (value && m_Graph != nullptr && m_Graph->TrigUnitMode != (int)Graphing::EvalTrigUnitMode::Grads)
                {
                    m_Graph->TrigUnitMode = (int)Graphing::EvalTrigUnitMode::Grads;
                    RaisePropertyChanged(L"TrigModeGradians");
                    RaisePropertyChanged(L"TrigModeDegrees");
                    RaisePropertyChanged(L"TrigModeRadians");
                }
            }
        }

    public:
        void UpdateDisplayRange()
        {
            if (m_dontUpdateDisplayRange || m_XError || m_YError || m_Graph == nullptr)
            {
                return;
            }
            m_Graph->SetDisplayRanges(m_XMin, m_XMax, m_YMin, m_YMax);
        }

    public:
        void SetGrapher(GraphControl::Grapher ^ grapher);

    private:
        double m_XMin;
        double m_XMax;
        double m_YMin;
        double m_YMax;
        bool m_dontUpdateDisplayRange;
    };
}
