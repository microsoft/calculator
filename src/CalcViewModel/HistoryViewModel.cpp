// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "HistoryViewModel.h"
#include "Common/TraceLogger.h"
#include "Common/LocalizationStringUtil.h"
#include "Common/LocalizationSettings.h"

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

namespace CalculatorApp::ViewModel::HistoryResourceKeys
{
    StringReference HistoryCleared(L"HistoryList_Cleared");
}

HistoryViewModel::HistoryViewModel(_In_ CalculationManager::CalculatorManager* calculatorManager) :
    m_calculatorManager(calculatorManager),
    m_localizedHistoryCleared(nullptr)
{
    AreHistoryShortcutsEnabled = true;

    Items = ref new Platform::Collections::Vector<HistoryItemViewModel^>();
    ItemSize = 0;
}

void HistoryViewModel::RestoreCompleteHistory()
{
    RestoreHistory(CalculationManager::CALCULATOR_MODE::CM_STD);
    RestoreHistory(CalculationManager::CALCULATOR_MODE::CM_SCI);
}

// this will reload Items with the history list based on current mode
void HistoryViewModel::ReloadHistory(_In_ ViewMode currentMode)
{
    if (currentMode == ViewMode::Standard)
    {
        m_currentMode = CalculationManager::CALCULATOR_MODE::CM_STD;
    }
    else if (currentMode == ViewMode::Scientific)
    {
        m_currentMode = CalculationManager::CALCULATOR_MODE::CM_SCI;
    }
    else
    {
        return;
    }

    auto historyListModel = m_calculatorManager->GetHistoryItems(m_currentMode);
    auto historyListVM = ref new Platform::Collections::Vector<HistoryItemViewModel^>();
    const auto& localizer = LocalizationSettings::GetInstance();
    if (historyListModel.size() > 0)
    {
        for (auto ritr = historyListModel.rbegin(); ritr != historyListModel.rend(); ++ritr)
        { 
            wstring expression = (*ritr)->historyItemVector.expression;
            wstring result = (*ritr)->historyItemVector.result;
            localizer.LocalizeDisplayValue(&expression);
            localizer.LocalizeDisplayValue(&result);

            auto item = ref new HistoryItemViewModel(ref new Platform::String( expression.c_str()),
                ref new Platform::String(result.c_str()),
                (*ritr)->historyItemVector.spTokens, (*ritr)->historyItemVector.spCommands);
            historyListVM->Append(item);
        }
    }

    Items = historyListVM;
    UpdateItemSize();
}

void HistoryViewModel::OnHistoryItemAdded(_In_ unsigned int addedItemIndex)
{
    auto newItem = m_calculatorManager->GetHistoryItem(addedItemIndex);
    const auto& localizer = LocalizationSettings::GetInstance();
    wstring expression = newItem->historyItemVector.expression;
    wstring result = newItem->historyItemVector.result;
    localizer.LocalizeDisplayValue(&expression);
    localizer.LocalizeDisplayValue(&result);
    auto item = ref new HistoryItemViewModel(ref new Platform::String(expression.c_str()),
    ref new Platform::String(result.c_str()),
    newItem->historyItemVector.spTokens, newItem->historyItemVector.spCommands );

    // check if we have not hit the max items
    if (Items->Size >= m_calculatorManager->MaxHistorySize())
    {
        // this means the item already exists
        Items->RemoveAt(Items->Size -1);
    }

    assert(addedItemIndex <= m_calculatorManager->MaxHistorySize() && addedItemIndex >= 0);
    Items->InsertAt(0, item);
    UpdateItemSize();
    SaveHistory();
}

void HistoryViewModel::SetCalculatorDisplay(CalculatorDisplay &calculatorDisplay)
{
    WeakReference historyViewModel(this);
    calculatorDisplay.SetHistoryCallback(historyViewModel);
}

void HistoryViewModel::ShowItem(_In_ HistoryItemViewModel^ e)
{
    HistoryItemClicked(e);
}

void HistoryViewModel::DeleteItem(_In_ HistoryItemViewModel^ e)
{
    uint32_t itemIndex;
    if (Items->IndexOf(e, &itemIndex))
    {
        if (m_calculatorManager->RemoveHistoryItem(itemIndex))
        {
            // Keys for the history container are index based.
            // SaveHistory() re-inserts the items anyway, so it's faster to just clear out the container.
            CalculationManager::CALCULATOR_MODE currentMode = m_currentMode;
            ApplicationDataContainer^ historyContainer = GetHistoryContainer(currentMode);
            historyContainer->Values->Clear();

            Items->RemoveAt(itemIndex);
            UpdateItemSize();
            SaveHistory();
        }
    }
}

