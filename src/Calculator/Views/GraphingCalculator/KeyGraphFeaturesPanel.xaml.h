// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views\GraphingCalculator\KeyGraphFeaturesPanel.g.h"
#include "CalcViewModel\GraphingCalculator\EquationViewModel.h"
#include "Controls/MathRichEditBox.h"
#include "Controls/EquationTextBox.h"
#include "TemplateSelectors/KeyGraphFeaturesTemplateSelector.h"

namespace CalculatorApp
{
public
    ref class KeyGraphFeaturesPanel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        KeyGraphFeaturesPanel();

        OBSERVABLE_OBJECT();

    public:
        property CalculatorApp::ViewModel::EquationViewModel ^ ViewModel
        {
            CalculatorApp::ViewModel::EquationViewModel ^ get()
            {
                return m_viewModel;
            }
            void set(CalculatorApp::ViewModel::EquationViewModel ^ value)
            {
                m_viewModel = value;
                RaisePropertyChanged(L"EquationViewModel");
                if (value != nullptr)
                {
                    SetEquationTextBoxProperties();
                }
            }
        }

        event Windows::UI::Xaml::RoutedEventHandler ^ KeyGraphFeaturesClosed;

    private:
        void EquationButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SetEquationTextBoxProperties();
        void EquationInputTextBox_Loaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

    private:
        CalculatorApp::ViewModel::EquationViewModel ^ m_viewModel;
    };
}
