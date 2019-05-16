// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
namespace CalculatorApp
{
    using CalculatorApp.Common;
    using CalculatorApp.Common.Automation;
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Linq;
    using System.Windows.Input;
    using Windows.Storage;
    using CM = CalculationManager;

    namespace ViewModel
    {
        public
            delegate void HideHistoryClickedHandler();
        public
            delegate void HistoryItemClickedHandler(CalculatorApp.ViewModel.HistoryItemViewModel e);

        [Windows.UI.Xaml.Data.Bindable]
        public sealed class HistoryViewModel : INotifyPropertyChanged
        {
            public event PropertyChangedEventHandler PropertyChanged;
            private void RaisePropertyChanged([System.Runtime.CompilerServices.CallerMemberName]string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));
            private int m_ItemSize;
            public int ItemSize { get => m_ItemSize; set { m_ItemSize = value; RaisePropertyChanged("ItemSize"); } }


            private ObservableCollection<HistoryItemViewModel> m_Items;
            public ObservableCollection<HistoryItemViewModel> Items { get => m_Items; set { m_Items = value; RaisePropertyChanged("Items"); } }


            private bool m_AreHistoryShortcutsEnabled;
            public bool AreHistoryShortcutsEnabled { get => m_AreHistoryShortcutsEnabled; set { m_AreHistoryShortcutsEnabled = value; RaisePropertyChanged("AreHistoryShortcutsEnabled"); } }


            private CalculatorApp.Common.Automation.NarratorAnnouncement m_HistoryAnnouncement;
            public CalculatorApp.Common.Automation.NarratorAnnouncement HistoryAnnouncement { get => m_HistoryAnnouncement; set { m_HistoryAnnouncement = value; RaisePropertyChanged("HistoryAnnouncement"); } }


            public ICommand HideCommand { get; }

            public ICommand ClearCommand { get; }

            // events that are created
            public event HideHistoryClickedHandler HideHistoryClicked;
            public event HistoryItemClickedHandler HistoryItemClicked;

            CalculationManager.CalculatorManager m_calculatorManager;
            CalculatorDisplay m_calculatorDisplay;
            CalculationManager.CALCULATOR_MODE m_currentMode;
            string m_localizedHistoryCleared;


            static string HistoryVectorLengthKey = "HistoryVectorLength";

            const string HistoryCleared = "HistoryList_Cleared";

            public HistoryViewModel(CalculationManager.CalculatorManager calculatorManager)
            { 
                HideCommand = new DelegateCommand(OnHideCommand);
                ClearCommand  = new DelegateCommand(OnClearCommand);

            m_calculatorManager = calculatorManager;
                m_localizedHistoryCleared = null;

                AreHistoryShortcutsEnabled = true;

                Items = new ObservableCollection<HistoryItemViewModel>();
                ItemSize = 0;
            }

            void RestoreCompleteHistory()
            {
                RestoreHistory(CalculationManager.CALCULATOR_MODE.CM_STD);
                RestoreHistory(CalculationManager.CALCULATOR_MODE.CM_SCI);
            }

            // this will reload Items with the history list based on current mode
            public void ReloadHistory(ViewMode currentMode)
            {
                if (currentMode == ViewMode.Standard)
                {
                    m_currentMode = CalculationManager.CALCULATOR_MODE.CM_STD;
                }
                else if (currentMode == ViewMode.Scientific)
                {
                    m_currentMode = CalculationManager.CALCULATOR_MODE.CM_SCI;
                }
                else
                {
                    return;
                }

                var historyListModel = m_calculatorManager.GetHistoryItems(m_currentMode);
                var historyListVM = new ObservableCollection<HistoryItemViewModel>();
                var localizer = LocalizationSettings.GetInstance();
                if (historyListModel.Count > 0)
                {
                    foreach(var it in historyListModel.AsEnumerable().Reverse())
                    {
                        string expression = it.historyItemVector.expression;
                        string result = it.historyItemVector.result;
                        localizer.LocalizeDisplayValue(ref expression);
                        localizer.LocalizeDisplayValue(ref result);

                        var item = new HistoryItemViewModel(
                            expression,
                            result,
                            it.historyItemVector.spTokens,
                            it.historyItemVector.spCommands);
                        historyListVM.Add(item);
                    }
                }

                Items = historyListVM;
                UpdateItemSize();
            }

