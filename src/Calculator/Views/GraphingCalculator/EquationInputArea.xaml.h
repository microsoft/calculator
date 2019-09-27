#pragma once

#include "Views/GraphingCalculator/EquationInputArea.g.h"
#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/GraphingCalculator/EquationViewModel.h"
#include "Converters/BooleanNegationConverter.h"

namespace CalculatorApp
{
    public ref class EquationInputArea sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		EquationInputArea();

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector< ViewModel::EquationViewModel^ >^, Equations);
        OBSERVABLE_PROPERTY_RW(bool, IsKeyGraphFeaturesVisible);
        OBSERVABLE_PROPERTY_RW(ViewModel::EquationViewModel ^, KeyGraphFeaturesVM);

        event Windows::UI::Xaml::RoutedEventHandler ^ KeyGraphFeaturesVisibilityChanged;

    private:
        void OnPropertyChanged(Platform::String^ propertyName);
        void OnEquationsPropertyChanged();

        void AddEquationButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AddNewEquation();

        void InputTextBox_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputTextBox_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void InputTextBox_Submitted(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        Windows::UI::Color GetNextLineColor();

    private:
        int m_lastLineColorIndex;
        void EquationTextBox_RemoveButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void EquationTextBox_KeyGraphFeaturesButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void EquationTextBox_EquationButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void KeyGraphFeatures_EquationButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    };
}
