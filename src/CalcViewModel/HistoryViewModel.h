// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/CalculatorManager.h"
#include "Common/Automation/NarratorAnnouncement.h"
#include "Common/CalculatorDisplay.h"
#include "Common/NavCategory.h"
#include "HistoryItemViewModel.h"

namespace CalculatorApp
{
    namespace CM = CalculationManager;

    namespace ViewModel
    {
        public delegate void HideHistoryClickedHandler();
        public delegate void HistoryItemClickedHandler(CalculatorApp::ViewModel::HistoryItemViewModel^ e);

        [Windows::UI::Xaml::Data::Bindable]
        public ref class HistoryViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {

        public:
            OBSERVABLE_OBJECT();
            OBSERVABLE_PROPERTY_RW(int, ItemSize);
            OBSERVABLE_PROPERTY_RW(Windows::UI::Xaml::Interop::IBindableObservableVector^, Items);
            OBSERVABLE_PROPERTY_RW(bool, AreHistoryShortcutsEnabled);
            OBSERVABLE_PROPERTY_RW(CalculatorApp::Common::Automation::NarratorAnnouncement^, HistoryAnnouncement);

            void OnHistoryItemAdded(_In_ unsigned int addedItemIndex);

            COMMAND_FOR_METHOD(HideCommand, HistoryViewModel::OnHideCommand);
            void OnHideCommand(_In_ Platform::Object^ e);
            COMMAND_FOR_METHOD(ClearCommand, HistoryViewModel::OnClearCommand);
            void OnClearCommand(_In_ Platform::Object^ e);

            // events that are created
            event HideHistoryClickedHandler^ HideHistoryClicked;
            event HistoryItemClickedHandler^ HistoryItemClicked;
            void ShowItem(_In_ CalculatorApp::ViewModel::HistoryItemViewModel^ e);
            void ClearHistory();
            void RestoreCompleteHistory();

        internal:
            HistoryViewModel(_In_ CalculationManager::CalculatorManager* calculatorManager);
            void SetCalculatorDisplay(CalculatorDisplay &calculatorDisplay);
            void ReloadHistory(_In_ CalculatorApp::Common::ViewMode currentMode);

            void DeleteItem(_In_ CalculatorApp::ViewModel::HistoryItemViewModel^ e);

            // store history in app data functions
            Platform::String^ SerializeHistoryItem(_In_ std::shared_ptr<CalculationManager::HISTORYITEM> const &item);
            void SaveHistory();

        private:
            CalculationManager::CalculatorManager* const m_calculatorManager;
            CalculatorDisplay m_calculatorDisplay;
            CalculationManager::CALCULATOR_MODE m_currentMode;
            Platform::String^ m_localizedHistoryCleared;

            void RestoreHistory(_In_ CalculationManager::CALCULATOR_MODE cMode);
            CalculationManager::HISTORYITEM DeserializeHistoryItem(_In_ Platform::String^ historyItemKey, _In_ Windows::Storage::ApplicationDataContainer^ historyContainer);
            Windows::Storage::ApplicationDataContainer^ GetHistoryContainer(_In_ CalculationManager::CALCULATOR_MODE cMode);
            Platform::String^ GetHistoryContainerKey(_In_ CalculationManager::CALCULATOR_MODE cMode);
            void ClearHistoryContainer(_In_ CalculationManager::CALCULATOR_MODE cMode);
            void UpdateHistoryVectorLength(_In_ int newValue, _In_ CalculationManager::CALCULATOR_MODE cMode);
            bool IsValid(_In_ CalculationManager::HISTORYITEM item);

            void MakeHistoryClearedNarratorAnnouncement(Platform::String^ resourceKey, Platform::String^& formatVariable);

            friend class CalculatorDisplay;
            void UpdateItemSize();
        };
    }
}