            public void OnHistoryItemAdded(int addedItemIndex)
            {
                var newItem = m_calculatorManager.GetHistoryItem(addedItemIndex);
                var localizer = LocalizationSettings.GetInstance();
                string expression = newItem.historyItemVector.expression;
                string result = newItem.historyItemVector.result;
                localizer.LocalizeDisplayValue(ref expression);
                localizer.LocalizeDisplayValue(ref result);
                var item = new HistoryItemViewModel(
                    expression,
                    result,
                    newItem.historyItemVector.spTokens,
                    newItem.historyItemVector.spCommands);

                // check if we have not hit the max items
                if (Items.Count >= m_calculatorManager.MaxHistorySize())
                {
                    // this means the item already exists
                    Items.RemoveAt(Items.Count - 1);
                }

                Debug.Assert(addedItemIndex <= m_calculatorManager.MaxHistorySize() && addedItemIndex >= 0);
                Items.Insert(0, item);
                UpdateItemSize();
                SaveHistory();
            }

            public void SetCalculatorDisplay(CalculatorDisplay calculatorDisplay)
            {
                calculatorDisplay.SetHistoryCallback(new WeakReference(this));
            }

            void ShowItem(HistoryItemViewModel e)
            {
                HistoryItemClicked(e);
            }

            void DeleteItem(HistoryItemViewModel e)
            {
                int itemIndex= Items.IndexOf(e);
                if (itemIndex != -1)
                {
                    if (m_calculatorManager.RemoveHistoryItem(itemIndex))
                    {
                        // Keys for the history container are index based.
                        // SaveHistory() re-inserts the items anyway, so it's faster to just clear out the container.
                        CalculationManager.CALCULATOR_MODE currentMode = m_currentMode;
                        ApplicationDataContainer historyContainer = GetHistoryContainer(currentMode);
                        historyContainer.Values.Clear();

                        Items.RemoveAt(itemIndex);
                        UpdateItemSize();
                        SaveHistory();
                    }
                }
            }

            void OnHideCommand(object e)
            {
                // added at VM layer so that the views do not have to individually raise events
                HideHistoryClicked();
            }

            void OnClearCommand(object e)
            {
                // TraceLogger.GetInstance().LogClearHistory();
                if (AreHistoryShortcutsEnabled == true)
                {
                    m_calculatorManager.ClearHistory();

                    if (Items.Count > 0)
                    {
                        CalculationManager.CALCULATOR_MODE currentMode = m_currentMode;
                        ClearHistoryContainer(currentMode);
                        Items.Clear();
                        UpdateItemSize();
                    }

                    MakeHistoryClearedNarratorAnnouncement(HistoryCleared, m_localizedHistoryCleared);
                }
            }

            // this method restores history vector per mode
            void RestoreHistory(CalculationManager.CALCULATOR_MODE cMode)
            {
                ApplicationDataContainer historyContainer = GetHistoryContainer(cMode);
                List<CalculationManager.HISTORYITEM> historyVector = new List<CalculationManager.HISTORYITEM>();
                var historyVectorLength = (int)(historyContainer.Values[HistoryVectorLengthKey]);
                bool failure = false;

                if (historyVectorLength > 0)
                {
                    for (int i = 0; i < historyVectorLength; ++i)
                    {
                        try
                        {
                            // deserialize each item
                            var item = DeserializeHistoryItem(i.ToString(), historyContainer);
                            historyVector.Add(item);
                        }
                        catch (Exception e)
                        {
                            failure = true;
                            break;
                        }
                    }

                    if (!failure)
                    {
                        // if task has been cancelled set history to 0
                        m_calculatorManager.SetHistory(cMode, historyVector);

                        // update length once again for consistency between stored number of items and length
                        UpdateHistoryVectorLength((int)(historyVector.Count), cMode);
                    }
                    else
                    {
                        // in case of failure do not show any item
                        UpdateHistoryVectorLength(0, cMode);
                    }
                }
            }

            string GetHistoryContainerKey(CalculationManager.CALCULATOR_MODE cMode)
            {
                int modeValue = (int)(cMode);
                return String.Concat(modeValue.ToString(), "_History");
            }

            ApplicationDataContainer GetHistoryContainer(CalculationManager.CALCULATOR_MODE cMode)
            {
                ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
                ApplicationDataContainer historyContainer;

                // naming container based on mode
                string historyContainerKey = GetHistoryContainerKey(cMode);

                if (localSettings.Containers.ContainsKey(historyContainerKey))
                {
                    historyContainer = localSettings.Containers[historyContainerKey];
                }
                else
                {
                    // create container for adding data
                    historyContainer = localSettings.CreateContainer(historyContainerKey, ApplicationDataCreateDisposition.Always);
                    int initialHistoryVectorLength = 0;
                    historyContainer.Values.Add(HistoryVectorLengthKey, initialHistoryVectorLength);
                }

                return historyContainer;
            }

            void ClearHistoryContainer(CalculationManager.CALCULATOR_MODE cMode)
            {
                ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
                localSettings.DeleteContainer(GetHistoryContainerKey(cMode));
            }

