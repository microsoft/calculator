// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "CalcViewModel/Common/LocalizationService.h"
#include "CalcViewModel/Common/Automation/NarratorNotifier.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "Views/Memory.xaml.h"
#include "Converters/BooleanToVisibilityConverter.h"
#include "CalcViewModel/Common/LocalizationStringUtil.h"
#include "Common/AppLifecycleLogger.h"
#include "Common/KeyboardShortcutManager.h"

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

MainPage::MainPage()
    : m_model(ref new ApplicationViewModel())
{
    InitializeComponent();

    KeyboardShortcutManager::Initialize();

    Application::Current->Suspending += ref new SuspendingEventHandler(this, &MainPage::App_Suspending);
    m_model->PropertyChanged += ref new PropertyChangedEventHandler(this, &MainPage::OnAppPropertyChanged);
    m_accessibilitySettings = ref new AccessibilitySettings();

    double sizeInInches = 0.0;

    if (SUCCEEDED(GetIntegratedDisplaySize(&sizeInInches)))
    {
        if (sizeInInches < 7.0) // If device's display size (diagonal length) is less than 7 inches then keep the calc always in Portrait mode only
        {
            DisplayInformation::AutoRotationPreferences = DisplayOrientations::Portrait | DisplayOrientations::PortraitFlipped;
        }
    }
}

void MainPage::OnNavigatedTo(NavigationEventArgs ^ e)
{
    ViewMode initialMode = ViewMode::Standard;
    if (e->Parameter != nullptr)
    {
        String ^ stringParameter = dynamic_cast<String ^>(e->Parameter);
        if (stringParameter != nullptr)
        {
            initialMode = (ViewMode)stoi(stringParameter->Data());
        }
    }
    else
    {
        ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
        if (localSettings->Values->HasKey(ApplicationViewModel::ModePropertyName))
        {
            initialMode = NavCategory::Deserialize(localSettings->Values->Lookup(ApplicationViewModel::ModePropertyName));
        }
    }

    m_model->Initialize(initialMode);
}

void MainPage::WindowSizeChanged(_In_ Platform::Object ^ /*sender*/, _In_ Windows::UI::Core::WindowSizeChangedEventArgs ^ e)
{
    // We don't use layout aware page's view states, we have our own
    UpdateViewState();
}

void MainPage::OnAppPropertyChanged(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::Data::PropertyChangedEventArgs ^ e)
{
    String ^ propertyName = e->PropertyName;
    if (propertyName == ApplicationViewModel::ModePropertyName)
    {
        ViewMode newValue = m_model->Mode;
        ViewMode previousMode = m_model->PreviousMode;

        KeyboardShortcutManager::DisableShortcuts(false);

        if (newValue == ViewMode::Standard)
        {
            EnsureCalculator();
            m_model->CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = true;
            m_calculator->AnimateCalculator(NavCategory::IsConverterViewMode(previousMode));
            m_model->CalculatorViewModel->HistoryVM->ReloadHistory(newValue);
        }
        else if (newValue == ViewMode::Scientific)
        {
            EnsureCalculator();
            m_model->CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = true;
            if (m_model->PreviousMode != ViewMode::Scientific)
            {
                m_calculator->AnimateCalculator(NavCategory::IsConverterViewMode(previousMode));
            }

            m_model->CalculatorViewModel->HistoryVM->ReloadHistory(newValue);
        }
        else if (newValue == ViewMode::Programmer)
        {
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
                m_model->CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = false;
            }
            EnsureDateCalculator();
        }
        else if (newValue == ViewMode::Graphing)
        {
            EnsureGraphingCalculator();
            KeyboardShortcutManager::DisableShortcuts(true);
        }
        else if (NavCategory::IsConverterViewMode(newValue))
        {
            if (m_model->CalculatorViewModel)
            {
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
        SetDefaultFocus();
    }
    else if (propertyName == ApplicationViewModel::CategoryNamePropertyName)
    {
        SetHeaderAutomationName();
        AnnounceCategoryName();
    }
}

void MainPage::ShowHideControls(ViewMode mode)
{
    auto isCalcViewMode = NavCategory::IsCalculatorViewMode(mode);
    auto isDateCalcViewMode = NavCategory::IsDateCalculatorViewMode(mode);
    auto isGraphingCalcViewMode = NavCategory::IsGraphingCalculatorViewMode(mode);
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

    if (m_graphingCalculator)
    {
        m_graphingCalculator->Visibility = BooleanToVisibilityConverter::Convert(isGraphingCalcViewMode);
        m_graphingCalculator->IsEnabled = isGraphingCalcViewMode;
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

void MainPage::OnPageLoaded(_In_ Object ^, _In_ RoutedEventArgs ^ args)
{
    if (!m_converter && !m_calculator && !m_dateCalculator && !m_graphingCalculator)
    {
        // We have just launched into our default mode (standard calc) so ensure calc is loaded
        EnsureCalculator();
        m_model->CalculatorViewModel->IsStandard = true;
    }

    m_windowSizeEventToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &MainPage::WindowSizeChanged);
    m_accessibilitySettingsToken = m_accessibilitySettings->HighContrastChanged +=
        ref new Windows::Foundation::TypedEventHandler<AccessibilitySettings ^, Object ^>(this, &CalculatorApp::MainPage::OnHighContrastChanged);
    UpdateViewState();

    SetHeaderAutomationName();
    SetDefaultFocus();

    // Delay load things later when we get a chance.
    this->Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]() {
            if (TraceLogger::GetInstance()->IsWindowIdInLog(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread())))
            {
                AppLifecycleLogger::GetInstance().LaunchUIResponsive();
                AppLifecycleLogger::GetInstance().LaunchVisibleComplete();
            }

            this->FindName(L"NavView");
        }));
}

