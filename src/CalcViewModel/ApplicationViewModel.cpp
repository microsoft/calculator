// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ApplicationViewModel.h"
#include "Common/TraceLogger.h"
#include "Common/AppResourceProvider.h"
#include "StandardCalculatorViewModel.h"
#include "DateCalculatorViewModel.h"
#include "DataLoaders/CurrencyHttpClient.h"
#include "DataLoaders/CurrencyDataLoader.h"
#include "DataLoaders/UnitConverterDataLoader.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::DataLoaders;
using namespace CalculatorApp::ViewModel;
using namespace CalculationManager;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::System;
using namespace Windows::Storage;
using namespace Utils;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Foundation;

namespace
{
    StringReference CategoriesPropertyName(L"Categories");
    StringReference ClearMemoryVisibilityPropertyName(L"ClearMemoryVisibility");
}

ApplicationViewModel::ApplicationViewModel()
    : m_CalculatorViewModel(nullptr)
    , m_DateCalcViewModel(nullptr)
    , m_ConverterViewModel(nullptr)
    , m_PreviousMode(ViewMode::None)
    , m_mode(ViewMode::None)
    , m_categories(nullptr)
{
    SetMenuCategories();
}

void ApplicationViewModel::Mode::set(ViewMode value)
{
    if (m_mode != value)
    {
        PreviousMode = m_mode;
        m_mode = value;
        SetDisplayNormalAlwaysOnTopOption();
        OnModeChanged();
        RaisePropertyChanged(ModePropertyName);
    }
}

void ApplicationViewModel::Categories::set(IObservableVector<NavCategoryGroup ^> ^ value)
{
    if (m_categories != value)
    {
        m_categories = value;
        RaisePropertyChanged(CategoriesPropertyName);
    }
}

void ApplicationViewModel::Initialize(ViewMode mode)
{
    if (!NavCategory::IsValidViewMode(mode))
    {
        mode = ViewMode::Standard;
    }

    try
    {
        Mode = mode;
    }
    catch (const std::exception& e)
    {
        TraceLogger::GetInstance().LogStandardException(__FUNCTIONW__, e);
        if (!TryRecoverFromNavigationModeFailure())
        {
            // Could not navigate to standard mode either.
            // Throw the original exception so we have a good stack to debug.
            throw;
        }
    }
    catch (Exception ^ e)
    {
        TraceLogger::GetInstance().LogPlatformException(__FUNCTIONW__, e);
        if (!TryRecoverFromNavigationModeFailure())
        {
            // Could not navigate to standard mode either.
            // Throw the original exception so we have a good stack to debug.
            throw;
        }
    }
}

