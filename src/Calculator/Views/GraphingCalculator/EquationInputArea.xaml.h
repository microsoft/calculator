// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/GraphingCalculator/EquationInputArea.g.h"
#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/GraphingCalculator/EquationViewModel.h"
#include "CalcViewModel/GraphingCalculator/GraphingCalculatorViewModel.h"
#include "EquationStylePanelControl.xaml.h"
#include "Common/KeyboardShortcutManager.h"
#include "Controls/EquationTextBox.h"
#include "Converters/BooleanNegationConverter.h"
#include "Controls/MathRichEditBox.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "Utils/DispatcherTimerDelayer.h"

namespace CalculatorApp
{
public
    ref class EquationInputArea sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        EquationInputArea();

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<ViewModel::EquationViewModel ^> ^, Equations);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<ViewModel::VariableViewModel ^> ^, Variables);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Windows::UI::Xaml::Media::SolidColorBrush ^> ^, AvailableColors);
        OBSERVABLE_PROPERTY_RW(bool, IsMatchAppTheme);
        event Windows::Foundation::EventHandler<ViewModel::EquationViewModel ^> ^ KeyGraphFeaturesRequested;
        event Windows::Foundation::EventHandler<CalculatorApp::Controls::MathRichEditBoxFormatRequest ^> ^ EquationFormatRequested;

    public:
        static Windows::UI::Xaml::Visibility ManageEditVariablesButtonVisibility(unsigned int numberOfVariables);
        static bool EquationInputArea::ManageEditVariablesButtonLoaded(unsigned int numberOfVariables);

        static Platform::String ^ GetChevronIcon(bool isCollapsed);

        static Windows::UI::Xaml::Media::SolidColorBrush
            ^ ToSolidColorBrush(Windows::UI::Color color) { return ref new Windows::UI::Xaml::Media::SolidColorBrush(color); }

        static Windows::UI::Xaml::Media::SolidColorBrush ^ GetForegroundColor(Windows::UI::Color lineColor);

        void FocusEquationTextBox(ViewModel::EquationViewModel ^ equation);
    private:
        void OnPropertyChanged(Platform::String ^ propertyName);
        void OnEquationsPropertyChanged();

        void AddNewEquation();

        void EquationTextBox_GotFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBox_LostFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBox_Submitted(Platform::Object ^ sender, CalculatorApp::Controls::MathRichEditBoxSubmission ^ e);

        void OnHighContrastChanged(Windows::UI::ViewManagement::AccessibilitySettings ^ sender, Platform::Object ^ args);
        void ReloadAvailableColors(bool isHighContrast, bool reassignColors);
        void OnColorValuesChanged(Windows::UI::ViewManagement::UISettings ^ sender, Platform::Object ^ args);

        void EquationTextBox_RemoveButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBox_KeyGraphFeaturesButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBox_EquationButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBox_Loaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBox_DataContextChanged(Windows::UI::Xaml::FrameworkElement ^ sender, Windows::UI::Xaml::DataContextChangedEventArgs ^ args);
        void FocusEquationIfNecessary(_In_ CalculatorApp::Controls::EquationTextBox ^ textBox);

        double validateDouble(Platform::String ^ value, double defaultValue);
        void TextBoxGotFocus(Windows::UI::Xaml::Controls::TextBox ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void TextBoxLosingFocus(Windows::UI::Xaml::Controls::TextBox ^ textbox, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args);
        void TextBoxKeyDown(Windows::UI::Xaml::Controls::TextBox ^ textbox, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);
        void SubmitTextbox(Windows::UI::Xaml::Controls::TextBox ^ textbox);
        void VariableAreaClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void VariableAreaButtonTapped(Platform::Object ^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^ e);
        void VariableAreaTapped(Platform::Object ^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^ e);
        void EquationTextBox_EquationFormatRequested(Platform::Object ^ sender, CalculatorApp::Controls::MathRichEditBoxFormatRequest ^ e);
        void Slider_ValueChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^ e);

        CalculatorApp::ViewModel::EquationViewModel ^ GetViewModelFromEquationTextBox(Platform::Object ^ sender);

        void ToggleVariableArea(CalculatorApp::ViewModel::VariableViewModel ^ selectedVariableViewModel);

        Windows::UI::ViewManagement::AccessibilitySettings ^ m_accessibilitySettings;
        Windows::UI::ViewManagement::UISettings ^ m_uiSettings;
        int m_lastLineColorIndex;
        int m_lastFunctionLabelIndex;
        bool m_isHighContrast;
        ViewModel::EquationViewModel ^ m_equationToFocus;
        Platform::Collections::Map<Platform::String ^, CalculatorApp::DispatcherTimerDelayer ^> ^ variableSliders;
    };
}