            // this method will be used to update the history item length
            void UpdateHistoryVectorLength(int newValue, CalculationManager.CALCULATOR_MODE cMode)
            {
                ApplicationDataContainer historyContainer = GetHistoryContainer(cMode);
                historyContainer.Values.Remove(HistoryVectorLengthKey);
                historyContainer.Values.Add(HistoryVectorLengthKey, newValue);
            }

            public void ClearHistory()
            {
                ClearHistoryContainer(CalculationManager.CALCULATOR_MODE.CM_STD);
                ClearHistoryContainer(CalculationManager.CALCULATOR_MODE.CM_SCI);
            }

            void SaveHistory()
            {
                ApplicationDataContainer historyContainer = GetHistoryContainer(m_currentMode);
                var currentHistoryVector = m_calculatorManager.GetHistoryItems(m_currentMode);
                bool failure = false;
                int index = 0;
                string serializedHistoryItem;

                foreach(var it in currentHistoryVector)
                {
                    try
                    {
                        serializedHistoryItem = SerializeHistoryItem(it);
                        historyContainer.Values[index.ToString()] = serializedHistoryItem;
                    }
                    catch (Exception e)
                    {
                        failure = true;
                        break;
                    }

                    ++index;
                }

                if (!failure)
                {
                    // insertion is successful
                    UpdateHistoryVectorLength(currentHistoryVector.Count, m_currentMode);
                }
                else
                {
                    UpdateHistoryVectorLength(0, m_currentMode);
                }
            }

            // this serializes a history item into a base64 encoded string
            string SerializeHistoryItem(CalculationManager.HISTORYITEM item)
            {
                //DataWriter writer = new DataWriter();
                //        var expr = item.historyItemVector.expression;
                //        var result = item.historyItemVector.result;
                //        var platformExpr = expr;
                //        writer.WriteUInt32(writer.MeasureString(platformExpr));
                //writer.WriteString(platformExpr);
                //var platformResult = result;
                //        writer.WriteUInt32(writer.MeasureString(platformResult));
                //writer.WriteString(platformResult);

                //Utils.SerializeCommandsAndTokens(item.historyItemVector.spTokens, item.historyItemVector.spCommands, writer);

                //IBuffer buffer = writer.DetachBuffer();
                //if (buffer == null)
                //{
                //    throw new Platform.Exception(E_POINTER, "History Item is NULL");
                //}

                //return CryptographicBuffer.EncodeToBase64String(buffer);
                return "";
            }

            CalculationManager.HISTORYITEM
            DeserializeHistoryItem(string historyItemKey, ApplicationDataContainer historyContainer)
            {
                //CalculationManager.HISTORYITEM historyItem;
                //if (historyContainer.Values.ContainsKey(historyItemKey))
                //{
                //    object historyItemValues = historyContainer.Values[historyItemKey];

                //    if (historyItemValues == null)
                //    {
                //        throw new Exception("History Item is NULL");
                //    }

                //    String historyData = (string)(historyItemValues);
                //    IBuffer buffer = CryptographicBuffer.DecodeFromBase64String(historyData);

                //    if (buffer == null)
                //    {
                //        throw new Platform.Exception(E_POINTER, "History Item is NULL");
                //    }

                //    DataReader reader = DataReader.FromBuffer(buffer);
                //    var exprLen = reader.ReadUInt32();
                //    var expression = reader.ReadString(exprLen);
                //    historyItem.historyItemVector.expression = expression.Data();

                //    var resultLen = reader.ReadUInt32();
                //    var result = reader.ReadString(resultLen);
                //    historyItem.historyItemVector.result = result.Data();

                //    historyItem.historyItemVector.spCommands = Utils.DeserializeCommands(reader);
                //    historyItem.historyItemVector.spTokens = Utils.DeserializeTokens(reader);
                //}
                //else
                //{
                //    throw new Platform.Exception(E_ACCESSDENIED, "History Item not found");
                //}
                //return historyItem;
                throw new NotImplementedException();
            }

            bool IsValid(CalculationManager.HISTORYITEM item)
            {
                return 
                    !string.IsNullOrEmpty(item.historyItemVector.expression) 
                    && !string.IsNullOrEmpty(item.historyItemVector.result) 
                    && item.historyItemVector.spCommands != null
                    && item.historyItemVector.spTokens != null;
            }

            void UpdateItemSize()
            {
                ItemSize = Items.Count;
            }

            void MakeHistoryClearedNarratorAnnouncement(String resourceKey, String formatVariable)
            {
                String announcement = LocalizationStringUtil.GetLocalizedNarratorAnnouncement(resourceKey, formatVariable);

                HistoryAnnouncement = CalculatorAnnouncement.GetHistoryClearedAnnouncement(announcement);
            }
        }
    }
}
