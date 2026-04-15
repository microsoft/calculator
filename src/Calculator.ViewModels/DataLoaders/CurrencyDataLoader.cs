// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using CalculatorApp.ViewModel.Common;
using Windows.Data.Json;
using Windows.Globalization.DateTimeFormatting;
using Windows.Globalization.NumberFormatting;
using Windows.Storage;
using Windows.UI.Core;

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
        public const string CurrencyUnitFromKey = "CURRENCY_UNIT_FROM_KEY";
        public const string CurrencyUnitToKey = "CURRENCY_UNIT_TO_KEY";
    }

    public static class CurrencyDataLoaderConstants
    {
        public const string CacheTimestampKey = "CURRENCY_CONVERTER_TIMESTAMP";
        public const string CacheLangcodeKey = "CURRENCY_CONVERTER_LANGCODE";
        public const string CacheDelimiter = "%";
        public const string StaticDataFilename = "CURRENCY_CONVERTER_STATIC_DATA.txt";
        public const string AllRatiosDataFilename = "CURRENCY_CONVERTER_ALL_RATIOS_DATA.txt";

        // (1 interval/100 nanosecond)(100 nanosecond/1e-7 s)(60 s/1 min)(60 min/1 hr)(24 hr/1 day)
        public const long DayDuration = 1L * 60 * 60 * 24 * 10000000;
        public const long WeekDuration = DayDuration * 7;
    }

    public sealed class CurrencyDataLoader
    {
        private const int UnitEnd = 168;
        private const string DefaultCurrencyCode = "USD";
        private const string DefaultFromCurrency = DefaultCurrencyCode;
        private const string DefaultToCurrency = "EUR";
        private const string DefaultFromToCurrencyFileUri = "ms-appx:///DataLoaders/DefaultFromToCurrency.json";
        private const string FromKey = "from";
        private const string ToKey = "to";

        private const int FormatterRateFractionPadding = 2;
        private const int FormatterRateMinDecimals = 4;
        private const int FormatterRateMinSignificantDecimals = 4;

        private static readonly string[] StaticDataProperties = { "CountryCode", "CountryName", "CurrencyCode", "CurrencyName", "CurrencySymbol" };
        private const string RatioKey = "Rt";
        private const string CurrencyCodeKey = "An";

        private readonly CurrencyHttpClient _client;
        private readonly string _responseLanguage;
        private readonly bool _isRtlLanguage;
        private readonly object _currencyUnitsMutex = new object();

        private List<CurrencyUnit> _currencyUnits;
        private Dictionary<int, Dictionary<int, ConversionData>> _currencyRatioMap;
        private Dictionary<int, CurrencyUnitMetadata> _currencyMetadata;

        private IViewModelCurrencyCallback _vmCallback;

        private DecimalFormatter _ratioFormatter;
        private string _ratioFormat;
        private string _timestampFormat;
        private DateTimeOffset _cacheTimestamp;

        private CurrencyLoadStatus _loadStatus;

        private readonly NetworkManager _networkManager;
        private NetworkAccessBehavior _networkAccessBehavior;
        private bool _meteredOverrideSet;

        public CurrencyDataLoader(string overrideLanguage = null)
        {
            _loadStatus = CurrencyLoadStatus.NotLoaded;
            _responseLanguage = "en-US";
            _ratioFormat = string.Empty;
            _timestampFormat = string.Empty;
            _networkManager = new NetworkManager();
            _meteredOverrideSet = false;

            if (overrideLanguage != null)
            {
                Debug.Assert(overrideLanguage.Length > 0, "overrideLanguage shall not be empty.");
                _responseLanguage = overrideLanguage;
            }
            else
            {
                var languages = Windows.System.UserProfile.GlobalizationPreferences.Languages;
                if (languages.Count > 0)
                {
                    string parsed = ParseLanguageCode(languages[0]);
                    if (parsed != null)
                    {
                        _responseLanguage = parsed;
                    }
                }
            }

            _client = new CurrencyHttpClient();
            _client.Initialize(DefaultCurrencyCode, _responseLanguage);

            var localizationSettings = LocalizationSettings.GetInstance();
            _isRtlLanguage = false;
            if (CoreWindow.GetForCurrentThread() != null)
            {
                _isRtlLanguage = localizationSettings.IsRtlLayout();
            }

            _ratioFormatter = localizationSettings.GetRegionalSettingsAwareDecimalFormatter();
            _ratioFormatter.IsGrouped = true;
            _ratioFormatter.IsDecimalPointAlwaysDisplayed = true;
            _ratioFormatter.FractionDigits = FormatterRateFractionPadding;

            var resProvider = AppResourceProvider.GetInstance();
            _ratioFormat = resProvider.GetResourceString("CurrencyFromToRatioFormat");
            _timestampFormat = resProvider.GetResourceString("CurrencyTimestampFormat");

            _currencyUnits = new List<CurrencyUnit>();
            _currencyRatioMap = new Dictionary<int, Dictionary<int, ConversionData>>();
            _currencyMetadata = new Dictionary<int, CurrencyUnitMetadata>();
        }

        public bool LoadFinished() => _loadStatus != CurrencyLoadStatus.NotLoaded;
        public bool LoadedFromCache() => _loadStatus == CurrencyLoadStatus.LoadedFromCache;
        public bool LoadedFromWeb() => _loadStatus == CurrencyLoadStatus.LoadedFromWeb;

        public async void LoadData()
        {
            if (!LoadFinished())
            {
                RegisterForNetworkBehaviorChanges();

                bool didLoad = false;

                didLoad = await TryLoadDataFromCacheAsync();
                if (!didLoad)
                {
                    didLoad = await TryLoadDataFromWebAsync();
                }

                UpdateDisplayedTimestamp();
                NotifyDataLoadFinished(didLoad);
            }
        }

        public List<CategoryData> GetOrderedCategories()
        {
            // This function should not be called.
            // The model will use the categories from UnitConverterDataLoader.
            return new List<CategoryData>();
        }

        public List<CurrencyUnit> GetOrderedUnits(int categoryId)
        {
            lock (_currencyUnitsMutex)
            {
                return new List<CurrencyUnit>(_currencyUnits);
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
            OnNetworkBehaviorChanged(_networkAccessBehavior);
        }

        public (string Symbol1, string Symbol2) GetCurrencySymbols(int unit1Id, int unit2Id)
        {
            lock (_currencyUnitsMutex)
            {
                string symbol1 = string.Empty;
                string symbol2 = string.Empty;

                if (_currencyMetadata.TryGetValue(unit1Id, out var meta1)
                    && _currencyMetadata.TryGetValue(unit2Id, out var meta2))
                {
                    symbol1 = meta1.Symbol;
                    symbol2 = meta2.Symbol;
                }

                return (symbol1, symbol2);
            }
        }

        public static double RoundCurrencyRatio(double ratio)
        {
            // Compute how many decimals we need to display two meaningful digits at minimum.
            // For example: 0.00000000342334 -> 0.000000003423, 0.000212 -> 0.000212
            int numberDecimals = FormatterRateMinDecimals;
            if (ratio < 1)
            {
                numberDecimals = Math.Max(FormatterRateMinDecimals, (int)(-Math.Log10(ratio)) + FormatterRateMinSignificantDecimals);
            }

            ulong scale = (ulong)Math.Pow(10, numberDecimals);
            return Math.Round(ratio * scale) / scale;
        }

        public (string Ratio1, string Ratio2) GetCurrencyRatioEquality(int unit1Id, int unit2Id)
        {
            try
            {
                if (_currencyRatioMap.TryGetValue(unit1Id, out var ratioMap))
                {
                    if (ratioMap.TryGetValue(unit2Id, out var convData))
                    {
                        double ratio = convData.Ratio;
                        double rounded = RoundCurrencyRatio(ratio);

                        var locSettings = LocalizationSettings.GetInstance();
                        char digit = locSettings.GetDigitSymbolFromEnUsDigit('1');
                        string digitSymbol = digit.ToString();
                        string roundedFormat = _ratioFormatter.Format(rounded);

                        // Look up the units by ID to get abbreviation and name
                        string unit1Abbreviation = string.Empty;
                        string unit1Name = string.Empty;
                        string unit2Abbreviation = string.Empty;
                        string unit2Name = string.Empty;

                        lock (_currencyUnitsMutex)
                        {
                            foreach (var u in _currencyUnits)
                            {
                                if (u.Id == unit1Id)
                                {
                                    unit1Abbreviation = u.Abbreviation;
                                    unit1Name = u.CountryName;
                                }
                                if (u.Id == unit2Id)
                                {
                                    unit2Abbreviation = u.Abbreviation;
                                    unit2Name = u.CountryName;
                                }
                            }
                        }

                        string ratioString = LocalizationStringUtil.GetLocalizedString(
                            _ratioFormat, digitSymbol, unit1Abbreviation, roundedFormat, unit2Abbreviation);

                        string accessibleRatioString = LocalizationStringUtil.GetLocalizedString(
                            _ratioFormat, digitSymbol, unit1Name, roundedFormat, unit2Name);

                        return (ratioString, accessibleRatioString);
                    }
                }
            }
            catch
            {
            }

            return (string.Empty, string.Empty);
        }

        public string GetCurrencyTimestamp()
        {
            if (_cacheTimestamp != default)
            {
                var dateFormatter = new DateTimeFormatter("shortdate");
                string date = dateFormatter.Format(_cacheTimestamp);

                var timeFormatter = new DateTimeFormatter("shorttime");
                string time = timeFormatter.Format(_cacheTimestamp);

                return LocalizationStringUtil.GetLocalizedString(_timestampFormat, date, time);
            }

            return string.Empty;
        }

        public async Task<bool> TryLoadDataFromCacheAsync()
        {
            try
            {
                ResetLoadStatus();

                var localSettings = ApplicationData.Current.LocalSettings;
                if (localSettings == null || !localSettings.Values.ContainsKey(CurrencyDataLoaderConstants.CacheTimestampKey))
                {
                    return false;
                }

                bool loadComplete = false;
                m_cacheTimestamp_raw = localSettings.Values[CurrencyDataLoaderConstants.CacheTimestampKey];
                _cacheTimestamp = (DateTimeOffset)m_cacheTimestamp_raw;

                if (Utils.IsDateTimeOlderThan(_cacheTimestamp, CurrencyDataLoaderConstants.DayDuration)
                    && _networkAccessBehavior == NetworkAccessBehavior.Normal)
                {
                    loadComplete = await TryLoadDataFromWebAsync();
                }

                if (!loadComplete)
                {
                    loadComplete = await TryFinishLoadFromCacheAsync();
                }

                return loadComplete;
            }
            catch
            {
                return false;
            }
        }

        // Backing field to preserve the raw boxed value from LocalSettings for round-tripping.
        private object m_cacheTimestamp_raw;

        public async Task<bool> TryLoadDataFromWebAsync()
        {
            try
            {
                ResetLoadStatus();

                if (_networkAccessBehavior == NetworkAccessBehavior.Offline
                    || (_networkAccessBehavior == NetworkAccessBehavior.OptIn && !_meteredOverrideSet))
                {
                    return false;
                }

                string staticDataResponse = await _client.GetCurrencyMetadataAsync();
                string allRatiosResponse = await _client.GetCurrencyRatiosAsync();
                if (staticDataResponse == null || allRatiosResponse == null)
                {
                    return false;
                }

                if (!TryParseWebResponses(staticDataResponse, allRatiosResponse,
                        out var staticData, out var ratioMap))
                {
                    return false;
                }

                // Set the timestamp before saving it below.
                _cacheTimestamp = Utils.GetUniversalSystemTime();

                try
                {
                    var localCacheFolder = ApplicationData.Current.LocalCacheFolder;
                    await Utils.WriteFileToFolder(localCacheFolder, CurrencyDataLoaderConstants.StaticDataFilename,
                        staticDataResponse, CreationCollisionOption.ReplaceExisting);
                    await Utils.WriteFileToFolder(localCacheFolder, CurrencyDataLoaderConstants.AllRatiosDataFilename,
                        allRatiosResponse, CreationCollisionOption.ReplaceExisting);

                    SaveLangCodeAndTimestamp();
                }
                catch
                {
                    // If we fail to save to cache it's okay, we should still continue.
                }

                _loadStatus = CurrencyLoadStatus.LoadedFromWeb;
                await FinalizeUnits(staticData, ratioMap);

                return true;
            }
            catch
            {
                return false;
            }
        }

        public async Task<bool> TryLoadDataFromWebOverrideAsync()
        {
            _meteredOverrideSet = true;
            bool didLoad = await TryLoadDataFromWebAsync();
            if (!didLoad)
            {
                _loadStatus = CurrencyLoadStatus.FailedToLoad;
            }

            return didLoad;
        }

        public void OnNetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
        {
            _networkAccessBehavior = newBehavior;
            _vmCallback?.NetworkBehaviorChanged(newBehavior);
        }

        #region Private methods

        private void ResetLoadStatus()
        {
            _loadStatus = CurrencyLoadStatus.NotLoaded;
        }

        private void NotifyDataLoadFinished(bool didLoad)
        {
            if (!didLoad)
            {
                _loadStatus = CurrencyLoadStatus.FailedToLoad;
            }

            _vmCallback?.CurrencyDataLoadFinished(didLoad);
        }

        private async Task<bool> TryFinishLoadFromCacheAsync()
        {
            var localSettings = ApplicationData.Current.LocalSettings;
            if (localSettings == null)
            {
                return false;
            }

            if (!localSettings.Values.ContainsKey(CurrencyDataLoaderConstants.CacheLangcodeKey)
                || (string)localSettings.Values[CurrencyDataLoaderConstants.CacheLangcodeKey] != _responseLanguage)
            {
                return false;
            }

            var localCacheFolder = ApplicationData.Current.LocalCacheFolder;
            if (localCacheFolder == null)
            {
                return false;
            }

            string staticDataResponse = await Utils.ReadFileFromFolder(localCacheFolder, CurrencyDataLoaderConstants.StaticDataFilename);
            string allRatiosResponse = await Utils.ReadFileFromFolder(localCacheFolder, CurrencyDataLoaderConstants.AllRatiosDataFilename);

            if (!TryParseWebResponses(staticDataResponse, allRatiosResponse, out var staticData, out var ratioMap))
            {
                return false;
            }

            _loadStatus = CurrencyLoadStatus.LoadedFromCache;
            await FinalizeUnits(staticData, ratioMap);

            return true;
        }

        private bool TryParseWebResponses(
            string staticDataJson,
            string allRatiosJson,
            out List<CurrencyStaticData> staticData,
            out Dictionary<string, CurrencyRatio> allRatiosData)
        {
            staticData = null;
            allRatiosData = null;

            if (!TryParseStaticData(staticDataJson, out staticData))
            {
                return false;
            }

            if (!TryParseAllRatiosData(allRatiosJson, out allRatiosData))
            {
                return false;
            }

            return true;
        }

        private bool TryParseStaticData(string rawJson, out List<CurrencyStaticData> staticData)
        {
            staticData = new List<CurrencyStaticData>();

            if (!JsonArray.TryParse(rawJson, out JsonArray data))
            {
                return false;
            }

            for (uint i = 0; i < data.Count; i++)
            {
                try
                {
                    JsonObject obj = data[(int)i].GetObject();

                    string countryCode = obj.GetNamedString(StaticDataProperties[0]);
                    string countryName = obj.GetNamedString(StaticDataProperties[1]);
                    string currencyCode = obj.GetNamedString(StaticDataProperties[2]);
                    string currencyName = obj.GetNamedString(StaticDataProperties[3]);
                    string currencySymbol = obj.GetNamedString(StaticDataProperties[4]);

                    staticData.Add(new CurrencyStaticData
                    {
                        CountryCode = countryCode,
                        CountryName = countryName,
                        CurrencyCode = currencyCode,
                        CurrencyName = currencyName,
                        CurrencySymbol = currencySymbol
                    });
                }
                catch (InvalidOperationException)
                {
                    continue;
                }
            }

            // Sort by country name using locale-aware comparison.
            staticData.Sort((a, b) => string.Compare(a.CountryName, b.CountryName, StringComparison.CurrentCulture));

            return true;
        }

        private bool TryParseAllRatiosData(string rawJson, out Dictionary<string, CurrencyRatio> allRatios)
        {
            allRatios = new Dictionary<string, CurrencyRatio>();

            if (!JsonArray.TryParse(rawJson, out JsonArray data))
            {
                return false;
            }

            string sourceCurrencyCode = DefaultCurrencyCode;

            for (uint i = 0; i < data.Count; i++)
            {
                try
                {
                    JsonObject obj = data[(int)i].GetObject();

                    double relativeRatio = obj.GetNamedNumber(RatioKey);
                    string targetCurrencyCode = obj.GetNamedString(CurrencyCodeKey);

                    allRatios[targetCurrencyCode] = new CurrencyRatio
                    {
                        Ratio = relativeRatio,
                        SourceCurrencyCode = sourceCurrencyCode,
                        TargetCurrencyCode = targetCurrencyCode
                    };
                }
                catch (InvalidOperationException)
                {
                    continue;
                }
            }

            return true;
        }

        private async Task FinalizeUnits(List<CurrencyStaticData> staticData, Dictionary<string, CurrencyRatio> ratioMap)
        {
            // idToUnit maps unit id -> (CurrencyUnit, baseRatio)
            var idToUnit = new Dictionary<int, (CurrencyUnit Unit, double BaseRatio)>();

            var defaultCurrencies = await GetDefaultFromToCurrency();
            string fromCurrency = defaultCurrencies.From;
            string toCurrency = defaultCurrencies.To;

            lock (_currencyUnitsMutex)
            {
                int i = 1;
                _currencyUnits.Clear();
                _currencyMetadata.Clear();
                bool isConversionSourceSet = false;
                bool isConversionTargetSet = false;

                foreach (var currencyUnit in staticData)
                {
                    if (ratioMap.TryGetValue(currencyUnit.CurrencyCode, out var currRatio) && currRatio.Ratio > 0)
                    {
                        int id = UnitEnd + i;

                        bool isConversionSource = (fromCurrency == currencyUnit.CurrencyCode);
                        isConversionSourceSet = isConversionSourceSet || isConversionSource;

                        bool isConversionTarget = (toCurrency == currencyUnit.CurrencyCode);
                        isConversionTargetSet = isConversionTargetSet || isConversionTarget;

                        var unit = new CurrencyUnit
                        {
                            Id = id,
                            Name = currencyUnit.CurrencyName,
                            CountryName = currencyUnit.CountryName,
                            Abbreviation = currencyUnit.CurrencyCode,
                            IsRtlLanguage = _isRtlLanguage,
                            IsConversionSource = isConversionSource,
                            IsConversionTarget = isConversionTarget
                        };

                        _currencyUnits.Add(unit);
                        _currencyMetadata[id] = new CurrencyUnitMetadata(currencyUnit.CurrencySymbol);
                        idToUnit[id] = (unit, currRatio.Ratio);
                        i++;
                    }
                }

                if (!isConversionSourceSet || !isConversionTargetSet)
                {
                    GuaranteeSelectedUnits();
                    defaultCurrencies = (DefaultFromCurrency, DefaultToCurrency);
                }

                _currencyRatioMap.Clear();
                foreach (var unit in _currencyUnits)
                {
                    var conversions = new Dictionary<int, ConversionData>();
                    double unitFactor = idToUnit[unit.Id].BaseRatio;

                    foreach (var kvp in idToUnit)
                    {
                        int targetId = kvp.Key;
                        double conversionRatio = kvp.Value.BaseRatio;
                        Debug.Assert(unitFactor > 0, "divide by zero assert");
                        conversions[targetId] = new ConversionData(conversionRatio / unitFactor);
                    }

                    _currencyRatioMap[unit.Id] = conversions;
                }
            }

            SaveSelectedUnitsToLocalSettings(defaultCurrencies);
        }

        private void GuaranteeSelectedUnits()
        {
            bool isConversionSourceSet = false;
            bool isConversionTargetSet = false;

            foreach (var unit in _currencyUnits)
            {
                unit.IsConversionSource = false;
                unit.IsConversionTarget = false;

                if (!isConversionSourceSet && unit.Abbreviation == DefaultFromCurrency)
                {
                    unit.IsConversionSource = true;
                    isConversionSourceSet = true;
                }
                if (!isConversionTargetSet && unit.Abbreviation == DefaultToCurrency)
                {
                    unit.IsConversionTarget = true;
                    isConversionTargetSet = true;
                }
            }

            if (_currencyUnits.Count > 0)
            {
                if (!isConversionSourceSet)
                {
                    _currencyUnits[0].IsConversionSource = true;
                }
                if (!isConversionTargetSet)
                {
                    _currencyUnits[0].IsConversionTarget = true;
                }
            }
        }

        private void SaveLangCodeAndTimestamp()
        {
            var localSettings = ApplicationData.Current.LocalSettings;
            if (localSettings == null)
            {
                return;
            }

            localSettings.Values[CurrencyDataLoaderConstants.CacheTimestampKey] = _cacheTimestamp;
            localSettings.Values[CurrencyDataLoaderConstants.CacheLangcodeKey] = _responseLanguage;
        }

        private void UpdateDisplayedTimestamp()
        {
            if (_vmCallback != null)
            {
                string timestamp = GetCurrencyTimestamp();
                bool isWeekOld = Utils.IsDateTimeOlderThan(_cacheTimestamp, CurrencyDataLoaderConstants.WeekDuration);
                _vmCallback.CurrencyTimestampUpdated(timestamp, isWeekOld);
            }
        }

        private void RegisterForNetworkBehaviorChanges()
        {
            _networkManager.NetworkBehaviorChanged -= OnNetworkBehaviorChangedHandler;
            _networkManager.NetworkBehaviorChanged += OnNetworkBehaviorChangedHandler;
            OnNetworkBehaviorChanged(NetworkManager.GetNetworkAccessBehavior());
        }

        private void OnNetworkBehaviorChangedHandler(NetworkAccessBehavior newBehavior)
        {
            OnNetworkBehaviorChanged(newBehavior);
        }

        private async Task<(string From, string To)> GetDefaultFromToCurrency()
        {
            string fromCurrency = DefaultFromCurrency;
            string toCurrency = DefaultToCurrency;

            // First, check if we previously stored the last used currencies.
            if (TryGetLastUsedCurrenciesFromLocalSettings(out string savedFrom, out string savedTo))
            {
                return (savedFrom, savedTo);
            }

            try
            {
                // Second, see if the current locale has preset defaults in DefaultFromToCurrency.json.
                var fileUri = new Uri(DefaultFromToCurrencyFileUri);
                StorageFile defaultFromToCurrencyFile = await StorageFile.GetFileFromApplicationUriAsync(fileUri);
                if (defaultFromToCurrencyFile != null)
                {
                    string fileContents = await FileIO.ReadTextAsync(defaultFromToCurrencyFile);
                    JsonObject fromToObject = JsonObject.Parse(fileContents);

                    if (fromToObject.ContainsKey(_responseLanguage))
                    {
                        JsonObject regionalDefaults = fromToObject.GetNamedObject(_responseLanguage);
                        string selectedFrom = regionalDefaults.GetNamedString(FromKey);
                        string selectedTo = regionalDefaults.GetNamedString(ToKey);
                        fromCurrency = selectedFrom;
                        toCurrency = selectedTo;
                    }
                }
            }
            catch
            {
            }

            return (fromCurrency, toCurrency);
        }

        private bool TryGetLastUsedCurrenciesFromLocalSettings(out string fromCurrency, out string toCurrency)
        {
            fromCurrency = null;
            toCurrency = null;

            string fromKey = UnitConverterResourceKeys.CurrencyUnitFromKey;
            string toKey = UnitConverterResourceKeys.CurrencyUnitToKey;

            var localSettings = ApplicationData.Current.LocalSettings;
            if (localSettings?.Values != null)
            {
                if (localSettings.Values.ContainsKey(fromKey) && localSettings.Values.ContainsKey(toKey))
                {
                    fromCurrency = (string)localSettings.Values[fromKey];
                    toCurrency = (string)localSettings.Values[toKey];
                    return true;
                }
            }

            return false;
        }

        private void SaveSelectedUnitsToLocalSettings((string From, string To) selectedUnits)
        {
            string fromKey = UnitConverterResourceKeys.CurrencyUnitFromKey;
            string toKey = UnitConverterResourceKeys.CurrencyUnitToKey;

            var localSettings = ApplicationData.Current.LocalSettings;
            if (localSettings?.Values != null)
            {
                localSettings.Values[fromKey] = selectedUnits.From;
                localSettings.Values[toKey] = selectedUnits.To;
            }
        }

        /// <summary>
        /// Parses a BCP 47 language tag and returns a lang-region code.
        /// </summary>
        private static string ParseLanguageCode(string bcp47)
        {
            if (string.IsNullOrEmpty(bcp47))
            {
                return null;
            }

            var segments = new List<string>();
            string current = string.Empty;

            foreach (char ch in bcp47)
            {
                if (segments.Count >= 3)
                {
                    break;
                }

                if (char.IsLetter(ch))
                {
                    current += ch;
                }
                else if (ch == '-')
                {
                    segments.Add(current);
                    current = string.Empty;
                }
                else
                {
                    return null;
                }
            }

            if (current.Length > 0)
            {
                segments.Add(current);
            }

            switch (segments.Count)
            {
                case 1:
                    return segments[0];
                case 2:
                    if (segments[1].Length == 2)
                    {
                        return segments[0] + "-" + segments[1];
                    }
                    return segments[0];
                case 3:
                    if (segments[1].Length == 2)
                    {
                        return segments[0] + "-" + segments[1];
                    }
                    else if (segments[2].Length == 2)
                    {
                        return segments[0] + "-" + segments[2];
                    }
                    return segments[0];
                default:
                    return null;
            }
        }

        #endregion
    }

    #region Supporting types

    public class CurrencyUnit
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string CountryName { get; set; }
        public string Abbreviation { get; set; }
        public bool IsRtlLanguage { get; set; }
        public bool IsConversionSource { get; set; }
        public bool IsConversionTarget { get; set; }
    }

    public class CurrencyUnitMetadata
    {
        public string Symbol { get; set; }

        public CurrencyUnitMetadata(string symbol)
        {
            Symbol = symbol;
        }
    }

    public struct CurrencyStaticData
    {
        public string CountryCode;
        public string CountryName;
        public string CurrencyCode;
        public string CurrencyName;
        public string CurrencySymbol;
    }

    public struct CurrencyRatio
    {
        public double Ratio;
        public string SourceCurrencyCode;
        public string TargetCurrencyCode;
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
