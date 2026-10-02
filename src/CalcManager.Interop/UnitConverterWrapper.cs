// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using CalcManager.UnitConversionManager;

namespace CalcManager.Interop
{
    public sealed class UnitConverterWrapper
    {
        public static Unit ToNativeUnit(UnitWrapper w)
        {
            return new Unit(
                w.Id,
                w.Name,
                w.Abbreviation,
                w.IsConversionSource,
                w.IsConversionTarget,
                w.IsWhimsical)
            {
                AccessibleName = w.AccessibleName
            };
        }

        public static UnitWrapper ToWinRTUnit(Unit u)
        {
            return new UnitWrapper
            {
                Id = u.Id,
                Name = u.Name,
                AccessibleName = u.AccessibleName,
                Abbreviation = u.Abbreviation,
                IsConversionSource = u.IsConversionSource,
                IsConversionTarget = u.IsConversionTarget,
                IsWhimsical = u.IsWhimsical
            };
        }

        public static Category ToNativeCategory(CategoryWrapper w)
        {
            return new Category(w.Id, w.Name, w.SupportsNegative);
        }

        public static CategoryWrapper ToWinRTCategory(Category c)
        {
            return new CategoryWrapper
            {
                Id = c.Id,
                Name = c.Name,
                SupportsNegative = c.SupportsNegative
            };
        }

        private class ConverterDataLoaderBridge : IConverterDataLoader
        {
            private readonly ConverterDataLoaderBase m_receiver;

            public ConverterDataLoaderBridge(ConverterDataLoaderBase receiver)
            {
                m_receiver = receiver;
            }

            public void LoadData()
            {
                m_receiver?.LoadData();
            }

            public List<Category> GetOrderedCategories()
            {
                var categories = m_receiver?.GetOrderedCategories();
                return categories?.Select(ToNativeCategory).ToList() ?? new List<Category>();
            }

            public List<Unit> GetOrderedUnits(Category c)
            {
                var units = m_receiver?.GetOrderedUnits(ToWinRTCategory(c));
                return units?.Select(ToNativeUnit).ToList() ?? new List<Unit>();
            }

            public Dictionary<Unit, ConversionData> LoadOrderedRatios(Unit u)
            {
                var entries = m_receiver?.LoadOrderedRatios(ToWinRTUnit(u));
                var result = new Dictionary<Unit, ConversionData>();
                if (entries != null)
                {
                    foreach (var entry in entries)
                    {
                        var targetUnit = ToNativeUnit(entry.Unit);
                        var convData = new ConversionData(entry.Ratio, entry.Offset, entry.OffsetFirst);
                        result[targetUnit] = convData;
                    }
                }
                return result;
            }

            public bool SupportsCategory(Category target)
            {
                return m_receiver?.SupportsCategory(ToWinRTCategory(target)) ?? false;
            }
        }

        private class UnitConverterVMCallbackBridge : IUnitConverterVMCallback
        {
            private readonly UnitConverterVMCallbackBase m_receiver;

            public UnitConverterVMCallbackBridge(UnitConverterVMCallbackBase receiver)
            {
                m_receiver = receiver;
            }

            public void DisplayCallback(string from, string to)
            {
                m_receiver?.DisplayCallback(from, to);
            }

            public void SuggestedValueCallback(List<Tuple<string, Unit>> suggestedValues)
            {
                var winrtValues = new List<SuggestedValueWrapper>();
                if (suggestedValues != null)
                {
                    foreach (var item in suggestedValues)
                    {
                        winrtValues.Add(new SuggestedValueWrapper
                        {
                            Value = item.Item1,
                            Unit = ToWinRTUnit(item.Item2)
                        });
                    }
                }
                m_receiver?.SuggestedValueCallback(winrtValues.ToArray());
            }

            public void MaxDigitsReached()
            {
                m_receiver?.MaxDigitsReached();
            }
        }

