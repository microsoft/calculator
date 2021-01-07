// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/Calculator.xaml.h"
#include "Views/MainPage.g.h"
#include "Views/DateCalculator.xaml.h"
#include "Views/GraphingCalculator/GraphingCalculator.xaml.h"
#include "Views/UnitConverter.xaml.h"
#include "CalcViewModel/ApplicationViewModel.h"

namespace CalculatorApp
{
    namespace WUC = Windows::UI::Core;
    namespace WUXI = Windows::UI::Xaml::Input;
    namespace WUXD = Windows::UI::Xaml::Data;
    namespace WUXC = Windows::UI::Xaml::Controls;

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
public
    ref class MainPage sealed
    {
    public:
        MainPage();
        property CalculatorApp::ViewModel::ApplicationViewModel
            ^ Model { CalculatorApp::ViewModel::ApplicationViewModel ^ get() { return m_model; } }

            void
            UnregisterEventHandlers();

        void SetDefaultFocus();
        void SetHeaderAutomationName();

        Windows::Foundation::Collections::IObservableVector<
            Platform::Object
            ^> ^ CreateUIElementsForCategories(_In_ Windows::Foundation::Collections::IObservableVector<CalculatorApp::Common::NavCategoryGroup ^> ^ categories);

    protected:
        void OnNavigatedTo(_In_ Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e) override;

    private:
        void WindowSizeChanged(_In_ Platform::Object ^ sender, _In_ Windows::UI::Core::WindowSizeChangedEventArgs ^ e);
        void OnAppPropertyChanged(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::Data::PropertyChangedEventArgs ^ e);

        void OnNavLoaded(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnNavPaneOpening(_In_ Microsoft::UI::Xaml::Controls::NavigationView ^ sender, _In_ Platform::Object ^ args);
        void OnNavPaneOpened(_In_ Microsoft::UI::Xaml::Controls::NavigationView ^ sender, _In_ Platform::Object ^ args);
        void OnNavPaneClosed(_In_ Microsoft::UI::Xaml::Controls::NavigationView ^ sender, _In_ Platform::Object ^ args);
        void OnNavSelectionChanged(_In_ Platform::Object ^ sender, _In_ Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs ^ e);
        void OnNavItemInvoked(
            Microsoft::UI::Xaml::Controls::NavigationView ^ /*sender*/,
            _In_ Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs ^ e);

        void OnAboutButtonClick(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::Controls::ItemClickEventArgs ^ e);
        void OnAboutButtonKeyDown(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);
        void OnAboutFlyoutOpened(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ e);
        void OnAboutFlyoutClosed(_In_ Platform::Object ^ sender, _In_ Platform::Object ^ e);
        void AlwaysOnTopButtonClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void TitleBarAlwaysOnTopButtonClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        Microsoft::UI::Xaml::Controls::NavigationViewItemHeader ^ CreateNavViewHeaderFromGroup(CalculatorApp::Common::NavCategoryGroup ^ group);
        Microsoft::UI::Xaml::Controls::NavigationViewItem ^ CreateNavViewItemFromCategory(CalculatorApp::Common::NavCategory ^ category);

        void ShowHideControls(CalculatorApp::Common::ViewMode mode);
        void UpdateViewState();
        void UpdatePanelViewState();
        void OnHighContrastChanged(Windows::UI::ViewManagement::AccessibilitySettings ^ sender, Platform::Object ^ args);

        void OnPageLoaded(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void App_Suspending(Object ^ sender, Windows::ApplicationModel::SuspendingEventArgs ^ e);

        void EnsureCalculator();
        void EnsureDateCalculator();
        void EnsureGraphingCalculator();
        void EnsureConverter();
        void ShowAboutPage();

        void AnnounceCategoryName();

        CalculatorApp::Calculator ^ m_calculator;
		GraphingCalculator^ m_graphingCalculator;
        CalculatorApp::UnitConverter ^ m_converter;
        CalculatorApp::DateCalculator ^ m_dateCalculator;
        Windows::Foundation::EventRegistrationToken m_windowSizeEventToken;
        CalculatorApp::ViewModel::ApplicationViewModel ^ m_model;
        Windows::Foundation::EventRegistrationToken m_accessibilitySettingsToken;
        Windows::UI::ViewManagement::AccessibilitySettings ^ m_accessibilitySettings;
    };
}
