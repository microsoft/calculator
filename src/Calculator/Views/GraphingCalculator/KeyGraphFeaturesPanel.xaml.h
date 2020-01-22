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
        OBSERVABLE_PROPERTY_RW(CalculatorApp::ViewModel::EquationViewModel ^, ViewModel);

    public:
        event Windows::UI::Xaml::RoutedEventHandler ^ KeyGraphFeaturesClosed;

        static Windows::UI::Xaml::Media::SolidColorBrush
            ^ ToSolidColorBrush(Windows::UI::Color color) { return ref new Windows::UI::Xaml::Media::SolidColorBrush(color); }
    private:
        void KeyGraphFeaturesPanel_Loaded(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void BackButton_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

    private:
        CalculatorApp::ViewModel::EquationViewModel ^ m_viewModel;
    };
}
