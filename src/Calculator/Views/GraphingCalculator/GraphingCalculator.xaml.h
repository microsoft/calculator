// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views\GraphingCalculator\GraphingCalculator.g.h"
#include "CalcViewModel\GraphingCalculator\GraphingCalculatorViewModel.h"
#include "Views\NumberPad.xaml.h"
#include "Views\GraphingCalculator\KeyGraphFeaturesPanel.xaml.h"
#include "Views\GraphingCalculator\GraphingNumPad.xaml.h"

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

        static Windows::UI::Xaml::Visibility ShouldDisplayPanel(bool isSmallState, bool isEquationModeActivated, bool isGraphPanel);
        static Platform::String ^ GetInfoForSwitchModeToggleButton(bool isChecked);
        static Windows::UI::Xaml::Visibility ManageEditVariablesButtonVisibility(unsigned int numberOfVariables);
        static Platform::String ^ GetTracingLegend(Platform::IBox<bool> ^ isTracing);
    private:
        void GraphingCalculator_DataContextChanged(Windows::UI::Xaml::FrameworkElement ^ sender, Windows::UI::Xaml::DataContextChangedEventArgs ^ args);

        void OnVariableChanged(Platform::Object ^ sender, CalculatorApp::ViewModel::VariableChangedEventArgs args);
        void OnEquationsVectorChanged(
            Windows::Foundation::Collections::IObservableVector<CalculatorApp::ViewModel::EquationViewModel ^> ^ sender,
            Windows::Foundation::Collections::IVectorChangedEventArgs ^ event);

        void OnZoomInCommand(Object ^ parameter);
        void OnZoomOutCommand(Object ^ parameter);
        void OnZoomResetCommand(Object ^ parameter);

        double validateDouble(Platform::String ^ value, double defaultValue);

        void OnShareClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        void OnShowTracePopupChanged(bool newValue);
        void OnTracePointChanged(Windows::Foundation::Point newPoint);
    private:
        void OnDataRequested(
            Windows::ApplicationModel::DataTransfer::DataTransferManager ^ sender,
            Windows::ApplicationModel::DataTransfer::DataRequestedEventArgs ^ e);

        void TextBoxGotFocus(Windows::UI::Xaml::Controls::TextBox ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void GraphingControl_LostFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void GraphingControl_LosingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args);
        void GraphingControl_VariablesUpdated(Platform::Object ^ sender, Object ^ args);
        void OnEquationKeyGraphFeaturesRequested(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnKeyGraphFeaturesClosed(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SwitchModeToggleButton_Checked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void TraceValuePopup_SizeChanged(Platform::Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e);
        void PositionGraphPopup();
        void ActiveTracing_Checked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ActiveTracing_Unchecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ActiveTracing_KeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args);
        void ActiveTracing_PointerCaptureLost(Platform::Object ^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e);
    private:
        Windows::Foundation::EventRegistrationToken m_dataRequestedToken;
        Windows::Foundation::EventRegistrationToken m_vectorChangedToken;
        Windows::Foundation::EventRegistrationToken m_variableUpdatedToken;
        Windows::Foundation::EventRegistrationToken m_activeTracingKeyUpToken;
        Windows::Foundation::EventRegistrationToken m_ActiveTracingPointerCaptureLost;
        CalculatorApp::ViewModel::GraphingCalculatorViewModel ^ m_viewModel;
    };

}