void HistoryViewModel::OnHideCommand(_In_ Platform::Object^ e)
{
    // added at VM layer so that the views do not have to individually raise events
    HideHistoryClicked();
}

void HistoryViewModel::OnClearCommand(_In_ Platform::Object^ e)
{
    TraceLogger::GetInstance().LogClearHistory();
    if (AreHistoryShortcutsEnabled == true)
    {
        m_calculatorManager->ClearHistory();

        if (Items->Size > 0)
        {
            CalculationManager::CALCULATOR_MODE currentMode = m_currentMode;
            ClearHistoryContainer(currentMode);
            Items->Clear();
            UpdateItemSize();
        }

        MakeHistoryClearedNarratorAnnouncement(HistoryResourceKeys::HistoryCleared, m_localizedHistoryCleared);
    }
}

// this method restores history vector per mode
void HistoryViewModel::RestoreHistory(_In_ CalculationManager::CALCULATOR_MODE cMode)
{
    ApplicationDataContainer^ historyContainer = GetHistoryContainer(cMode);
    std::shared_ptr<std::vector<std::shared_ptr<CalculationManager::HISTORYITEM>>> historyVector = std::make_shared<std::vector<std::shared_ptr<CalculationManager::HISTORYITEM>>>();
    auto historyVectorLength = static_cast<int>(historyContainer->Values->Lookup(HistoryVectorLengthKey));
    bool failure = false;

    if (historyVectorLength > 0)
    {
        for (int i = 0; i < historyVectorLength; ++i)
        {
            try
            {
                // deserialize each item
                auto item = DeserializeHistoryItem(i.ToString(), historyContainer);
                std::shared_ptr<CalculationManager::HISTORYITEM> Item = std::make_shared<CalculationManager::HISTORYITEM>(item);
                historyVector->push_back(Item);
            }
            catch (Platform::Exception^ e)
            {
                failure = true;
                break;
            }
        }

        if (!failure)
        {
            // if task has been cancelled set history to 0
            m_calculatorManager->SetHistory(cMode, *historyVector);

            // update length once again for consistency between stored number of items and length
            UpdateHistoryVectorLength(static_cast<int>(historyVector->size()), cMode);
        }
        else
        {
            // in case of failure do not show any item
            UpdateHistoryVectorLength(0, cMode);
        }
    }
}

Platform::String^ HistoryViewModel::GetHistoryContainerKey(_In_ CalculationManager::CALCULATOR_MODE cMode)
{
    Platform::ValueType^ modeValue = static_cast<int>(cMode);
    return Platform::String::Concat(modeValue->ToString(), L"_History");
}

ApplicationDataContainer^ HistoryViewModel::GetHistoryContainer(_In_ CalculationManager::CALCULATOR_MODE cMode)
{
    ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
    ApplicationDataContainer^ historyContainer;

    // naming container based on mode
    Platform::String^ historyContainerKey = GetHistoryContainerKey(cMode);

    if (localSettings->Containers->HasKey(historyContainerKey))
    {
        historyContainer = localSettings->Containers->Lookup(historyContainerKey);
    }
    else
    {
        // create container for adding data
        historyContainer = localSettings->CreateContainer(historyContainerKey, ApplicationDataCreateDisposition::Always);
        int initialHistoryVectorLength = 0;
        historyContainer->Values->Insert(HistoryVectorLengthKey, initialHistoryVectorLength);
    }

    return historyContainer;
}

void HistoryViewModel::ClearHistoryContainer(_In_ CalculationManager::CALCULATOR_MODE cMode)
{
    ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
    localSettings->DeleteContainer(GetHistoryContainerKey(cMode));
}

// this method will be used to update the history item length
void HistoryViewModel::UpdateHistoryVectorLength(_In_ int newValue, _In_ CalculationManager::CALCULATOR_MODE cMode)
{
    ApplicationDataContainer^ historyContainer = GetHistoryContainer(cMode);
    historyContainer->Values->Remove(HistoryVectorLengthKey);
    historyContainer->Values->Insert(HistoryVectorLengthKey, newValue);
}

void HistoryViewModel::ClearHistory()
{
    ClearHistoryContainer(CalculationManager::CALCULATOR_MODE::CM_STD);
    ClearHistoryContainer(CalculationManager::CALCULATOR_MODE::CM_SCI);
}

