// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// Nullable analysis is disabled here: this file is a verbatim port of
// non-nullable-annotated upstream code; behavior is covered by tests.
#nullable disable
#pragma warning disable CS8632 // annotations kept for readers; analysis off (see above)

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.IO;
using System.Text.Json;
using Calculator.Linux.Common;
using Calculator.Linux.Services;

namespace Calculator.Linux.DataLoaders
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

    public sealed class CurrencyDataLoader : ICurrencySource
    {
        private const int UnitEnd = 168;
        private const string DefaultCurrencyCode = "USD";
        private const string DefaultFromCurrency = DefaultCurrencyCode;
        private const string DefaultToCurrency = "EUR";
        private const string DefaultFromToCurrencyFileName = "DefaultFromToCurrency.json";
        private const string FromKey = "from";
        private const string ToKey = "to";

        private const int FormatterRateFractionPadding = 2;
        private const int FormatterRateMinDecimals = 4;
        private const int FormatterRateMinSignificantDecimals = 4;

        private static readonly string[] StaticDataProperties = { "CountryCode", "CountryName", "CurrencyCode", "CurrencyName", "CurrencySymbol" };
        private const string RatioKey = "Rt";
        private const string CurrencyCodeKey = "An";

        private readonly FrankfurterCurrencyClient _client;
        private readonly LinuxSettings _settings;
        private readonly LinuxFileCache _cache;
        private readonly NetworkManager _networkManager;
        private NetworkBehaviorChangedHandler? _networkHandler;
        private readonly string _responseLanguage;
        private readonly bool _isRtlLanguage;
        private readonly object _currencyUnitsMutex = new object();

        // Initial loads and explicit refreshes replace the same currency data.
        private readonly SemaphoreSlim _loadGate = new SemaphoreSlim(1, 1);
        public Func<Task>? LoadGateEnteredForTest { get; set; }
        public Action? LoadGateExitedForTest { get; set; }

        private List<CurrencyUnit> _currencyUnits;
        private Dictionary<int, Dictionary<int, ConversionData>> _currencyRatioMap;
        private Dictionary<int, CurrencyUnitMetadata> _currencyMetadata;

        private IViewModelCurrencyCallback _vmCallback;

        private LinuxNumberFormatter _ratioFormatter;
        private string _ratioFormat;
        private string _timestampFormat;
        private DateTimeOffset _cacheTimestamp;

        // Volatile int (not enum: Volatile<T> is reference-typed): release/acquire
        // pairing so LoadFinished() polling never observes a stale flag while
        // units/timestamps published before it are still in flight.
        private volatile int _loadStatusRaw;

        private NetworkAccessBehavior _networkAccessBehavior;
        private bool _meteredOverrideSet;

        public CurrencyDataLoader(
            string? overrideLanguage = null,
            LinuxSettings? settings = null,
            LinuxFileCache? cache = null,
            FrankfurterCurrencyClient? client = null)
        {
            _settings = settings ?? new LinuxSettings();
            _cache = cache ?? new LinuxFileCache();
            _client = client ?? new FrankfurterCurrencyClient();
            _networkManager = new NetworkManager();
            _loadStatusRaw = (int)CurrencyLoadStatus.NotLoaded;
            _responseLanguage = "en-US";
            _ratioFormat = string.Empty;
            _timestampFormat = string.Empty;
            _networkAccessBehavior = NetworkAccessBehavior.Normal;
            _meteredOverrideSet = false;

            if (!string.IsNullOrEmpty(overrideLanguage))
            {
                Debug.Assert(overrideLanguage.Length > 0, "overrideLanguage shall not be empty.");
                _responseLanguage = overrideLanguage;
            }
            else
            {
                string parsed = ParseLanguageCode(CultureInfo.CurrentUICulture.Name);
                if (parsed != null)
                {
                    _responseLanguage = parsed;
                }
            }

            _client.Initialize(DefaultCurrencyCode, _responseLanguage);

            _isRtlLanguage = LinuxLocalization.Current.IsRightToLeft;

            _ratioFormatter = new LinuxNumberFormatter(LinuxLocalization.Current)
            {
                IsGrouped = true,
                IsDecimalPointAlwaysDisplayed = true,
                FractionDigits = FormatterRateFractionPadding,
            };

            var resProvider = AppResources.Instance;
            _ratioFormat = resProvider.GetResourceString("CurrencyFromToRatioFormat");
            _timestampFormat = resProvider.GetResourceString("CurrencyTimestampFormat");

            _currencyUnits = new List<CurrencyUnit>();
            _currencyRatioMap = new Dictionary<int, Dictionary<int, ConversionData>>();
            _currencyMetadata = new Dictionary<int, CurrencyUnitMetadata>();
        }

        public bool LoadFinished() => (CurrencyLoadStatus)_loadStatusRaw != CurrencyLoadStatus.NotLoaded;
        public bool LoadedFromCache() => (CurrencyLoadStatus)_loadStatusRaw == CurrencyLoadStatus.LoadedFromCache;
        public bool LoadedFromWeb() => (CurrencyLoadStatus)_loadStatusRaw == CurrencyLoadStatus.LoadedFromWeb;

        public async void LoadData()
        {
            bool notified = false;
            await _loadGate.WaitAsync().ConfigureAwait(false);
            try
            {
                if (LoadGateEnteredForTest != null)
                {
                    await LoadGateEnteredForTest().ConfigureAwait(false);
                }

                if (LoadFinished())
                {
                    return;
                }

                RegisterForNetworkBehaviorChanges();

                bool didLoad = false;

                didLoad = await TryLoadDataFromCacheAsync();
                if (!didLoad)
                {
                    didLoad = await TryLoadDataFromWebAsync();
                }

                // Timestamp formatting must not suppress load completion.
                try
                {
                    UpdateDisplayedTimestamp();
                }
                catch (Exception ex)
                {
                    LinuxTraceLogger.Instance.LogPlatformException(
                        ViewMode.Currency, nameof(UpdateDisplayedTimestamp), ex);
                }

                // Do not report a second completion if the callback itself throws.
                notified = true;
                NotifyDataLoadFinished(didLoad);
            }
            catch (Exception ex)
            {
                LinuxTraceLogger.Instance.LogPlatformException(
                    ViewMode.Currency, nameof(LoadData), ex);
                if (!notified)
                {
                    try
                    {
                        NotifyDataLoadFinished(false);
                    }
                    catch (Exception notifyEx)
                    {
                        LinuxTraceLogger.Instance.LogPlatformException(
                            ViewMode.Currency, nameof(NotifyDataLoadFinished), notifyEx);
                    }
                }
            }
            finally
            {
                try
                {
                    LoadGateExitedForTest?.Invoke();
                }
                finally
                {
                    _loadGate.Release();
                }
            }
        }

        public List<CategoryData> GetOrderedCategories()
        {
            // This function should not be called.
            // The model will use the categories from UnitConverterDataLoader.
            return new List<CategoryData>();
        }

        public List<CurrencyUnit> GetOrderedUnits()
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
                double ratio;
                string unit1Abbreviation = string.Empty;
                string unit1Name = string.Empty;
                string unit2Abbreviation = string.Empty;
                string unit2Name = string.Empty;

                lock (_currencyUnitsMutex)
                {
                    if (!_currencyRatioMap.TryGetValue(unit1Id, out var ratioMap)
                        || !ratioMap.TryGetValue(unit2Id, out var convData))
                    {
                        return (string.Empty, string.Empty);
                    }

                    ratio = convData.Ratio;
                    foreach (var unit in _currencyUnits)
                    {
                        if (unit.Id == unit1Id)
                        {
                            unit1Abbreviation = unit.Abbreviation;
                            unit1Name = ComposeAccessibleName(unit);
                        }
                        if (unit.Id == unit2Id)
                        {
                            unit2Abbreviation = unit.Abbreviation;
                            unit2Name = ComposeAccessibleName(unit);
                        }
                    }
                }

                double rounded = RoundCurrencyRatio(ratio);
                var locSettings = LinuxLocalization.Current;
                string digitSymbol = locSettings.GetDigitSymbolFromEnUsDigit('1').ToString();
                string roundedFormat = _ratioFormatter.Format(rounded);
                string ratioString = LocalizationStringUtil.GetLocalizedString(
                    _ratioFormat, digitSymbol, unit1Abbreviation, roundedFormat, unit2Abbreviation);
                string accessibleRatioString = LocalizationStringUtil.GetLocalizedString(
                    _ratioFormat, digitSymbol, unit1Name, roundedFormat, unit2Name);

                return (ratioString, accessibleRatioString);
            }
            catch
            {
            }

            return (string.Empty, string.Empty);
        }

        /// <summary>
        /// Screen readers get the country and currency names rather than the abbreviation, composed
        /// the same way the unit list composes them so the two stay consistent.
        /// </summary>
        private static string ComposeAccessibleName(CurrencyUnit unit)
        {
            string firstName = unit.IsRtlLanguage ? unit.Name : unit.CountryName;
            string secondName = unit.IsRtlLanguage ? unit.CountryName : unit.Name;
            return firstName + " " + secondName;
        }

        public string GetCurrencyTimestamp()
        {
            if (_cacheTimestamp != default)
            {
                CultureInfo culture = CultureForResponseLanguage();
                string date = _cacheTimestamp.ToString("d", culture);
                string time = _cacheTimestamp.ToString("t", culture);
                return LocalizationStringUtil.GetLocalizedString(_timestampFormat, date, time);
            }

            return string.Empty;
        }

        private CultureInfo CultureForResponseLanguage()
        {
            try
            {
                return new CultureInfo(_responseLanguage);
            }
            catch (CultureNotFoundException)
            {
                return CultureInfo.CurrentCulture;
            }
        }

        public async Task<bool> TryLoadDataFromCacheAsync()
        {
            try
            {
                ResetLoadStatus();

                if (!_settings.Contains(CurrencyDataLoaderConstants.CacheTimestampKey))
                {
                    return false;
                }

                bool loadComplete = false;
                _cacheTimestamp = _settings.Get<DateTimeOffset>(CurrencyDataLoaderConstants.CacheTimestampKey);

                if (CalcUtils.IsDateTimeOlderThan(_cacheTimestamp, CurrencyDataLoaderConstants.DayDuration)
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
            catch (Exception ex)
            {
                LinuxTraceLogger.Instance.LogPlatformException(
                    ViewMode.Currency, nameof(TryLoadDataFromCacheAsync), ex);
                return false;
            }
        }

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

                string? staticDataResponse = await _client.GetCurrencyMetadataAsync();
                string? allRatiosResponse = await _client.GetCurrencyRatiosAsync();
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
                _cacheTimestamp = CalcUtils.GetUniversalSystemTime();

                try
                {
                    await _cache.WriteTextAsync(CurrencyDataLoaderConstants.StaticDataFilename, staticDataResponse);
                    await _cache.WriteTextAsync(CurrencyDataLoaderConstants.AllRatiosDataFilename, allRatiosResponse);
                    SaveLangCodeAndTimestamp();
                }
                catch
                {
                    // If we fail to save to cache it's okay, we should still continue.
                }

                await FinalizeUnits(staticData, ratioMap);
                _loadStatusRaw = (int)CurrencyLoadStatus.LoadedFromWeb;

                return true;
            }
            catch (Exception ex)
            {
                LinuxTraceLogger.Instance.LogPlatformException(
                    ViewMode.Currency, nameof(TryLoadDataFromWebAsync), ex);
                return false;
            }
        }

        public async Task<bool> TryLoadDataFromWebOverrideAsync()
        {
            // Serialize with the initial load.
            await _loadGate.WaitAsync().ConfigureAwait(false);
            try
            {
                if (LoadGateEnteredForTest != null)
                {
                    await LoadGateEnteredForTest().ConfigureAwait(false);
                }

                _meteredOverrideSet = true;
                bool didLoad = await TryLoadDataFromWebAsync();
                if (!didLoad)
                {
                    _loadStatusRaw = (int)CurrencyLoadStatus.FailedToLoad;
                }

                // Timestamp formatting must not suppress the caller's completion path.
                try
                {
                    UpdateDisplayedTimestamp();
                }
                catch (Exception ex)
                {
                    LinuxTraceLogger.Instance.LogPlatformException(
                        ViewMode.Currency, nameof(UpdateDisplayedTimestamp), ex);
                }

                return didLoad;
            }
            finally
            {
                try
                {
                    LoadGateExitedForTest?.Invoke();
                }
                finally
                {
                    _loadGate.Release();
                }
            }
        }

        public void OnNetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
        {
            _networkAccessBehavior = newBehavior;
            _vmCallback?.NetworkBehaviorChanged(newBehavior);
        }

        private void ResetLoadStatus()
        {
            _loadStatusRaw = (int)CurrencyLoadStatus.NotLoaded;
        }

        private void NotifyDataLoadFinished(bool didLoad)
        {
            if (!didLoad)
            {
                _loadStatusRaw = (int)CurrencyLoadStatus.FailedToLoad;
            }

            _vmCallback?.CurrencyDataLoadFinished(didLoad);
        }

        private async Task<bool> TryFinishLoadFromCacheAsync()
        {
            if (!_settings.Contains(CurrencyDataLoaderConstants.CacheLangcodeKey)
                || _settings.Get<string>(CurrencyDataLoaderConstants.CacheLangcodeKey) != _responseLanguage)
            {
                return false;
            }

            string? staticDataResponse = await _cache.ReadTextAsync(CurrencyDataLoaderConstants.StaticDataFilename);
            string? allRatiosResponse = await _cache.ReadTextAsync(CurrencyDataLoaderConstants.AllRatiosDataFilename);

            if (!TryParseWebResponses(staticDataResponse, allRatiosResponse, out var staticData, out var ratioMap))
            {
                return false;
            }

            await FinalizeUnits(staticData, ratioMap);
            _loadStatusRaw = (int)CurrencyLoadStatus.LoadedFromCache;

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

        private bool TryParseStaticData(string? rawJson, out List<CurrencyStaticData> staticData)
        {
            staticData = new List<CurrencyStaticData>();

            try
            {
                if (string.IsNullOrEmpty(rawJson))
                {
                    return false;
                }
                using JsonDocument data = JsonDocument.Parse(rawJson);
                if (data.RootElement.ValueKind != JsonValueKind.Array)
                {
                    return false;
                }

                foreach (JsonElement element in data.RootElement.EnumerateArray())
                {
                    if (element.ValueKind != JsonValueKind.Object)
                    {
                        continue;
                    }
                    if (!TryGetString(element, StaticDataProperties[0], out string countryCode)
                        || !TryGetString(element, StaticDataProperties[1], out string countryName)
                        || !TryGetString(element, StaticDataProperties[2], out string currencyCode)
                        || !TryGetString(element, StaticDataProperties[3], out string currencyName)
                        || !TryGetString(element, StaticDataProperties[4], out string currencySymbol))
                    {
                        continue;
                    }

                    staticData.Add(new CurrencyStaticData
                    {
                        CountryCode = countryCode,
                        CountryName = countryName,
                        CurrencyCode = currencyCode,
                        CurrencyName = currencyName,
                        CurrencySymbol = currencySymbol
                    });
                }
            }
            catch (JsonException)
            {
                return false;
            }

            staticData.Sort((a, b) =>
                string.Compare(a.CountryName, b.CountryName, StringComparison.CurrentCulture));

            return true;
        }

        private static bool TryGetString(JsonElement element, string name, out string value)
        {
            value = string.Empty;
            if (!element.TryGetProperty(name, out JsonElement prop)
                || prop.ValueKind != JsonValueKind.String)
            {
                return false;
            }
            string? text = prop.GetString();
            if (text == null)
            {
                return false;
            }
            value = text;
            return true;
        }

        private bool TryParseAllRatiosData(string? rawJson, out Dictionary<string, CurrencyRatio> allRatios)
        {
            allRatios = new Dictionary<string, CurrencyRatio>();

            try
            {
                if (string.IsNullOrEmpty(rawJson))
                {
                    return false;
                }
                using JsonDocument data = JsonDocument.Parse(rawJson);
                if (data.RootElement.ValueKind != JsonValueKind.Array)
                {
                    return false;
                }

                string sourceCurrencyCode = DefaultCurrencyCode;

                foreach (JsonElement element in data.RootElement.EnumerateArray())
                {
                    if (element.ValueKind != JsonValueKind.Object)
                    {
                        continue;
                    }
                    if (!element.TryGetProperty(RatioKey, out JsonElement ratioProp)
                        || ratioProp.ValueKind != JsonValueKind.Number
                        || !ratioProp.TryGetDouble(out double relativeRatio))
                    {
                        continue;
                    }
                    if (!TryGetString(element, CurrencyCodeKey, out string targetCurrencyCode))
                    {
                        continue;
                    }

                    allRatios[targetCurrencyCode] = new CurrencyRatio
                    {
                        Ratio = relativeRatio,
                        SourceCurrencyCode = sourceCurrencyCode,
                        TargetCurrencyCode = targetCurrencyCode
                    };
                }
            }
            catch (JsonException)
            {
                return false;
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
            _settings.Set(CurrencyDataLoaderConstants.CacheTimestampKey, _cacheTimestamp);
            _settings.Set(CurrencyDataLoaderConstants.CacheLangcodeKey, _responseLanguage);
        }

        private void UpdateDisplayedTimestamp()
        {
            if (_vmCallback != null)
            {
                string timestamp = GetCurrencyTimestamp();
                bool isWeekOld = CalcUtils.IsDateTimeOlderThan(_cacheTimestamp, CurrencyDataLoaderConstants.WeekDuration);
                _vmCallback.CurrencyTimestampUpdated(timestamp, isWeekOld);
            }
        }

        private void RegisterForNetworkBehaviorChanges()
        {
            if (_networkHandler != null)
            {
                _networkManager.NetworkBehaviorChanged -= _networkHandler;
            }
            _networkHandler = OnNetworkBehaviorChangedHandler;
            _networkManager.NetworkBehaviorChanged += _networkHandler;
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
                // Second, see if the current locale has preset defaults in DefaultFromToCurrency.json
                // (shipped next to the binaries; see Calculator.Linux.csproj content link).
                string path = Path.Combine(AppContext.BaseDirectory, "DefaultFromToCurrency.json");
                if (File.Exists(path))
                {
                    string fileContents = await File.ReadAllTextAsync(path);
                    using JsonDocument fromToObject = JsonDocument.Parse(fileContents);
                    if (fromToObject.RootElement.ValueKind == JsonValueKind.Object
                        && fromToObject.RootElement.TryGetProperty(_responseLanguage, out JsonElement regionalDefaults)
                        && regionalDefaults.ValueKind == JsonValueKind.Object
                        && TryGetString(regionalDefaults, FromKey, out string selectedFrom)
                        && TryGetString(regionalDefaults, ToKey, out string selectedTo))
                    {
                        fromCurrency = selectedFrom;
                        toCurrency = selectedTo;
                    }
                }
            }
            catch (Exception ex) when (ex is IOException or JsonException or UnauthorizedAccessException)
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

            if (_settings.Contains(fromKey) && _settings.Contains(toKey))
            {
                string? savedFrom = _settings.Get<string>(fromKey);
                string? savedTo = _settings.Get<string>(toKey);
                if (!string.IsNullOrEmpty(savedFrom) && !string.IsNullOrEmpty(savedTo))
                {
                    fromCurrency = savedFrom;
                    toCurrency = savedTo;
                    return true;
                }
            }

            return false;
        }

        private void SaveSelectedUnitsToLocalSettings((string From, string To) selectedUnits)
        {
            string fromKey = UnitConverterResourceKeys.CurrencyUnitFromKey;
            string toKey = UnitConverterResourceKeys.CurrencyUnitToKey;

            _settings.Set(fromKey, selectedUnits.From);
            _settings.Set(toKey, selectedUnits.To);
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

    }

}
