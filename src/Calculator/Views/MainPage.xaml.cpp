// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "CalcViewModel/Common/LocalizationService.h"
#include "CalcViewModel/Common/Automation/NarratorNotifier.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "Views/Memory.xaml.h"
#include "Converters/BooleanToVisibilityConverter.h"
#include "Common/AppLifecycleLogger.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::Automation;
using namespace CalculatorApp::Converters;
using namespace CalculatorApp::ViewModel;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::System::Threading;
using namespace Utils;

namespace MUXC = Microsoft::UI::Xaml::Controls;

namespace CalculatorApp::VisualStates
{
    namespace Mode
    {
        StringReference Calculator(L"Calculator");
        StringReference DateCalculator(L"Date");
        StringReference UnitConverter(L"Converter");
    }

    namespace Memory
    {
        StringReference MemoryEmpty(L"MemoryEmpty");
        StringReference MemoryRight(L"MemoryRight");
        StringReference MemoryBottom(L"MemoryBottom");
        StringReference MemoryDock(L"MemoryDock");
    }
}

MainPage::MainPage() :
    m_model(ref new ApplicationViewModel())
{
    InitializeComponent();

    m_uiSettings = ref new UISettings();

    KeyboardShortcutManager::Initialize();

    m_model->PropertyChanged += ref new PropertyChangedEventHandler(this, &MainPage::OnAppPropertyChanged);

    double sizeInInches = 0.0;

    if (SUCCEEDED(GetIntegratedDisplaySize(&sizeInInches)))
    {
        if (sizeInInches < 7.0) // If device's display size (diagonal length) is less than 7 inches then keep the calc always in Portrait mode only
        {
            DisplayInformation::AutoRotationPreferences = DisplayOrientations::Portrait | DisplayOrientations::PortraitFlipped;
        }
    }    
}

void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    if (m_model->CalculatorViewModel)
    {
        m_model->CalculatorViewModel->HistoryVM->ClearHistory();
    }

    ViewMode initialMode = ViewMode::Standard;
    if (e->Parameter != nullptr)
    {
        String^ stringParameter = dynamic_cast<String^>(e->Parameter);
        if (stringParameter != nullptr)
        {
            initialMode = (ViewMode)stoi(stringParameter->Data());
        }
    }
    else
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
        if (localSettings->Values->HasKey(ApplicationViewModelProperties::Mode))
        {
            initialMode = NavCategory::Deserialize(localSettings->Values->Lookup(ApplicationViewModelProperties::Mode));
        }
    }

    m_model->Initialize(initialMode);
}

void MainPage::WindowSizeChanged(_In_ Platform::Object^ /*sender*/, _In_ Windows::UI::Core::WindowSizeChangedEventArgs^ e)
{
    // We don't use layout aware page's view states, we have our own
    UpdateViewState();
}

void MainPage::OnAppPropertyChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e)
{
    if (e->PropertyName->Equals(ApplicationViewModelProperties::Mode))
    {
        ViewMode newValue = m_model->Mode;
        ViewMode previousMode = m_model->PreviousMode;

        if (newValue == ViewMode::Standard)
        {
            EnsureCalculator();
            m_model->CalculatorViewModel->AreHistoryShortcutsEnabled = true;
            m_model->CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = true;
            m_calculator->AnimateCalculator(NavCategory::IsConverterViewMode(previousMode));
            m_model->CalculatorViewModel->HistoryVM->ReloadHistory(newValue);
        }
        else if (newValue == ViewMode::Scientific)
        {
            EnsureCalculator();
            m_model->CalculatorViewModel->AreHistoryShortcutsEnabled = true;
            m_model->CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = true;
            if (m_model->PreviousMode != ViewMode::Scientific)
            {
                m_calculator->AnimateCalculator(NavCategory::IsConverterViewMode(previousMode));
            }

            m_model->CalculatorViewModel->HistoryVM->ReloadHistory(newValue);
        }
        else if (newValue == ViewMode::Programmer)
        {
            m_model->CalculatorViewModel->AreHistoryShortcutsEnabled = false;
            m_model->CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = false;
            EnsureCalculator();
            if (m_model->PreviousMode != ViewMode::Programmer)
            {
                m_calculator->AnimateCalculator(NavCategory::IsConverterViewMode(previousMode));
            }
        }
        else if (NavCategory::IsDateCalculatorViewMode(newValue))
        {
            if (m_model->CalculatorViewModel)
            {
                m_model->CalculatorViewModel->AreHistoryShortcutsEnabled = false;
                m_model->CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = false;
            }
            EnsureDateCalculator();
        }
        else if (NavCategory::IsConverterViewMode(newValue))
        {
            if (m_model->CalculatorViewModel)
            {
                m_model->CalculatorViewModel->AreHistoryShortcutsEnabled = false;
                m_model->CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = false;
            }
            EnsureConverter();
            if (!NavCategory::IsConverterViewMode(previousMode))
            {
                m_converter->AnimateConverter();
            }
        }

        ShowHideControls(newValue);

        UpdateViewState();
        SetTitleBarControlColors();
        SetDefaultFocus();
    }
    else if (e->PropertyName->Equals(ApplicationViewModelProperties::CategoryName))
    {
        SetHeaderAutomationName();
        AnnounceCategoryName();
    }
}