void MainPage::OnHighContrastChanged(_In_ AccessibilitySettings ^ /*sender*/, _In_ Object ^ /*args*/)
{
    if (Model->IsAlwaysOnTop && this->ActualHeight < 394)
    {
        // Sets to default always-on-top size to force re-layout
        ApplicationView::GetForCurrentView()->TryResizeView(Size(320, 394));
    }
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
    if (m_graphingCalculator != nullptr && m_graphingCalculator->Visibility == ::Visibility::Visible)
    {
        m_graphingCalculator->SetDefaultFocus();
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
        Binding ^ isStandardBinding = ref new Binding();
        isStandardBinding->Path = ref new PropertyPath(L"IsStandard");
        m_calculator->SetBinding(m_calculator->IsStandardProperty, isStandardBinding);
        Binding ^ isScientificBinding = ref new Binding();
        isScientificBinding->Path = ref new PropertyPath(L"IsScientific");
        m_calculator->SetBinding(m_calculator->IsScientificProperty, isScientificBinding);
        Binding ^ isProgramerBinding = ref new Binding();
        isProgramerBinding->Path = ref new PropertyPath(L"IsProgrammer");
        m_calculator->SetBinding(m_calculator->IsProgrammerProperty, isProgramerBinding);
        Binding ^ isAlwaysOnTopBinding = ref new Binding();
        isAlwaysOnTopBinding->Path = ref new PropertyPath(L"IsAlwaysOnTop");
        m_calculator->SetBinding(m_calculator->IsAlwaysOnTopProperty, isAlwaysOnTopBinding);
        m_calculator->Style = CalculatorBaseStyle;

        CalcHolder->Child = m_calculator;

        // Calculator's "default" state is visible, but if we get delay loaded
        // when in converter, we should not be visible. This is not a problem for converter
        // since its default state is hidden.
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

void MainPage::EnsureGraphingCalculator()
{
    if (!m_graphingCalculator)
    {
        m_graphingCalculator = ref new GraphingCalculator();
        m_graphingCalculator->Name = L"GraphingCalculator";
        m_graphingCalculator->DataContext = m_model->GraphingCalcViewModel;

        GraphingCalcHolder->Child = m_graphingCalculator;
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

void MainPage::OnNavLoaded(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    if (NavView->SelectedItem == nullptr)
    {
        auto menuItems = static_cast<IObservableVector<Object ^> ^>(NavView->MenuItemsSource);
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

void MainPage::OnNavPaneOpening(_In_ MUXC::NavigationView ^ sender, _In_ Object ^ args)
{
    if (!AboutButton)
    {
        this->FindName(L"AboutButton");
    }
}

void MainPage::OnNavPaneOpened(_In_ MUXC::NavigationView ^ sender, _In_ Object ^ args)
{
    KeyboardShortcutManager::HonorShortcuts(false);
    TraceLogger::GetInstance()->LogNavBarOpened();
}

void MainPage::OnNavPaneClosed(_In_ MUXC::NavigationView ^ sender, _In_ Object ^ args)
{
    if (Model->Mode != ViewMode::Graphing)
    {
        KeyboardShortcutManager::HonorShortcuts(true);
    }

    this->SetDefaultFocus();
}

void MainPage::OnAboutButtonClick(Object ^ sender, ItemClickEventArgs ^ e)
{
    ShowAboutPage();
}

void MainPage::OnAboutButtonKeyDown(Object ^ sender, KeyRoutedEventArgs ^ e)
{
    if (e->Key == VirtualKey::Space || e->Key == VirtualKey::Enter)
    {
        ShowAboutPage();
    }
}

void MainPage::OnAboutFlyoutOpened(_In_ Object ^ sender, _In_ Object ^ e)
{
    // Keep Ignoring Escape till the About page flyout is opened
    KeyboardShortcutManager::IgnoreEscape(false);
    KeyboardShortcutManager::HonorShortcuts(false);
}

void MainPage::OnAboutFlyoutClosed(_In_ Object ^ sender, _In_ Object ^ e)
{
    // Start Honoring Escape once the About page flyout is closed
    KeyboardShortcutManager::HonorEscape();
    KeyboardShortcutManager::HonorShortcuts(!NavView->IsPaneOpen);
}

void MainPage::OnNavSelectionChanged(_In_ Object ^ sender, _In_ MUXC::NavigationViewSelectionChangedEventArgs ^ e)
{
    auto item = dynamic_cast<MUXC::NavigationViewItem ^>(e->SelectedItemContainer);
    if (item != nullptr)
    {
        auto selectedItem = static_cast<NavCategory ^>(item->DataContext);
        Model->Mode = selectedItem->Mode;
    }
}

IObservableVector<Object ^> ^ MainPage::CreateUIElementsForCategories(_In_ IObservableVector<NavCategoryGroup ^> ^ categories)
{
    auto menuCategories = ref new Vector<Object ^>();

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

MUXC::NavigationViewItemHeader ^ MainPage::CreateNavViewHeaderFromGroup(NavCategoryGroup ^ group)
{
    auto header = ref new MUXC::NavigationViewItemHeader();
    header->DataContext = group;

    header->Content = group->Name;
    AutomationProperties::SetName(header, group->AutomationName);
    AutomationProperties::SetHeadingLevel(header, Peers::AutomationHeadingLevel::Level1);

    return header;
}

MUXC::NavigationViewItem ^ MainPage::CreateNavViewItemFromCategory(NavCategory ^ category)
{
    auto item = ref new MUXC::NavigationViewItem();
    item->DataContext = category;

    auto icon = ref new FontIcon();
    icon->FontFamily = static_cast<Windows::UI::Xaml::Media::FontFamily ^>(App::Current->Resources->Lookup(L"CalculatorFontFamily"));
    icon->Glyph = category->Glyph;
    item->Icon = icon;

    item->Content = category->Name;
    item->AccessKey = category->AccessKey;
    item->IsEnabled = category->IsEnabled;
    item->Style = static_cast<Windows::UI::Xaml::Style ^>(Resources->Lookup(L"NavViewItemStyle"));

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

void MainPage::UnregisterEventHandlers()
{
    Window::Current->SizeChanged -= m_windowSizeEventToken;
    m_windowSizeEventToken.Value = 0;
    m_accessibilitySettings->HighContrastChanged -= m_accessibilitySettingsToken;
    m_accessibilitySettingsToken.Value = 0;

    if (m_calculator != nullptr)
    {
        m_calculator->UnregisterEventHandlers();
    }
}

void MainPage::SetHeaderAutomationName()
{
    ViewMode mode = m_model->Mode;
    auto resProvider = AppResourceProvider::GetInstance();

    String ^ name;
    if (NavCategory::IsDateCalculatorViewMode(mode))
    {
        name = resProvider->GetResourceString(L"HeaderAutomationName_Date");
    }
    else
    {
        String ^ full;
        if (NavCategory::IsCalculatorViewMode(mode) || NavCategory::IsGraphingCalculatorViewMode(mode))
        {
            full = resProvider->GetResourceString(L"HeaderAutomationName_Calculator");
        }
        else if (NavCategory::IsConverterViewMode(mode))
        {
            full = resProvider->GetResourceString(L"HeaderAutomationName_Converter");
        }
        name = LocalizationStringUtil::GetLocalizedString(full, m_model->CategoryName);
    }

    AutomationProperties::SetName(Header, name);
}

void MainPage::AnnounceCategoryName()
{
    String ^ categoryName = AutomationProperties::GetName(Header);
    NarratorAnnouncement ^ announcement = CalculatorAnnouncement::GetCategoryNameChangedAnnouncement(categoryName);
    NarratorNotifier->Announce(announcement);
}

void MainPage::OnNavItemInvoked(MUXC::NavigationView ^ /*sender*/, _In_ MUXC::NavigationViewItemInvokedEventArgs ^ e)
{
    NavView->IsPaneOpen = false;
}

void MainPage::TitleBarAlwaysOnTopButtonClick(_In_ Object ^ /*sender*/, _In_ RoutedEventArgs ^ /*e*/)
{
    auto bounds = Window::Current->Bounds;
    Model->ToggleAlwaysOnTop(bounds.Width, bounds.Height);
}

void MainPage::AlwaysOnTopButtonClick(_In_ Object ^ /*sender*/, _In_ RoutedEventArgs ^ /*e*/)
{
    Model->ToggleAlwaysOnTop(0, 0);
}

void MainPage::App_Suspending(Object ^ sender, Windows::ApplicationModel::SuspendingEventArgs ^ e)
{
    if (m_model->IsAlwaysOnTop)
    {
        ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
        localSettings->Values->Insert(ApplicationViewModel::WidthLocalSettings, this->ActualWidth);
        localSettings->Values->Insert(ApplicationViewModel::HeightLocalSettings, this->ActualHeight);
    }
}
