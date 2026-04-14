// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using CalculatorApp.ViewModel.Common;

namespace CalculatorApp.ViewModel.DataLoaders
{
    public enum CurrencyLoadStatus
    {
        NotLoaded = 0,
        FailedToLoad = 1,
        LoadedFromCache = 2,
        LoadedFromWeb = 3
    }

    public static class UnitConverterResourceKeys
    {
        public const string CurrencyUnitFromKey = "CurrencyUnitFrom";
        public const string CurrencyUnitToKey = "CurrencyUnitTo";
    }

    public static class CurrencyDataLoaderConstants
    {
        public const string CacheTimestampKey = "CurrencyConverterTimestamp";
        public const string CacheLangcodeKey = "CurrencyConverterLangcode";
        public const string CacheDelimiter = "_";
        public const string StaticDataFilename = "CurrencyStaticData";
        public const string AllRatiosDataFilename = "CurrencyAllRatiosData";
        public const long DayDuration = 864000000000L; // 24h in 100ns ticks
    }

    public sealed class CurrencyDataLoader
    {
        private readonly CurrencyHttpClient _client;
        private readonly string _responseLanguage;
        private readonly bool _isRtlLanguage;
        private readonly object _currencyUnitsMutex = new object();

        private List<CurrencyUnit> _currencyUnits;
        private Dictionary<int, Dictionary<int, ConversionData>> _currencyRatioMap;
        private Dictionary<int, CurrencyUnitMetadata> _currencyMetadata;

        private IViewModelCurrencyCallback _vmCallback;

        private CurrencyLoadStatus _loadStatus;
        private NetworkAccessBehavior _networkAccessBehavior;
        private bool _meteredOverrideSet;
        private DateTimeOffset _cacheTimestamp;

        public CurrencyDataLoader(string overrideLanguage = null)
        {
            _client = new CurrencyHttpClient();
            _responseLanguage = overrideLanguage ?? Windows.Globalization.Language.CurrentInputMethodLanguageTag;
            _isRtlLanguage = false;
            _currencyUnits = new List<CurrencyUnit>();
            _currencyRatioMap = new Dictionary<int, Dictionary<int, ConversionData>>();
            _currencyMetadata = new Dictionary<int, CurrencyUnitMetadata>();
            _loadStatus = CurrencyLoadStatus.NotLoaded;
            _networkAccessBehavior = NetworkAccessBehavior.Normal;
        }

        public bool LoadFinished() => _loadStatus != CurrencyLoadStatus.NotLoaded;
        public bool LoadedFromCache() => _loadStatus == CurrencyLoadStatus.LoadedFromCache;
        public bool LoadedFromWeb() => _loadStatus == CurrencyLoadStatus.LoadedFromWeb;

        public void LoadData()
        {
            // Load currency data from cache or web
        }

        public List<CategoryData> GetOrderedCategories()
        {
            return new List<CategoryData>();
        }

        public List<UnitData> GetOrderedUnits(int categoryId)
        {
            lock (_currencyUnitsMutex)
            {
                return new List<UnitData>();
            }
        }

        public Dictionary<int, ConversionData> LoadOrderedRatios(int unitId)
        {
            lock (_currencyUnitsMutex)
            {
                if (_currencyRatioMap.TryGetValue(unitId, out var ratios))
                {
                    return new Dictionary<int, ConversionData>(ratios);
                }
                return new Dictionary<int, ConversionData>();
            }
        }

        public bool SupportsCategory(int categoryId)
        {
            return categoryId == NavCategoryStates.Serialize(ViewMode.Currency);
        }

        public void SetViewModelCallback(IViewModelCurrencyCallback callback)
        {
            _vmCallback = callback;
        }

        public (string Symbol1, string Symbol2) GetCurrencySymbols(int unit1Id, int unit2Id)
        {
            string symbol1 = _currencyMetadata.TryGetValue(unit1Id, out var meta1) ? meta1.Symbol : string.Empty;
            string symbol2 = _currencyMetadata.TryGetValue(unit2Id, out var meta2) ? meta2.Symbol : string.Empty;
            return (symbol1, symbol2);
        }

        public (string Ratio1, string Ratio2) GetCurrencyRatioEquality(int unit1Id, int unit2Id)
        {
            return (string.Empty, string.Empty);
        }

        public string GetCurrencyTimestamp()
        {
            return _cacheTimestamp.ToString();
        }

        public static double RoundCurrencyRatio(double ratio)
        {
            return Math.Round(ratio, 4);
        }

        public async Task<bool> TryLoadDataFromCacheAsync()
        {
            try
            {
                // Try to load from local cache
                return await TryFinishLoadFromCacheAsync();
            }
            catch
            {
                return false;
            }
        }

        public async Task<bool> TryLoadDataFromWebAsync()
        {
            try
            {
                _client.Initialize(string.Empty, _responseLanguage);
                string staticData = await _client.GetCurrencyMetadataAsync();
                string allRatios = await _client.GetCurrencyRatiosAsync();

                if (string.IsNullOrEmpty(staticData) || string.IsNullOrEmpty(allRatios))
                {
                    return false;
                }

                // Parse and store data
                _loadStatus = CurrencyLoadStatus.LoadedFromWeb;
                NotifyDataLoadFinished(true);
                return true;
            }
            catch
            {
                NotifyDataLoadFinished(false);
                return false;
            }
        }

        public Task<bool> TryLoadDataFromWebOverrideAsync()
        {
            _meteredOverrideSet = true;
            return TryLoadDataFromWebAsync();
        }

        public void OnNetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
        {
            _networkAccessBehavior = newBehavior;
        }

        private void ResetLoadStatus()
        {
            _loadStatus = CurrencyLoadStatus.NotLoaded;
        }

        private void NotifyDataLoadFinished(bool didLoad)
        {
            _vmCallback?.CurrencyDataLoadFinished(didLoad);
        }

        private async Task<bool> TryFinishLoadFromCacheAsync()
        {
            // Implementation would read cached data from local storage
            await Task.Yield();
            return false;
        }
    }

    #region Supporting types

    public class CurrencyUnit
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string Abbreviation { get; set; }
        public string Symbol { get; set; }
    }

    public class CurrencyUnitMetadata
    {
        public string Symbol { get; set; }

        public CurrencyUnitMetadata(string symbol)
        {
            Symbol = symbol;
        }
    }

    public class ConversionData
    {
        public double Ratio { get; set; }
        public double Offset { get; set; }
        public bool OffsetFirst { get; set; }

        public ConversionData(double ratio, double offset = 0, bool offsetFirst = false)
        {
            Ratio = ratio;
            Offset = offset;
            OffsetFirst = offsetFirst;
        }
    }

    public class CategoryData
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public bool SupportsNegative { get; set; }
    }

    public class UnitData
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string Abbreviation { get; set; }
    }

    public interface IViewModelCurrencyCallback
    {
        void CurrencyDataLoadFinished(bool didLoad);
        void CurrencyTimestampUpdated(string timestamp, bool isWeekOld);
        void NetworkBehaviorChanged(NetworkAccessBehavior newBehavior);
    }

    #endregion
}