void MainPage::ShowHideControls(ViewMode mode)
{
    auto isCalcViewMode = NavCategory::IsCalculatorViewMode(mode);
    auto isDateCalcViewMode = NavCategory::IsDateCalculatorViewMode(mode);
    auto isConverterViewMode = NavCategory::IsConverterViewMode(mode);
    
    if (m_calculator)
    {
        m_calculator->Visibility = BooleanToVisibilityConverter::Convert(isCalcViewMode);
        m_calculator->IsEnabled = isCalcViewMode;
    }

    if (m_dateCalculator)
    {
        m_dateCalculator->Visibility = BooleanToVisibilityConverter::Convert(isDateCalcViewMode);
        m_dateCalculator->IsEnabled = isDateCalcViewMode;
    }

    if (m_converter)
    {
        m_converter->Visibility = BooleanToVisibilityConverter::Convert(isConverterViewMode);
        m_converter->IsEnabled = isConverterViewMode;
    }
}

void MainPage::UpdateViewState()
{
    // All layout related view states are now handled only inside individual controls (standard, scientific, programmer, date, converter)
    if (NavCategory::IsConverterViewMode(m_model->Mode))
    {
        int modeIndex = NavCategory::GetIndexInGroup(m_model->Mode, CategoryGroupType::Converter);
        m_model->ConverterViewModel->CurrentCategory = m_model->ConverterViewModel->Categories->GetAt(modeIndex);
    }
}

void MainPage::UpdatePanelViewState()
{
    if (m_calculator != nullptr)
    {
        m_calculator->UpdatePanelViewState();
    }
}

void MainPage::OnPageLoaded(_In_ Object^, _In_ RoutedEventArgs^ args)
{
    if (!m_converter && !m_calculator && !m_dateCalculator)
    {
        // We have just launched into our default mode (standard calc) so ensure calc is loaded
        EnsureCalculator();
        m_model->CalculatorViewModel->IsStandard = true;
    }

    _windowSizeEventToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &MainPage::WindowSizeChanged);
    UpdateViewState();

    // Set custom XAML Title Bar window caption control button brushes
    m_uiSettings->ColorValuesChanged -= m_colorValuesChangedToken;
    m_colorValuesChangedToken = m_uiSettings->ColorValuesChanged += ref new TypedEventHandler<UISettings^, Object^>(this, &MainPage::ColorValuesChanged);
    SetTitleBarControlColors();

    SetHeaderAutomationName();
    SetDefaultFocus();

    // Delay load things later when we get a chance.
    this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([]()
    {
        if (TraceLogger::GetInstance().UpdateWindowIdLog(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread())))
        {
            TraceLogger::GetInstance().LogAppLaunchComplete();
            AppLifecycleLogger::GetInstance().LaunchUIResponsive();
            AppLifecycleLogger::GetInstance().LaunchVisibleComplete();
        }
    }));
}

