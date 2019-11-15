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
        DEPENDENCY_PROPERTY_OWNER(GraphingCalculator);
        COMMAND_FOR_METHOD(ZoomOutButtonPressed, GraphingCalculator::OnZoomOutCommand);
        COMMAND_FOR_METHOD(ZoomInButtonPressed, GraphingCalculator::OnZoomInCommand);
        COMMAND_FOR_METHOD(ZoomResetButtonPressed, GraphingCalculator::OnZoomResetCommand);
        OBSERVABLE_PROPERTY_RW(bool, IsKeyGraphFeaturesVisible);
        DEPENDENCY_PROPERTY(bool, IsSmallState);

        property CalculatorApp::ViewModel::GraphingCalculatorViewModel^ ViewModel
        {
            CalculatorApp::ViewModel::GraphingCalculatorViewModel^ get();
            void set(CalculatorApp::ViewModel::GraphingCalculatorViewModel^ vm);
        }

        Windows::UI::Xaml::Visibility ShouldDisplayPanel(bool isSmallState, bool isEquationModeActivated, bool isGraphPanel);
        Platform::String ^ GetInfoForSwitchModeToggleButton(bool isChecked);
    private:
        void GraphingCalculator_DataContextChanged(Windows::UI::Xaml::FrameworkElement ^ sender, Windows::UI::Xaml::DataContextChangedEventArgs ^ args);

        void GraphVariablesUpdated(Platform::Object ^ sender, Object ^ args);
        void OnVariableChanged(Platform::Object ^ sender, CalculatorApp::ViewModel::VariableChangedEventArgs args);

        void TextBoxLosingFocus(Windows::UI::Xaml::Controls::TextBox ^ textbox, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args);
        void TextBoxKeyDown(Windows::UI::Xaml::Controls::TextBox ^ textbox, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);
        void SubmitTextbox(Windows::UI::Xaml::Controls::TextBox ^ textbox);

        void OnZoomInCommand(Object ^ parameter);
        void OnZoomOutCommand(Object ^ parameter);
        void OnZoomResetCommand(Object ^ parameter);

        double validateDouble(Platform::String ^ value, double defaultValue);

        CalculatorApp::ViewModel::GraphingCalculatorViewModel ^ m_viewModel;

        void OnShareClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        void OnShowTracePopupChanged(bool newValue);
        void OnTracePointChanged(Windows::Foundation::Point newPoint);

    private:
        Windows::Foundation::EventRegistrationToken m_dataRequestedToken;
        void OnDataRequested(
            Windows::ApplicationModel::DataTransfer::DataTransferManager ^ sender,
            Windows::ApplicationModel::DataTransfer::DataRequestedEventArgs ^ e);

        void TextBoxGotFocus(Windows::UI::Xaml::Controls::TextBox ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnActiveTracingClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnGraphLostFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnLoosingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args);
        void OnEquationKeyGraphFeaturesVisibilityChanged(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnKeyGraphFeaturesClosed(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        bool ActiveTracingOn;
        void SwitchModeToggleButton_Checked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    };

}
