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

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(CalculatorApp::ViewModel::EquationViewModel ^, ViewModel);
        event Windows::UI::Xaml::RoutedEventHandler ^ KeyGraphFeaturesClosed;

    private:

        void OnPropertyChanged(Platform::String ^ propertyName);
        void EquationButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SetEquationTextBoxProperties();
        void EquationInputTextBox_Loaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    };
}