void MainPage::OnPageUnLoaded(_In_ Object^, _In_ RoutedEventArgs^)
{
    // OnPageUnloaded Event Handler does not get fired when the calc window is closed.
    // On closing the instance of a window, On Window Consolidate gets fired.
}

void MainPage::PinUnpinAppBarButtonOnClicked(
    _In_ Object^ sender,
    _In_ RoutedEventArgs^ e)
{
     m_model->CalculatorViewModel->OnPinUnpinCommand(sender);
}

void MainPage::SetDefaultFocus()
{
    if (m_calculator != nullptr && m_calculator->Visibility == ::Visibility::Visible)
    {
        m_calculator->SetDefaultFocus();
    }
    if (m_dateCalculator != nullptr && m_dateCalculator->Visibility == ::Visibility::Visible)
    {
        m_dateCalculator->SetDefaultFocus();
    }
    if (m_converter != nullptr && m_converter->Visibility == ::Visibility::Visible)
    {
        m_converter->SetDefaultFocus();
    }
}

void MainPage::EnsureCalculator()
{
    if (!m_calculator)
    {
        // delay load calculator.
        m_calculator = ref new Calculator();
        m_calculator->Name = L"Calculator";
        m_calculator->DataContext = m_model->CalculatorViewModel;
        Binding^ isStandardBinding = ref new Binding();
        isStandardBinding->Path = ref new PropertyPath(L"IsStandard");
        m_calculator->SetBinding(m_calculator->IsStandardProperty, isStandardBinding);
        Binding^ isScientificBinding = ref new Binding();
        isScientificBinding->Path = ref new PropertyPath(L"IsScientific");
        m_calculator->SetBinding(m_calculator->IsScientificProperty, isScientificBinding);
        Binding^ isProgramerBinding = ref new Binding();
        isProgramerBinding->Path = ref new PropertyPath(L"IsProgrammer");
        m_calculator->SetBinding(m_calculator->IsProgrammerProperty, isProgramerBinding);
        m_calculator->Style = CalculatorBaseStyle;

        m_fullscreenFlyoutClosedToken =
            m_calculator->FullscreenFlyoutClosed += ref new FullscreenFlyoutClosedEventHandler(this, &MainPage::OnFullscreenFlyoutClosed);

        CalcHolder->Child = m_calculator;

        // Calculator's "default" state is visible, but if we get delay loaded
        // when in converter, we should not be visible. This is not a problem for converter
        // since it's default state is hidden.
        ShowHideControls(this->Model->Mode);
    }

    if (m_dateCalculator != nullptr)
    {
        m_dateCalculator->CloseCalendarFlyout();
    }

}

void MainPage::EnsureDateCalculator()
{
    if (!m_dateCalculator)
    {
        // delay loading converter
        m_dateCalculator = ref new DateCalculator();
        m_dateCalculator->Name = L"dateCalculator";
        m_dateCalculator->DataContext = m_model->DateCalcViewModel;

        DateCalcHolder->Child = m_dateCalculator;
    }

    if (m_calculator != nullptr)
    {
        m_calculator->CloseHistoryFlyout();
        m_calculator->CloseMemoryFlyout();
    }
}

void MainPage::EnsureConverter()
{
    if (!m_converter)
    {
        // delay loading converter
        m_converter = ref new CalculatorApp::UnitConverter();
        m_converter->Name = L"unitConverter";
        m_converter->DataContext = m_model->ConverterViewModel;
        m_converter->Style = UnitConverterBaseStyle;
        ConverterHolder->Child = m_converter;
    }
}

void MainPage::OnNavLoaded(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    if (NavView->SelectedItem == nullptr)
    {
        auto menuItems = static_cast<IObservableVector<Object^>^>(NavView->MenuItemsSource);
        auto itemCount = static_cast<int>(menuItems->Size);
        auto flatIndex = NavCategory::GetFlatIndex(Model->Mode);

        if (flatIndex >= 0 && flatIndex < itemCount)
        {
            NavView->SelectedItem = menuItems->GetAt(flatIndex);
        }
    }

    auto acceleratorList = NavCategory::GetCategoryAcceleratorKeys();
    for (auto accelerator : acceleratorList)
    {
        NavView->SetValue(Common::KeyboardShortcutManager::VirtualKeyAltChordProperty, accelerator);
    }
    // Special case logic for Ctrl+E accelerator for Date Calculation Mode
    NavView->SetValue(Common::KeyboardShortcutManager::VirtualKeyControlChordProperty, Common::MyVirtualKey::E);
}

