// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views\GraphingCalculator\GraphingCalculator.g.h"
#include "CalcViewModel\GraphingCalculator\GraphingCalculatorViewModel.h"
#include "Views\NumberPad.xaml.h"
#include "Views\GraphingCalculator\KeyGraphFeaturesPanel.xaml.h"

namespace CalculatorApp
{
    constexpr double zoomInScale = 1 / 1.0625;
    constexpr double zoomOutScale = 1.0625;

    public ref class GraphingCalculator sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        GraphingCalculator();

        OBSERVABLE_OBJECT();
        COMMAND_FOR_METHOD(ZoomOutButtonPressed, GraphingCalculator::OnZoomOutCommand);
        COMMAND_FOR_METHOD(ZoomInButtonPressed, GraphingCalculator::OnZoomInCommand);
        COMMAND_FOR_METHOD(ZoomResetButtonPressed, GraphingCalculator::OnZoomResetCommand);
        OBSERVABLE_PROPERTY_RW(bool, IsKeyGraphFeaturesVisible);

        property CalculatorApp::ViewModel::GraphingCalculatorViewModel^ ViewModel
        {
            CalculatorApp::ViewModel::GraphingCalculatorViewModel^ get();
            void set(CalculatorApp::ViewModel::GraphingCalculatorViewModel^ vm);
        }

    private:
        void GraphingCalculator_DataContextChanged(Windows::UI::Xaml::FrameworkElement^ sender, Windows::UI::Xaml::DataContextChangedEventArgs^ args);

        void GraphVariablesUpdated(Platform::Object^ sender, Object^ args);
        void OnVariableChanged(Platform::Object^ sender, CalculatorApp::ViewModel::VariableChangedEventArgs args);
        void OnEquationsVectorChanged(
            Windows::Foundation::Collections::IObservableVector<CalculatorApp::ViewModel::EquationViewModel ^> ^ sender,
            Windows::Foundation::Collections::IVectorChangedEventArgs ^ event);

        void TextBoxLosingFocus(Windows::UI::Xaml::Controls::TextBox^ textbox, Windows::UI::Xaml::Input::LosingFocusEventArgs^ args);
        void TextBoxKeyDown(Windows::UI::Xaml::Controls::TextBox^ textbox, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
        void SubmitTextbox(Windows::UI::Xaml::Controls::TextBox^ textbox);

        void OnZoomInCommand(Object ^ parameter);
        void OnZoomOutCommand(Object ^ parameter);
        void OnZoomResetCommand(Object ^ parameter);

        double validateDouble(Platform::String^ value, double defaultValue);

        CalculatorApp::ViewModel::GraphingCalculatorViewModel^ m_viewModel;

        void OnShareClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void OnShowTracePopupChanged(bool newValue);
        void OnTracePointChanged(Windows::Foundation::Point newPoint);


    private:
        Windows::Foundation::EventRegistrationToken m_dataRequestedToken;
        Windows::Foundation::EventRegistrationToken m_vectorChangedToken;
        Windows::Foundation::EventRegistrationToken m_variableUpdatedToken;
        void OnDataRequested(Windows::ApplicationModel::DataTransfer::DataTransferManager^ sender, Windows::ApplicationModel::DataTransfer::DataRequestedEventArgs^ e);

        void TextBoxGotFocus(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnActiveTracingClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnGraphLostFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnLoosingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args);
        void OnEquationKeyGraphFeaturesRequested(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnKeyGraphFeaturesClosed(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        bool ActiveTracingOn;
    };

}
