#pragma once

#include "Views\GraphingCalculator\GraphingCalculator.g.h"
#include "CalcViewModel\GraphingCalculator\GraphingCalculatorViewModel.h"
#include "Views\NumberPad.xaml.h"

namespace CalculatorApp
{
    public ref class GraphingCalculator sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        GraphingCalculator();

        OBSERVABLE_OBJECT();

        property CalculatorApp::ViewModel::GraphingCalculatorViewModel^ ViewModel
        {
            CalculatorApp::ViewModel::GraphingCalculatorViewModel^ get();
            void set(CalculatorApp::ViewModel::GraphingCalculatorViewModel^ vm);
        }

        static property double zoomButtonDelta
        {
            double get()
            {
                return 50;
            }
        }

    private:
        void GraphingCalculator_DataContextChanged(Windows::UI::Xaml::FrameworkElement^ sender, Windows::UI::Xaml::DataContextChangedEventArgs^ args);

        void GraphVariablesUpdated(Platform::Object^ sender, Object^ args);
        void OnVariableChanged(Platform::Object^ sender, CalculatorApp::ViewModel::VariableChangedEventArgs args);

        void TextBoxLosingFocus(Windows::UI::Xaml::Controls::TextBox^ textbox, Windows::UI::Xaml::Input::LosingFocusEventArgs^ args);
        void TextBoxKeyDown(Windows::UI::Xaml::Controls::TextBox^ textbox, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
        void SubmitTextbox(Windows::UI::Xaml::Controls::TextBox^ textbox);

        void ZoomInButtonClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ZoomOutButtonClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ZoomResetButtonClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        double validateDouble(Platform::String^ value, double defaultValue);

        CalculatorApp::ViewModel::GraphingCalculatorViewModel^ m_viewModel;

        void TextBoxGotFocus(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
