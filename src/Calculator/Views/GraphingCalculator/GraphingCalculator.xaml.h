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

    private:
        void GraphingCalculator_DataContextChanged(Windows::UI::Xaml::FrameworkElement^ sender, Windows::UI::Xaml::DataContextChangedEventArgs^ args);

        void GraphVariablesUpdated(Platform::Object^ sender, Object^ args);
        void OnVariableChanged(Platform::Object^ sender, CalculatorApp::ViewModel::VariableChangedEventArgs args);

        void BeforeTextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxBeforeTextChangingEventArgs^ args);
        void TextBoxLosingFocus(Windows::UI::Xaml::Controls::TextBox^ textbox, Windows::UI::Xaml::Input::LosingFocusEventArgs^ args);
        void TextBoxKeyDown(Windows::UI::Xaml::Controls::TextBox^ textbox, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
        void ValidateTextBox(Windows::UI::Xaml::Controls::TextBox^ textbox);

        CalculatorApp::ViewModel::GraphingCalculatorViewModel^ m_viewModel;
        bool m_ignoreNextTextChanged;

    };
}
