// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/GraphingCalculator/EquationInputArea.g.h"
#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/GraphingCalculator/EquationViewModel.h"
#include "EquationStylePanelControl.xaml.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "Controls/EquationTextBox.h"
#include "Converters/BooleanNegationConverter.h"
#include "Controls/MathRichEditBox.h"

namespace CalculatorApp
{
    public ref class EquationInputArea sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		EquationInputArea();

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector< ViewModel::EquationViewModel^ >^, Equations);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Windows::UI::Xaml::Media::SolidColorBrush ^> ^, AvailableColors);
        event Windows::Foundation::EventHandler<ViewModel::EquationViewModel ^> ^ KeyGraphFeaturesRequested;
        static Windows::UI::Xaml::Media::SolidColorBrush
            ^ ToSolidColorBrush(Windows::UI::Color color) { return ref new Windows::UI::Xaml::Media::SolidColorBrush(color); }
    
    private:
        void OnPropertyChanged(Platform::String^ propertyName);
        void OnEquationsPropertyChanged();

        void AddNewEquation();

        void InputTextBox_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputTextBox_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputTextBox_Submitted(Platform::Object ^ sender, CalculatorApp::Controls::EquationSubmissionSource e);

        void OnHighContrastChanged(Windows::UI::ViewManagement::AccessibilitySettings ^ sender, Platform::Object ^ args);
        void ReloadAvailableColors(bool isHighContrast);
        void FocusEquationTextBox(ViewModel::EquationViewModel ^ equation);

        void EquationTextBox_RemoveButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBox_KeyGraphFeaturesButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBox_EquationButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBoxLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

    private:
        Windows::UI::ViewManagement::AccessibilitySettings ^ m_accessibilitySettings;
        int m_lastLineColorIndex;
        int m_lastFunctionLabelIndex;
        ViewModel::EquationViewModel ^ m_equationToFocus;
    };
}