bool ApplicationViewModel::TryRecoverFromNavigationModeFailure()
{
    // Here we are simply trying to recover from being unable to navigate to a mode.
    // Try falling back to standard mode and if there are *any* exceptions, we should
    // fail because something is seriously wrong.
    try
    {
        Mode = ViewMode::Standard;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void ApplicationViewModel::OnModeChanged()
{
    assert(NavCategory::IsValidViewMode(m_mode));
    TraceLogger::GetInstance().LogModeChangeBegin(m_PreviousMode, m_mode, ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    if (NavCategory::IsCalculatorViewMode(m_mode))
    {
        TraceLogger::GetInstance().LogCalculatorModeViewed(m_mode, ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
        if (!m_CalculatorViewModel)
        {
            m_CalculatorViewModel = ref new StandardCalculatorViewModel();
        }
        m_CalculatorViewModel->SetCalculatorType(m_mode);
    }
    else if (NavCategory::IsDateCalculatorViewMode(m_mode))
    {
        TraceLogger::GetInstance().LogDateCalculatorModeViewed(m_mode, ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
        if (!m_DateCalcViewModel)
        {
            m_DateCalcViewModel = ref new DateCalculatorViewModel();
        }
    }
    else if (NavCategory::IsConverterViewMode(m_mode))
    {
        TraceLogger::GetInstance().LogConverterModeViewed(m_mode, ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
        if (!m_ConverterViewModel)
        {
            auto dataLoader = make_shared<UnitConverterDataLoader>(ref new GeographicRegion());
            auto currencyDataLoader = make_shared<CurrencyDataLoader>(make_unique<CurrencyHttpClient>());
            m_ConverterViewModel = ref new UnitConverterViewModel(make_shared<UnitConversionManager::UnitConverter>(dataLoader, currencyDataLoader));
        }

        m_ConverterViewModel->Mode = m_mode;
    }

    auto resProvider = AppResourceProvider::GetInstance();
    CategoryName = resProvider.GetResourceString(NavCategory::GetNameResourceKey(m_mode));

    // This is the only place where a ViewMode enum should be cast to an int.
    //
    // Save the changed mode, so that the new window launches in this mode.
    // Don't save until after we have adjusted to the new mode, so we don't save a mode that fails to load.
    ApplicationData::Current->LocalSettings->Values->Insert(ModePropertyName, NavCategory::Serialize(m_mode));

    TraceLogger::GetInstance().LogModeChangeEnd(m_mode, ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    RaisePropertyChanged(ClearMemoryVisibilityPropertyName);
}

void ApplicationViewModel::OnCopyCommand(Object ^ parameter)
{
    if (NavCategory::IsConverterViewMode(m_mode))
    {
        ConverterViewModel->OnCopyCommand(parameter);
    }
    else if (NavCategory::IsDateCalculatorViewMode(m_mode))
    {
        DateCalcViewModel->OnCopyCommand(parameter);
    }
    else
    {
        CalculatorViewModel->OnCopyCommand(parameter);
    }
}

void ApplicationViewModel::OnPasteCommand(Object ^ parameter)
{
    if (NavCategory::IsConverterViewMode(m_mode))
    {
        ConverterViewModel->OnPasteCommand(parameter);
    }
    else if (NavCategory::IsCalculatorViewMode(m_mode))
    {
        CalculatorViewModel->OnPasteCommand(parameter);
    }
}

void ApplicationViewModel::SetMenuCategories()
{
    // Use the Categories property instead of the backing variable
    // because we want to take advantage of binding updates and
    // property setter logic.
    Categories = NavCategoryGroup::CreateMenuOptions();
}

void ApplicationViewModel::AlwaysOnTopButtonClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    HandleAlwaysOnTopButtonClick(sender, e);
}

Concurrency::task<void> ApplicationViewModel::HandleAlwaysOnTopButtonClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (ApplicationView::GetForCurrentView()->GetForCurrentView()->ViewMode == ApplicationViewMode::CompactOverlay)
    {
        CalculatorViewModel->AreHistoryShortcutsEnabled = true;
        CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = true;

        bool success = co_await ApplicationView::GetForCurrentView()->TryEnterViewModeAsync(ApplicationViewMode::Default);
        CalculatorViewModel->IsAlwaysOnTop = !success;
    }
    else
    {
        CalculatorViewModel->AreHistoryShortcutsEnabled = false;
        CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = false;

        Windows::Storage::ApplicationDataContainer ^ localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
        ViewModePreferences ^ compactOptions = ViewModePreferences::CreateDefault(ApplicationViewMode::CompactOverlay);
        if (!localSettings->Values->GetView()->HasKey(LaunchedLocalSettings))
        {
            compactOptions->CustomSize = Windows::Foundation::Size(320, 394);
            localSettings->Values->Insert(LaunchedLocalSettings, true);
        }
        else
        {
            if (localSettings->Values->GetView()->HasKey(WidthLocalSettings) && localSettings->Values->GetView()->HasKey(HeightLocalSettings))
            {
                float width = safe_cast<IPropertyValue ^>(localSettings->Values->GetView()->Lookup(WidthLocalSettings))->GetSingle();
                float height = safe_cast<IPropertyValue ^>(localSettings->Values->GetView()->Lookup(HeightLocalSettings))->GetSingle();
                compactOptions->CustomSize = Windows::Foundation::Size(width, height);
            }
            else
            {
                compactOptions->CustomSize = Windows::Foundation::Size(320, 394);
            }
        }

        bool success = co_await ApplicationView::GetForCurrentView()->TryEnterViewModeAsync(ApplicationViewMode::CompactOverlay, compactOptions);
        CalculatorViewModel->IsAlwaysOnTop = success;
    }
    SetDisplayNormalAlwaysOnTopOption();
}

void ApplicationViewModel::SetDisplayNormalAlwaysOnTopOption()
{
    if (CalculatorViewModel != nullptr)
    {
        DisplayNormalAlwaysOnTopOption = m_mode == ViewMode::Standard && ApplicationView::GetForCurrentView()->IsViewModeSupported(ApplicationViewMode::CompactOverlay) && !CalculatorViewModel->IsAlwaysOnTop;
    }
    else
    {
        DisplayNormalAlwaysOnTopOption = m_mode == ViewMode::Standard && ApplicationView::GetForCurrentView()->IsViewModeSupported(ApplicationViewMode::CompactOverlay);
    }
}
