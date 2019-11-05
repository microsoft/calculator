#pragma once

#include "Views/GraphingCalculator/EquationInputArea.g.h"
#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/GraphingCalculator/EquationViewModel.h"
#include "EquationStylePanelControl.xaml.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "Controls/EquationTextBox.h"

namespace CalculatorApp
{
    public ref class EquationInputArea sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		EquationInputArea();

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector< ViewModel::EquationViewModel^ >^, Equations);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Windows::UI::Xaml::Media::SolidColorBrush ^> ^, AvailableColors);


    private:
        void OnPropertyChanged(Platform::String^ propertyName);
        void OnEquationsPropertyChanged();

        void AddEquationButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AddNewEquation();

        void InputTextBox_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputTextBox_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputTextBox_Submitted(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        void OnHighContrastChanged(Windows::UI::ViewManagement::AccessibilitySettings ^ sender, Platform::Object ^ args);
        void ReloadAvailableColors(bool isHighContrast);

    private:
        Windows::UI::ViewManagement::AccessibilitySettings ^ m_accessibilitySettings;
        int m_lastLineColorIndex;
        void EquationTextBox_RemoveButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void EquationTextBoxLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    };
}
