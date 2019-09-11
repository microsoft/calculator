#pragma once

#include "Views/GraphingCalculator/EquationInputArea.g.h"
#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/GraphingCalculator/EquationViewModel.h"
#include "StylePanel.xaml.h"
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

    private:
        void OnPropertyChanged(Platform::String^ propertyName);
        void OnEquationsPropertyChanged();

        void AddEquationButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AddNewEquation();

        void InputTextBox_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputTextBox_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputTextBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);

        Windows::UI::Xaml::Media::SolidColorBrush ^ GetNextLineColor();

    private:
        int m_lastLineColorIndex;
        void EquationTextBox_RemoveButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