        private class ViewModelCurrencyCallbackBridge : IViewModelCurrencyCallback
        {
            private readonly ViewModelCurrencyCallbackBase m_receiver;

            public ViewModelCurrencyCallbackBridge(ViewModelCurrencyCallbackBase receiver)
            {
                m_receiver = receiver;
            }

            public void CurrencyDataLoadFinished(bool didLoad)
            {
                m_receiver?.CurrencyDataLoadFinished(didLoad);
            }

            public void CurrencySymbolsCallback(string fromSymbol, string toSymbol)
            {
                m_receiver?.CurrencySymbolsCallback(fromSymbol, toSymbol);
            }

            public void CurrencyRatiosCallback(string ratioEquality, string accRatioEquality)
            {
                m_receiver?.CurrencyRatiosCallback(ratioEquality, accRatioEquality);
            }

            public void CurrencyTimestampCallback(string timestamp, bool isWeekOldData)
            {
                m_receiver?.CurrencyTimestampCallback(timestamp, isWeekOldData);
            }

            public void NetworkBehaviorChanged(int newBehavior)
            {
                m_receiver?.NetworkBehaviorChanged(newBehavior);
            }
        }

        private readonly ConverterDataLoaderBridge m_dataLoaderBridge;
        private readonly UnitConverter m_converter;
        private UnitConverterVMCallbackBridge m_vmCallbackBridge;
        private ViewModelCurrencyCallbackBridge m_currencyCallbackBridge;

        public UnitConverterWrapper(ConverterDataLoaderBase dataLoader)
        {
            m_dataLoaderBridge = new ConverterDataLoaderBridge(dataLoader);
            m_converter = new UnitConverter(m_dataLoaderBridge);
        }

        public void Initialize() => m_converter.Initialize();

        public CategoryWrapper[] GetCategories()
        {
            var categories = m_converter.GetCategories();
            return categories?.Select(ToWinRTCategory).ToArray() ?? Array.Empty<CategoryWrapper>();
        }

        public CategorySelectionResult SetCurrentCategory(CategoryWrapper category)
        {
            var result = m_converter.SetCurrentCategory(ToNativeCategory(category));
            var winrtUnits = result.Item1?.Select(ToWinRTUnit).ToArray() ?? Array.Empty<UnitWrapper>();
            return new CategorySelectionResult(winrtUnits, ToWinRTUnit(result.Item2), ToWinRTUnit(result.Item3));
        }

        public CategoryWrapper GetCurrentCategory() => ToWinRTCategory(m_converter.GetCurrentCategory());

        public void SetCurrentUnitTypes(UnitWrapper fromType, UnitWrapper toType)
        {
            m_converter.SetCurrentUnitTypes(ToNativeUnit(fromType), ToNativeUnit(toType));
        }

        public void SwitchActive(string newValue) => m_converter.SwitchActive(newValue);
        public bool IsSwitchedActive => m_converter.IsSwitchedActive();

        public string SaveUserPreferences() => m_converter.SaveUserPreferences();
        public void RestoreUserPreferences(string userPreference) => m_converter.RestoreUserPreferences(userPreference);

        public void SendCommand(UnitConverterCommand command)
        {
            m_converter.SendCommand((CalcManager.UnitConversionManager.Command)(int)command);
        }

        public void SetViewModelCallback(UnitConverterVMCallbackBase callback)
        {
            m_vmCallbackBridge = new UnitConverterVMCallbackBridge(callback);
            m_converter.SetViewModelCallback(m_vmCallbackBridge);
        }

        public void SetViewModelCurrencyCallback(ViewModelCurrencyCallbackBase callback)
        {
            m_currencyCallbackBridge = new ViewModelCurrencyCallbackBridge(callback);
            m_converter.SetViewModelCurrencyCallback(m_currencyCallbackBridge);
        }

        public void Calculate() => m_converter.Calculate();
        public void ResetCategoriesAndRatios() => m_converter.ResetCategoriesAndRatios();
    }
}
