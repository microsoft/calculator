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
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculatorApp::ViewModel::DataLoaders;
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
using namespace Concurrency;

namespace
{
    StringReference CategoriesPropertyName(L"Categories");
    StringReference ClearMemoryVisibilityPropertyName(L"ClearMemoryVisibility");
}

ApplicationViewModel::ApplicationViewModel()
    : m_CalculatorViewModel(nullptr)
    , m_DateCalcViewModel(nullptr)
    , m_GraphingCalcViewModel(nullptr)
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
    if (!NavCategoryStates::IsValidViewMode(mode) || !NavCategoryStates::IsViewModeEnabled(mode))
    {
        mode = ViewMode::Standard;
    }

    try
    {
        Mode = mode;
    }
    catch (const std::exception& e)
    {
        TraceLogger::GetInstance()->LogStandardException(mode, __FUNCTIONW__, e);
        if (!TryRecoverFromNavigationModeFailure())
        {
            // Could not navigate to standard mode either.
            // Throw the original exception so we have a good stack to debug.
            throw;
        }
    }
    catch (Exception ^ e)
    {
        TraceLogger::GetInstance()->LogPlatformException(mode, __FUNCTIONW__, e);
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
    assert(NavCategoryStates::IsValidViewMode(m_mode));
    if (NavCategory::IsCalculatorViewMode(m_mode))
    {
        if (!m_CalculatorViewModel)
        {
            m_CalculatorViewModel = ref new StandardCalculatorViewModel();
        }
        m_CalculatorViewModel->SetCalculatorType(m_mode);
    }
    else if (NavCategory::IsGraphingCalculatorViewMode(m_mode))
    {
        if (!m_GraphingCalcViewModel)
        {
            m_GraphingCalcViewModel = ref new GraphingCalculatorViewModel();
        }
    }
    else if (NavCategory::IsDateCalculatorViewMode(m_mode))
    {
        if (!m_DateCalcViewModel)
        {
            m_DateCalcViewModel = ref new DateCalculatorViewModel();
        }
    }
    else if (NavCategory::IsConverterViewMode(m_mode))
    {
        if (!m_ConverterViewModel)
        {
            auto dataLoader = make_shared<UnitConverterDataLoader>(ref new GeographicRegion());
            auto currencyDataLoader = make_shared<CurrencyDataLoader>(make_unique<CurrencyHttpClient>());
            m_ConverterViewModel = ref new UnitConverterViewModel(make_shared<UnitConversionManager::UnitConverter>(dataLoader, currencyDataLoader));
        }

        m_ConverterViewModel->Mode = m_mode;
    }

    auto resProvider = AppResourceProvider::GetInstance();
    CategoryName = resProvider->GetResourceString(NavCategoryStates::GetNameResourceKey(m_mode));

    // Cast mode to an int in order to save it to app data.
    // Save the changed mode, so that the new window launches in this mode.
    // Don't save until after we have adjusted to the new mode, so we don't save a mode that fails to load.
    ApplicationData::Current->LocalSettings->Values->Insert(ModePropertyName, NavCategoryStates::Serialize(m_mode));

    // Log ModeChange event when not first launch, log WindowCreated on first launch
    if (NavCategoryStates::IsValidViewMode(m_PreviousMode))
    {
        TraceLogger::GetInstance()->LogModeChange(m_mode);
    }
    else
    {
        TraceLogger::GetInstance()->LogWindowCreated(m_mode, ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    }

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
    else if (NavCategory::IsCalculatorViewMode(m_mode))
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
    Categories = NavCategoryStates::CreateMenuOptions();
}

void ApplicationViewModel::ToggleAlwaysOnTop(float width, float height)
{
    HandleToggleAlwaysOnTop(width, height);
}

#pragma optimize("", off)
task<void> ApplicationViewModel::HandleToggleAlwaysOnTop(float width, float height)
{
    if (ApplicationView::GetForCurrentView()->ViewMode == ApplicationViewMode::CompactOverlay)
    {
        ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
        localSettings->Values->Insert(WidthLocalSettings, width);
        localSettings->Values->Insert(HeightLocalSettings, height);

        bool success = co_await ApplicationView::GetForCurrentView()->TryEnterViewModeAsync(ApplicationViewMode::Default);
        CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = success;
        CalculatorViewModel->IsAlwaysOnTop = !success;
        IsAlwaysOnTop = !success;
    }
    else
    {
        ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
        ViewModePreferences ^ compactOptions = ViewModePreferences::CreateDefault(ApplicationViewMode::CompactOverlay);
        if (!localSettings->Values->GetView()->HasKey(LaunchedLocalSettings))
        {
            compactOptions->CustomSize = Size(320, 394);
            localSettings->Values->Insert(LaunchedLocalSettings, true);
        }
        else
        {
            if (localSettings->Values->GetView()->HasKey(WidthLocalSettings) && localSettings->Values->GetView()->HasKey(HeightLocalSettings))
            {
                float oldWidth = safe_cast<IPropertyValue ^>(localSettings->Values->GetView()->Lookup(WidthLocalSettings))->GetSingle();
                float oldHeight = safe_cast<IPropertyValue ^>(localSettings->Values->GetView()->Lookup(HeightLocalSettings))->GetSingle();
                compactOptions->CustomSize = Size(oldWidth, oldHeight);
            }
            else
            {
                compactOptions->CustomSize = Size(320, 394);
            }
        }

        bool success = co_await ApplicationView::GetForCurrentView()->TryEnterViewModeAsync(ApplicationViewMode::CompactOverlay, compactOptions);
        CalculatorViewModel->HistoryVM->AreHistoryShortcutsEnabled = !success;
        CalculatorViewModel->IsAlwaysOnTop = success;
        IsAlwaysOnTop = success;
    }
    SetDisplayNormalAlwaysOnTopOption();
};
#pragma optimize("", on)

void ApplicationViewModel::SetDisplayNormalAlwaysOnTopOption()
{
    DisplayNormalAlwaysOnTopOption =
        m_mode == ViewMode::Standard && ApplicationView::GetForCurrentView()->IsViewModeSupported(ApplicationViewMode::CompactOverlay) && !IsAlwaysOnTop;
}
