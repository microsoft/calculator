// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "HistoryViewModel.h"
#include "Common/TraceLogger.h"
#include "Common/LocalizationStringUtil.h"
#include "Common/LocalizationSettings.h"
#include "StandardCalculatorViewModel.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::Automation;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Cryptography;
using namespace Windows::Foundation::Collections;

static StringReference HistoryVectorLengthKey{ L"HistoryVectorLength" };
static StringReference ItemsSizeKey{ L"ItemsCount" };

namespace HistoryResourceKeys
{
    StringReference HistoryCleared(L"HistoryList_Cleared");
    StringReference HistorySlotCleared(L"Format_HistorySlotCleared");
}

HistoryViewModel::HistoryViewModel(_In_ CalculationManager::CalculatorManager* calculatorManager)
    : m_calculatorManager(calculatorManager)
    , m_localizedHistoryCleared(nullptr)
    , m_localizedHistorySlotCleared(nullptr)
{
    AreHistoryShortcutsEnabled = true;

    Items = ref new Platform::Collections::Vector<HistoryItemViewModel ^>();
}

// this will reload Items with the history list based on current mode
void HistoryViewModel::ReloadHistory(_In_ ViewMode currentMode)
{
    if (currentMode == ViewMode::Standard)
    {
        m_currentMode = CalculationManager::CalculatorMode::Standard;
    }
    else if (currentMode == ViewMode::Scientific)
    {
        m_currentMode = CalculationManager::CalculatorMode::Scientific;
    }
    else
    {
        return;
    }

    auto historyListModel = m_calculatorManager->GetHistoryItems(m_currentMode);
    auto historyListVM = ref new Platform::Collections::Vector<HistoryItemViewModel ^>();
    const auto& localizer = LocalizationSettings::GetInstance();
    if (historyListModel.size() > 0)
    {
        for (auto ritr = historyListModel.rbegin(); ritr != historyListModel.rend(); ++ritr)
        {
            wstring expression = (*ritr)->historyItemVector.expression;
            wstring result = (*ritr)->historyItemVector.result;
            localizer.LocalizeDisplayValue(&expression);
            localizer.LocalizeDisplayValue(&result);

            auto item = ref new HistoryItemViewModel(
                ref new Platform::String(expression.c_str()),
                ref new Platform::String(result.c_str()),
                (*ritr)->historyItemVector.spTokens,
                (*ritr)->historyItemVector.spCommands);
            historyListVM->Append(item);
        }
    }

    Items = historyListVM;
    RaisePropertyChanged(ItemsSizeKey);
}

void HistoryViewModel::OnHistoryItemAdded(_In_ unsigned int addedItemIndex)
{
    auto newItem = m_calculatorManager->GetHistoryItem(addedItemIndex);
    const auto& localizer = LocalizationSettings::GetInstance();
    wstring expression = newItem->historyItemVector.expression;
    wstring result = newItem->historyItemVector.result;
    localizer.LocalizeDisplayValue(&expression);
    localizer.LocalizeDisplayValue(&result);
    auto item = ref new HistoryItemViewModel(
        ref new Platform::String(expression.c_str()),
        ref new Platform::String(result.c_str()),
        newItem->historyItemVector.spTokens,
        newItem->historyItemVector.spCommands);

    // check if we have not hit the max items
    if (Items->Size >= m_calculatorManager->MaxHistorySize())
    {
        // this means the item already exists
        Items->RemoveAt(Items->Size - 1);
    }

    assert(addedItemIndex <= m_calculatorManager->MaxHistorySize() && addedItemIndex >= 0);
    Items->InsertAt(0, item);
    RaisePropertyChanged(ItemsSizeKey);
}

void HistoryViewModel::SetCalculatorDisplay(CalculatorDisplay& calculatorDisplay)
{
    WeakReference historyViewModel(this);
    calculatorDisplay.SetHistoryCallback(historyViewModel);
}

void HistoryViewModel::ShowItem(_In_ HistoryItemViewModel ^ e)
{
    unsigned int index;
    Items->IndexOf(e, &index);
    TraceLogger::GetInstance()->LogHistoryItemLoad((ViewMode)m_currentMode, Items->Size, (int)(index));
    HistoryItemClicked(e);
}

void HistoryViewModel::DeleteItem(_In_ HistoryItemViewModel ^ e)
{
    uint32_t itemIndex;
    if (Items->IndexOf(e, &itemIndex))
    {
        if (m_calculatorManager->RemoveHistoryItem(itemIndex))
        {
            Items->RemoveAt(itemIndex);
            RaisePropertyChanged(ItemsSizeKey);
        }
    }
    // Adding 1 to the history item index to provide 1-based numbering on announcements.
    wstring localizedIndex = to_wstring(itemIndex + 1);
    LocalizationSettings::GetInstance().LocalizeDisplayValue(&localizedIndex);
    m_localizedHistorySlotCleared = AppResourceProvider::GetInstance()->GetResourceString(HistoryResourceKeys::HistorySlotCleared);
    String ^ announcement = LocalizationStringUtil::GetLocalizedString(m_localizedHistorySlotCleared, StringReference(localizedIndex.c_str()));
    HistoryAnnouncement = CalculatorAnnouncement::GetHistorySlotClearedAnnouncement(announcement);
}

void HistoryViewModel::OnHideCommand(_In_ Platform::Object ^ e)
{
    // added at VM layer so that the views do not have to individually raise events
    HideHistoryClicked();
}

void HistoryViewModel::OnClearCommand(_In_ Platform::Object ^ e)
{
    if (AreHistoryShortcutsEnabled)
    {
        m_calculatorManager->ClearHistory();

        if (Items->Size > 0)
        {
            Items->Clear();
            RaisePropertyChanged(ItemsSizeKey);
        }

        if (m_localizedHistoryCleared == nullptr)
        {
            m_localizedHistoryCleared = AppResourceProvider::GetInstance()->GetResourceString(HistoryResourceKeys::HistoryCleared);
        }
        HistoryAnnouncement = CalculatorAnnouncement::GetHistoryClearedAnnouncement(m_localizedHistoryCleared);
    }
}

unsigned long long HistoryViewModel::GetMaxItemSize()
{
    return static_cast<unsigned long long>(m_calculatorManager->MaxHistorySize());
}
