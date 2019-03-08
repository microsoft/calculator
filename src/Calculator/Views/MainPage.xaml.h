// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/Calculator.xaml.h"
#include "Views/MainPage.g.h"
#include "Views/DateCalculator.xaml.h"
#include "Views/UnitConverter.xaml.h"
#include "CalcViewModel/ApplicationViewModel.h"
#include "Views/TitleBar.xaml.h"

namespace CalculatorApp
{
    namespace WUC = Windows::UI::Core;
    namespace WUXI = Windows::UI::Xaml::Input;
    namespace WUXD = Windows::UI::Xaml::Data;
    namespace WUXC = Windows::UI::Xaml::Controls;

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public ref class MainPage sealed
    {
    public:
        MainPage();
        property CalculatorApp::ViewModel::ApplicationViewModel^ Model
        {
            CalculatorApp::ViewModel::ApplicationViewModel^ get(){
                return m_model;
            }
        }

        void UnregisterEventHandlers();

        void SetDefaultFocus();
        void SetHeaderAutomationName();

        Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ CreateUIElementsForCategories(_In_ Windows::Foundation::Collections::IObservableVector<CalculatorApp::Common::NavCategoryGroup^>^ categories);

    protected:
        void OnNavigatedTo(_In_ Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void WindowSizeChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Core::WindowSizeChangedEventArgs^ e);
        void OnAppPropertyChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e);
        void SetTitleBarControlColors();
        void ColorValuesChanged(_In_ Windows::UI::ViewManagement::UISettings^ sender, _In_ Platform::Object^ e);

        void OnNavLoaded(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnNavPaneOpening(_In_ Microsoft::UI::Xaml::Controls::NavigationView^ sender, _In_ Platform::Object^ args);
        void OnNavPaneOpened(_In_ Microsoft::UI::Xaml::Controls::NavigationView^ sender, _In_ Platform::Object^ args);
        void OnNavPaneClosed(_In_ Microsoft::UI::Xaml::Controls::NavigationView^ sender, _In_ Platform::Object^ args);
        void OnNavSelectionChanged(_In_ Platform::Object^ sender, _In_ Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs^ e);

        void OnAboutButtonClick(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
        void OnAboutFlyoutOpened(_In_ Platform::Object^ sender, _In_ Platform::Object^ e);
        void OnAboutFlyoutClosed(_In_ Platform::Object^ sender, _In_ Platform::Object^ e);

        Microsoft::UI::Xaml::Controls::NavigationViewItemHeader^ CreateNavViewHeaderFromGroup(CalculatorApp::Common::NavCategoryGroup^ group);
        Microsoft::UI::Xaml::Controls::NavigationViewItem^ CreateNavViewItemFromCategory(CalculatorApp::Common::NavCategory^ category);
        
        Windows::Foundation::EventRegistrationToken m_fullscreenFlyoutClosedToken;
        void OnFullscreenFlyoutClosed();
        
        void ShowHideControls(CalculatorApp::Common::ViewMode mode);
        void UpdateViewState();
        void UpdatePanelViewState();

        void OnPageLoaded(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnPageUnLoaded(_In_ Platform::Object^, _In_ Windows::UI::Xaml::RoutedEventArgs^);

        void PinUnpinAppBarButtonOnClicked(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);

        void EnsureCalculator();
        void EnsureConverter();
        void EnsureDateCalculator();
        void ShowAboutPage();

        void AnnounceCategoryName();

        CalculatorApp::Calculator^ m_calculator;
        CalculatorApp::UnitConverter^ m_converter;
        CalculatorApp::DateCalculator^ m_dateCalculator;
        Windows::Foundation::EventRegistrationToken _windowSizeEventToken;
        Windows::Foundation::EventRegistrationToken m_hardwareButtonsBackPressedToken;
        Windows::Foundation::EventRegistrationToken m_colorValuesChangedToken;
        CalculatorApp::ViewModel::ApplicationViewModel^ m_model;
        Windows::UI::ViewManagement::UISettings^ m_uiSettings;

        std::unique_ptr<CalculatorApp::Common::TitleBarHelper> m_titleBarHelper;
    };
}