void MainPage::OnNavPaneOpening(_In_ MUXC::NavigationView^ sender, _In_ Object^ args)
{
    if (!NavFooter)
    {
        this->FindName(L"NavFooter");
    }
}

void MainPage::OnNavPaneOpened(_In_ MUXC::NavigationView^ sender, _In_ Object^ args)
{
    KeyboardShortcutManager::HonorShortcuts(false);
    TraceLogger::GetInstance().LogNavBarOpened();
}

void MainPage::OnNavPaneClosed(_In_ MUXC::NavigationView^ sender, _In_ Object^ args)
{
    KeyboardShortcutManager::HonorShortcuts(true);
    this->SetDefaultFocus();
}

void MainPage::OnAboutButtonClick(Object^ sender, ItemClickEventArgs^ e)
{
    ShowAboutPage();
}

void MainPage::OnAboutFlyoutOpened(_In_ Object^ sender, _In_ Object^ e)
{
    // Keep Ignoring Escape till the About page flyout is opened
    KeyboardShortcutManager::IgnoreEscape(false);

    KeyboardShortcutManager::UpdateDropDownState(this->AboutPageFlyout);
}

void MainPage::OnAboutFlyoutClosed(_In_ Object^ sender, _In_ Object^ e)
{
    // Start Honoring Escape once the About page flyout is closed
    KeyboardShortcutManager::HonorEscape();

    KeyboardShortcutManager::UpdateDropDownState(nullptr);
}

void MainPage::OnNavSelectionChanged(_In_ Object^ sender, _In_ MUXC::NavigationViewSelectionChangedEventArgs^ e)
{
    auto item = dynamic_cast<MUXC::NavigationViewItem^>(e->SelectedItemContainer);
    if (item != nullptr)
    {
        auto selectedItem = static_cast<NavCategory^>(item->DataContext);
        Model->Mode = selectedItem->Mode;
    }
}

IObservableVector<Object^>^ MainPage::CreateUIElementsForCategories(_In_ IObservableVector<NavCategoryGroup^>^ categories)
{
    auto menuCategories = ref new Vector<Object^>();

    for (auto group : categories)
    {
        menuCategories->Append(CreateNavViewHeaderFromGroup(group));

        for (auto category : group->Categories)
        {
            menuCategories->Append(CreateNavViewItemFromCategory(category));
        }
    }

    return menuCategories;
}

MUXC::NavigationViewItemHeader^ MainPage::CreateNavViewHeaderFromGroup(NavCategoryGroup^ group)
{
    auto header = ref new MUXC::NavigationViewItemHeader();
    header->DataContext = group;

    header->Content = group->Name;
    AutomationProperties::SetName(header, group->AutomationName);
    AutomationProperties::SetHeadingLevel(header, Peers::AutomationHeadingLevel::Level1);

    return header;
}

MUXC::NavigationViewItem^ MainPage::CreateNavViewItemFromCategory(NavCategory^ category)
{
    auto item = ref new MUXC::NavigationViewItem();
    item->DataContext = category;

    auto icon = ref new FontIcon();
    icon->FontFamily = static_cast<Windows::UI::Xaml::Media::FontFamily^>(App::Current->Resources->Lookup(L"CalculatorFontFamily"));
    icon->Glyph = category->Glyph;
    item->Icon = icon;

    item->Content = category->Name;
    item->AccessKey = category->AccessKey;
    item->Style = static_cast<Windows::UI::Xaml::Style^>(Resources->Lookup(L"NavViewItemStyle"));

    AutomationProperties::SetName(item, category->AutomationName);
    AutomationProperties::SetAutomationId(item, category->AutomationId);

    return item;
}

void MainPage::ShowAboutPage()
{
    if (!AboutPage)
    {
        this->FindName(L"AboutPage");
    }

    FlyoutBase::ShowAttachedFlyout(AboutButton);
}

