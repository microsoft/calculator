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
        ref class StandardCalculatorViewModel;

    public
        delegate void HideHistoryClickedHandler();
    public
        delegate void HistoryItemClickedHandler(CalculatorApp::ViewModel::HistoryItemViewModel ^ e);

        [Windows::UI::Xaml::Data::Bindable] public ref class HistoryViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            OBSERVABLE_OBJECT();
            OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<HistoryItemViewModel ^> ^, Items);
            OBSERVABLE_PROPERTY_RW(bool, AreHistoryShortcutsEnabled);
            OBSERVABLE_PROPERTY_R(CalculatorApp::ViewModel::Common::Automation::NarratorAnnouncement ^, HistoryAnnouncement);
            property int ItemsCount
            {
                int get()
                {
                    return Items->Size;
                }
            }

            void OnHistoryItemAdded(_In_ unsigned int addedItemIndex);

            COMMAND_FOR_METHOD(HideCommand, HistoryViewModel::OnHideCommand);
            void OnHideCommand(_In_ Platform::Object ^ e);
            COMMAND_FOR_METHOD(ClearCommand, HistoryViewModel::OnClearCommand);
            void OnClearCommand(_In_ Platform::Object ^ e);

            // events that are created
            event HideHistoryClickedHandler ^ HideHistoryClicked;
            event HistoryItemClickedHandler ^ HistoryItemClicked;
            void ShowItem(_In_ CalculatorApp::ViewModel::HistoryItemViewModel ^ e);
            void DeleteItem(_In_ CalculatorApp::ViewModel::HistoryItemViewModel ^ e);
            void ReloadHistory(_In_ CalculatorApp::ViewModel::Common::ViewMode currentMode);

            internal : HistoryViewModel(_In_ CalculationManager::CalculatorManager* calculatorManager);
            void SetCalculatorDisplay(Common::CalculatorDisplay& calculatorDisplay);
            unsigned long long GetMaxItemSize();


        private:
            CalculationManager::CalculatorManager* const m_calculatorManager;
            Common::CalculatorDisplay m_calculatorDisplay;
            CalculationManager::CalculatorMode m_currentMode;
            Platform::String ^ m_localizedHistoryCleared;
            Platform::String ^ m_localizedHistorySlotCleared;
        };
    }
}
