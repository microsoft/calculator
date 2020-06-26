// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/Calculator.g.h"
#include "Converters/BooleanNegationConverter.h"
#include "Converters/ExpressionItemTemplateSelector.h"
#include "Converters/VisibilityNegationConverter.h"
#include "CalcViewModel/Common/Automation/NarratorNotifier.h"
#include "Controls/CalculatorButton.h"
#include "Controls/CalculationResult.h"
#include "Controls/OverflowTextBlock.h"
#include "Controls/OperatorPanelListView.h"
#include "Controls/OperatorPanelButton.h"
#include "Controls/EquationTextBox.h"
#include "Controls/MathRichEditBox.h"
#include "CalcViewModel/HistoryViewModel.h"
#include "TemplateSelectors/KeyGraphFeaturesTemplateSelector.h"
#include "Views/CalculatorProgrammerDisplayPanel.xaml.h"
#include "Views/CalculatorProgrammerOperators.xaml.h"
#include "Views/CalculatorScientificAngleButtons.xaml.h"
#include "Views/HistoryList.xaml.h"
#include "Views/Memory.xaml.h"
#include "Views/OperatorsPanel.xaml.h"
#include "Views/StateTriggers/ControlSizeTrigger.h"

namespace CalculatorApp
{
    struct FontTable
    {
        std::wstring numericSystem;
        double fullFont;
        double fullFontMin;
        double portraitMin;
        double snapFont;
        double fullNumPadFont;
        double snapScientificNumPadFont;
        double portraitScientificNumPadFont;
    };

public
    delegate void FullscreenFlyoutClosedEventHandler();

    [Windows::Foundation::Metadata::WebHostHidden] public ref class Calculator sealed
    {
    public:
        event FullscreenFlyoutClosedEventHandler ^ FullscreenFlyoutClosed;

        Calculator();
        property CalculatorApp::ViewModel::StandardCalculatorViewModel^ Model
        {
            CalculatorApp::ViewModel::StandardCalculatorViewModel^ get() {
                return static_cast<CalculatorApp::ViewModel::StandardCalculatorViewModel^>(this->DataContext);
            }
        }

        DEPENDENCY_PROPERTY_OWNER(Calculator);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(bool, IsStandard, false);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(bool, IsScientific, false);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(bool, IsProgrammer, false);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(bool, IsAlwaysOnTop, false);

        COMMAND_FOR_METHOD(HistoryButtonPressed, Calculator::ToggleHistoryFlyout);

        void AnimateCalculator(bool resultAnimate);
        void InitializeHistoryView(CalculatorApp::ViewModel::HistoryViewModel ^ historyVM);
        void UpdatePanelViewState();
        void UnregisterEventHandlers();

        void CloseHistoryFlyout();
        void CloseMemoryFlyout();

        void SetDefaultFocus();

        // Methods used by native bindings
        static Windows::UI::Xaml::Visibility ShouldDisplayHistoryButton(bool isAlwaysOnTop, bool isProgrammer, Windows::UI::Xaml::Visibility dockPanelVisibility);
    private:
        void OnLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        void LoadResourceStrings();

        void UpdateViewState();
        void UpdateMemoryState();
        void UpdateHistoryState();

        void OnContextRequested(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::ContextRequestedEventArgs ^ e);
        void OnContextCanceled(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnIsScientificPropertyChanged(bool oldValue, bool newValue);
        void OnIsProgrammerPropertyChanged(bool oldValue, bool newValue);
        void OnIsStandardPropertyChanged(bool oldValue, bool newValue);
        void OnIsAlwaysOnTopPropertyChanged(bool oldValue, bool newValue);
        void OnIsInErrorPropertyChanged();
        void OnCalcPropertyChanged(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::Data::PropertyChangedEventArgs ^ e);
        void OnLayoutVisualStateCompleted(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ e);
        void OnModeVisualStateCompleted(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ e);
        void OnErrorVisualStateCompleted(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ e);
        void OnDisplayVisualStateCompleted(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ e);
        void EnsureScientific();
        void EnsureProgrammer();
        void SetFontSizeResources();
        Platform::String ^ GetCurrentLayoutState();
        void Calculator_SizeChanged(Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e);

    private:
        Windows::UI::Xaml::Controls::ListView ^ m_tokenList;
        Windows::UI::Xaml::Controls::MenuFlyout ^ m_displayFlyout;
        bool m_doAnimate;
        bool m_resultAnimate;
        bool m_isLastAnimatedInScientific;
        bool m_isLastAnimatedInProgrammer;
        bool m_IsLastFlyoutMemory = false;
        bool m_IsLastFlyoutHistory = false;

        Platform::String ^ m_openMemoryFlyoutAutomationName;
        Platform::String ^ m_closeMemoryFlyoutAutomationName;
        Platform::String ^ m_openHistoryFlyoutAutomationName;
        Platform::String ^ m_closeHistoryFlyoutAutomationName;
        Platform::String ^ m_dockPanelHistoryMemoryLists;
        Platform::String ^ m_dockPanelMemoryList;

        Windows::UI::Xaml::Controls::PivotItem ^ m_pivotItem;
        bool m_IsDigit = false;
        Memory ^ m_memory;
        void HistoryFlyout_Opened(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ args);
        void HistoryFlyout_Closing(
            _In_ Windows::UI::Xaml::Controls::Primitives::FlyoutBase ^ sender,
            _In_ Windows::UI::Xaml::Controls::Primitives::FlyoutBaseClosingEventArgs ^ args);
        void HistoryFlyout_Closed(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ args);
        void OnHideHistoryClicked();
        void OnHideMemoryClicked();
        void OnHistoryItemClicked(_In_ CalculatorApp::ViewModel::HistoryItemViewModel ^ e);
        void ToggleHistoryFlyout(Platform::Object ^ parameter);
        void ToggleMemoryFlyout();
        CalculatorApp::HistoryList ^ m_historyList;
        bool m_fIsHistoryFlyoutOpen;
        bool m_fIsMemoryFlyoutOpen;
        void OnMemoryFlyoutOpened(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ args);
        void OnMemoryFlyoutClosing(
            _In_ Windows::UI::Xaml::Controls::Primitives::FlyoutBase ^ sender,
            _In_ Windows::UI::Xaml::Controls::Primitives::FlyoutBaseClosingEventArgs ^ args);
        void OnMemoryFlyoutClosed(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ args);
        void SetChildAsMemory();
        void SetChildAsHistory();
        Memory ^ GetMemory();
        void EnableControls(bool enable);
        void EnableMemoryControls(bool enable);
        void OnMemoryFlyOutTapped(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::Input::TappedRoutedEventArgs ^ e);
        void OnHistoryFlyOutTapped(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::Input::TappedRoutedEventArgs ^ e);
        void DockPanelTapped(_In_ Windows::UI::Xaml::Input::TappedRoutedEventArgs ^ e);
        void OnHistoryAccessKeyInvoked(_In_ Windows::UI::Xaml::UIElement ^ sender, _In_ Windows::UI::Xaml::Input::AccessKeyInvokedEventArgs ^ args);
        void OnMemoryAccessKeyInvoked(_In_ Windows::UI::Xaml::UIElement ^ sender, _In_ Windows::UI::Xaml::Input::AccessKeyInvokedEventArgs ^ args);
        void OnVisualStateChanged(Platform::Object ^ sender, Windows::UI::Xaml::VisualStateChangedEventArgs ^ e);

   };
}