void MainPage::ColorValuesChanged(_In_ UISettings^ sender, _In_ Object^ e)
{
    WeakReference weakThis(this);
    RunOnUIThreadNonblocking([weakThis]()
    { 
        auto refThis = weakThis.Resolve<MainPage>();
        if (refThis != nullptr)
        {
            refThis->SetTitleBarControlColors();
        }
    }, this->Dispatcher);
}

void MainPage::SetTitleBarControlColors()
{
    auto applicationView = ApplicationView::GetForCurrentView();
    if (applicationView == nullptr) { return; }

    auto applicationTitleBar = applicationView->TitleBar;
    if (applicationTitleBar == nullptr) { return; }

    auto bgbrush = safe_cast<SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlBackgroundTransparentBrush"));
    auto fgbrush = safe_cast<SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlPageTextBaseHighBrush"));
    auto inactivefgbrush = safe_cast<SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlForegroundChromeDisabledLowBrush"));
    auto hoverbgbrush = safe_cast<SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlBackgroundListLowBrush"));
    auto hoverfgbrush = safe_cast<SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlForegroundBaseHighBrush"));
    auto pressedbgbrush = safe_cast<SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlBackgroundListMediumBrush"));
    auto pressedfgbrush = safe_cast<SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlForegroundBaseHighBrush"));

    applicationTitleBar->ButtonBackgroundColor = bgbrush->Color;
    applicationTitleBar->ButtonForegroundColor = fgbrush->Color;
    applicationTitleBar->ButtonInactiveBackgroundColor = bgbrush->Color;
    applicationTitleBar->ButtonInactiveForegroundColor = inactivefgbrush->Color;
    applicationTitleBar->ButtonHoverBackgroundColor = hoverbgbrush->Color;
    applicationTitleBar->ButtonHoverForegroundColor = hoverfgbrush->Color;
    applicationTitleBar->ButtonPressedBackgroundColor = pressedbgbrush->Color;
    applicationTitleBar->ButtonPressedForegroundColor = pressedfgbrush->Color;
}

void MainPage::UnregisterEventHandlers()
{
    m_uiSettings->ColorValuesChanged -= m_colorValuesChangedToken;
    m_colorValuesChangedToken.Value = 0;

    Window::Current->SizeChanged -= _windowSizeEventToken;
    _windowSizeEventToken.Value = 0;

    if (m_calculator != nullptr)
    {
        m_calculator->FullscreenFlyoutClosed -= m_fullscreenFlyoutClosedToken;
        m_fullscreenFlyoutClosedToken.Value = 0;

        m_calculator->UnregisterEventHandlers();
    }

    m_titleBarHelper = nullptr;
}

void MainPage::SetHeaderAutomationName()
{
    ViewMode mode = m_model->Mode;
    auto resProvider = AppResourceProvider::GetInstance();

    String^ name;
    if (NavCategory::IsDateCalculatorViewMode(mode))
    {
        name = resProvider.GetResourceString(L"HeaderAutomationName_Date");
    }
    else
    {
        wstring full;
        if (NavCategory::IsCalculatorViewMode(mode))
        {
            full = resProvider.GetResourceString(L"HeaderAutomationName_Calculator")->Data();
        }
        else if (NavCategory::IsConverterViewMode(mode))
        {
            full = resProvider.GetResourceString(L"HeaderAutomationName_Converter")->Data();
        }
        
        string::size_type found = full.find(L"%1");
        wstring strMode = m_model->CategoryName->Data();
        full = full.replace(found, 2, strMode);

        name = ref new String(full.c_str());
    }

    AutomationProperties::SetName(Header, name);
}

void MainPage::OnFullscreenFlyoutClosed()
{
    this->CustomTitleBar->SetTitleBar();
}

void MainPage::AnnounceCategoryName()
{
    String^ categoryName = AutomationProperties::GetName(Header);
    NarratorAnnouncement^ announcement = CalculatorAnnouncement::GetCategoryNameChangedAnnouncement(categoryName);
    NarratorNotifier->Announce(announcement);
}