void HistoryViewModel::SaveHistory()
{
    ApplicationDataContainer^ historyContainer = GetHistoryContainer(m_currentMode);
    auto currentHistoryVector = m_calculatorManager->GetHistoryItems(m_currentMode);
    bool failure = false;
    int index = 0;
    Platform::String^ serializedHistoryItem;

    for (auto iter = currentHistoryVector.begin(); iter != currentHistoryVector.end(); ++iter)
    {
        try
        {
            serializedHistoryItem = SerializeHistoryItem(*iter);
            historyContainer->Values->Insert(index.ToString(), serializedHistoryItem);
        }
        catch (Platform::Exception^)
        {
            failure = true;
            break;
        }

        ++index;
    }

    if (!failure)
    {
        // insertion is successful
        UpdateHistoryVectorLength(static_cast<int>(currentHistoryVector.size()), m_currentMode);
    }
    else
    {
        UpdateHistoryVectorLength(0, m_currentMode);
    }
}

// this serializes a history item into a base64 encoded string
Platform::String^ HistoryViewModel::SerializeHistoryItem(_In_ std::shared_ptr<CalculationManager::HISTORYITEM> const &item)
{
    DataWriter^ writer = ref new DataWriter();
    auto expr = item->historyItemVector.expression;
    auto result = item->historyItemVector.result;
    auto platformExpr = ref new Platform::String(expr.c_str());
    writer->WriteUInt32(writer->MeasureString(platformExpr));
    writer->WriteString(platformExpr);
    auto platformResult = ref new Platform::String(result.c_str());
    writer->WriteUInt32(writer->MeasureString(platformResult));
    writer->WriteString(platformResult);

    Utils::SerializeCommandsAndTokens(item->historyItemVector.spTokens, item->historyItemVector.spCommands, writer);

    IBuffer^ buffer = writer->DetachBuffer();
    if (buffer == nullptr)
    {
        throw ref new Platform::Exception(E_POINTER, ref new Platform::String(L"History Item is NULL"));
    }

    return CryptographicBuffer::EncodeToBase64String(buffer);
}

CalculationManager::HISTORYITEM HistoryViewModel::DeserializeHistoryItem(_In_ Platform::String^ historyItemKey, _In_ ApplicationDataContainer^ historyContainer)
{
    CalculationManager::HISTORYITEM historyItem;
    if (historyContainer->Values->HasKey(historyItemKey))
    {
        Object^ historyItemValues = historyContainer->Values->Lookup(historyItemKey);

        if (historyItemValues == nullptr)
        {
            throw ref new Platform::Exception(E_POINTER, ref new Platform::String(L"History Item is NULL"));
        }

        String^ historyData = safe_cast<Platform::String^>(historyItemValues);
        IBuffer^ buffer = CryptographicBuffer::DecodeFromBase64String(historyData);

        if (buffer == nullptr)
        {
            throw ref new Platform::Exception(E_POINTER, ref new Platform::String(L"History Item is NULL"));
        }

        DataReader^ reader = DataReader::FromBuffer(buffer);
        auto exprLen = reader->ReadUInt32();
        auto expression = reader->ReadString(exprLen);
        historyItem.historyItemVector.expression = expression->Data();

        auto resultLen = reader->ReadUInt32();
        auto result = reader->ReadString(resultLen);
        historyItem.historyItemVector.result = result->Data();

        historyItem.historyItemVector.spCommands = Utils::DeserializeCommands(reader);
        historyItem.historyItemVector.spTokens = Utils::DeserializeTokens(reader);
    }
    else
    {
        throw ref new Platform::Exception(E_ACCESSDENIED, ref new Platform::String(L"History Item not found"));
    }
    return historyItem;
}

bool HistoryViewModel::IsValid(_In_ CalculationManager::HISTORYITEM item)
{
    return (!item.historyItemVector.expression.empty() &&
        !item.historyItemVector.result.empty() &&
        (bool)item.historyItemVector.spCommands &&
        (bool)item.historyItemVector.spTokens);
}

void HistoryViewModel::UpdateItemSize()
{
    ItemSize = Items->Size;
}

void HistoryViewModel::MakeHistoryClearedNarratorAnnouncement(String^ resourceKey, String^& formatVariable)
{
    String^ announcement = LocalizationStringUtil::GetLocalizedNarratorAnnouncement(resourceKey, formatVariable);

    HistoryAnnouncement = CalculatorAnnouncement::GetHistoryClearedAnnouncement(announcement);
}
