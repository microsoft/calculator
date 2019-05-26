// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Foundation.Collections;
using Windows.Globalization.DateTimeFormatting;
using Windows.Storage;
using Windows.System.UserProfile;
using Windows.UI.Core;
using CalculatorApp.Common;
using CalculatorApp.DataLoaders;
using UCM = UnitConversionManager;
using CurrencyRatioMap = System.Collections.Generic.Dictionary<string, UnitConversionManager.CurrencyRatio>;
using SelectedUnits = System.Collections.Generic.KeyValuePair<string, string>;
using CategorySelectionInitializer = System.Tuple<CalculatorApp.CalculatorList<UnitConversionManager.Unit>, UnitConversionManager.Unit, UnitConversionManager.Unit>;
using UnitToUnitToConversionDataMap = System.Collections.Generic.Dictionary<UnitConversionManager.Unit, System.Collections.Generic.Dictionary<UnitConversionManager.Unit, UnitConversionManager.ConversionData>>;
using CategoryToUnitVectorMap = System.Collections.Generic.Dictionary<UnitConversionManager.Category, CalculatorApp.CalculatorList<UnitConversionManager.Unit>>;
using System.Globalization;

namespace CalculatorApp.ViewModel
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
		public const string CurrencyUnitFromKey = CurrencyDataLoader.CURRENCY_UNIT_FROM_KEY;
		public const string CurrencyUnitToKey = CurrencyDataLoader.CURRENCY_UNIT_TO_KEY;
	}

	public static class CurrencyDataLoaderConstants
	{
		public const string CacheTimestampKey = CurrencyDataLoader.CACHE_TIMESTAMP_KEY;
		public const string CacheLangcodeKey = CurrencyDataLoader.CACHE_LANGCODE_KEY;
		public const string CacheDelimiter = CurrencyDataLoader.CACHE_DELIMITER;
		public const string StaticDataFilename = CurrencyDataLoader.STATIC_DATA_FILENAME;
		public const string AllRatiosDataFilename = CurrencyDataLoader.ALL_RATIOS_DATA_FILENAME;
		public const long DayDuration = CurrencyDataLoader.DAY_DURATION;
	}

	struct CurrencyUnitMetadata
	{
		public CurrencyUnitMetadata(string s)
		{
			symbol = s;
		}

		public string symbol;
	};

	public class CurrencyDataLoader : UCM.IConverterDataLoader, UCM.ICurrencyConverterDataLoader
	{
		//private:
		string m_responseLanguage;
		CalculatorApp.DataLoaders.ICurrencyHttpClient m_client;

		bool m_isRtlLanguage;

		//Mutex m_currencyUnitsMutex;
		object m_currencyUnitsMutex = new object();
		CalculatorList<UCM.Unit> m_currencyUnits = new CalculatorList<UCM.Unit>();
		UnitToUnitToConversionDataMap m_currencyRatioMap = new UnitToUnitToConversionDataMap(EqualityComparer<UCM.Unit>.Default);
		Dictionary<UCM.Unit, CurrencyUnitMetadata> m_currencyMetadata = new Dictionary<UCM.Unit, CurrencyUnitMetadata>(EqualityComparer<UCM.Unit>.Default);

		UCM.IViewModelCurrencyCallback m_vmCallback;

		Windows.Globalization.NumberFormatting.DecimalFormatter m_ratioFormatter;
		string m_ratioFormat;
		DateTime m_cacheTimestamp;
		string m_timestampFormat;

		CurrencyLoadStatus m_loadStatus;

		CalculatorApp.NetworkManager m_networkManager;

		CalculatorApp.NetworkAccessBehavior m_networkAccessBehavior;

		//Windows.Foundation.EventRegistrationToken m_networkBehaviorToken;
		bool m_meteredOverrideSet;

		internal const string CURRENCY_UNIT_FROM_KEY = "CURRENCY_UNIT_FROM_KEY";
		internal const string CURRENCY_UNIT_TO_KEY = "CURRENCY_UNIT_TO_KEY";

		// Calculate number of 100-nanosecond intervals-per-day
		// (1 interval/100 nanosecond)(100 nanosecond/1e-7 s)(60 s/1 min)(60 min/1 hr)(24 hr/1 day) = (interval/day)
		internal const long DAY_DURATION = 1L * 60 * 60 * 24 * 10000000;
		internal const long WEEK_DURATION = DAY_DURATION * 7;

		static int FORMATTER_DIGIT_COUNT = 4;

		internal const string CACHE_TIMESTAMP_KEY = "CURRENCY_CONVERTER_TIMESTAMP";
		internal const string CACHE_LANGCODE_KEY = "CURRENCY_CONVERTER_LANGCODE";
		internal const string CACHE_DELIMITER = "%";

		internal const string STATIC_DATA_FILENAME = "CURRENCY_CONVERTER_STATIC_DATA.txt";

		private static string[] STATIC_DATA_PROPERTIES =
		{
			"CountryCode",
			"CountryName",
			"CurrencyCode",
			"CurrencyName",
			"CurrencySymbol"
		};

		internal const string ALL_RATIOS_DATA_FILENAME = "CURRENCY_CONVERTER_ALL_RATIOS_DATA.txt";
		internal const string RATIO_KEY = "Rt";
		internal const string CURRENCY_CODE_KEY = "An";
		internal static string[] ALL_RATIOS_DATA_PROPERTIES = {RATIO_KEY, CURRENCY_CODE_KEY};

		static string DEFAULT_FROM_TO_CURRENCY_FILE_URI = "ms-appx:///DataLoaders/DefaultFromToCurrency.json";
		static string FROM_KEY = "from";
		static string TO_KEY = "to";

		private static string DefaultCurrencyCode = LocalizationService.DefaultCurrencyCode;

		// Fallback default values.
		static string DEFAULT_FROM_CURRENCY = DefaultCurrencyCode;
		static string DEFAULT_TO_CURRENCY = "EUR";

		public CurrencyDataLoader(ICurrencyHttpClient client)
		{
			this.m_client = client;
			;
			this.m_loadStatus = CurrencyLoadStatus.NotLoaded;
			this.m_responseLanguage = "en-US";
			this.m_ratioFormat = "";
			this.m_timestampFormat = "";
			this.m_networkManager = new NetworkManager();
			this.m_meteredOverrideSet = false;


			// TODO UNO
			//if (GlobalizationPreferences.Languages.Count > 0)
			//{
			//	m_responseLanguage = GlobalizationPreferences.Languages[0];
			//}
			m_responseLanguage = CultureInfo.CurrentCulture.IetfLanguageTag;

			if (m_client != null)
			{
				m_client.SetSourceCurrencyCode(DefaultCurrencyCode);
				m_client.SetResponseLanguage(m_responseLanguage);
			}

			// TODO UNO
			//if (CoreWindow.GetForCurrentThread() != null)
			//{
			//	// Must have a CoreWindow to access the resource context.
			//	m_isRtlLanguage = LocalizationService.GetInstance().IsRtlLayout();
			//}
			m_isRtlLanguage = true;

			m_ratioFormatter = LocalizationService.GetRegionalSettingsAwareDecimalFormatter();
			m_ratioFormatter.IsGrouped = true;
			m_ratioFormatter.IsDecimalPointAlwaysDisplayed = true;
			m_ratioFormatter.FractionDigits = FORMATTER_DIGIT_COUNT;

			m_ratioFormat = AppResourceProvider.GetInstance().GetResourceString("CurrencyFromToRatioFormat");
			m_timestampFormat = AppResourceProvider.GetInstance().GetResourceString("CurrencyTimestampFormat");
		}

		~CurrencyDataLoader()
		{
			UnregisterForNetworkBehaviorChanges();
		}

		void UnregisterForNetworkBehaviorChanges()
		{
			m_networkManager.NetworkBehaviorChanged -= OnNetworkBehaviorChanged;
		}

		void RegisterForNetworkBehaviorChanges()
		{
			UnregisterForNetworkBehaviorChanges();

			m_networkManager.NetworkBehaviorChanged += new NetworkBehaviorChangedHandler(OnNetworkBehaviorChanged);

			OnNetworkBehaviorChanged(NetworkManager.GetNetworkAccessBehavior());
		}

		void OnNetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
		{
			m_networkAccessBehavior = newBehavior;
			if (m_vmCallback != null)
			{
				m_vmCallback.NetworkBehaviorChanged((int)(m_networkAccessBehavior));
			}
		}

		bool LoadFinished()
		{
			return m_loadStatus != CurrencyLoadStatus.NotLoaded;
		}

		bool LoadedFromCache()
		{
			return m_loadStatus == CurrencyLoadStatus.LoadedFromCache;
		}

		bool LoadedFromWeb()
		{
			return m_loadStatus == CurrencyLoadStatus.LoadedFromWeb;
		}

		void ResetLoadStatus()
		{
			m_loadStatus = CurrencyLoadStatus.NotLoaded;
		}

		public async void LoadData()
		{
			RegisterForNetworkBehaviorChanges();

			if (!LoadFinished())
			{
				var loadFunctions = new Func<Task<bool>>[]
				{
					TryLoadDataFromCacheAsync,
					TryLoadDataFromWebAsync
				};

				bool didLoad = false;
				foreach (var f in loadFunctions)
				{
					didLoad = await f();
					if (didLoad)
					{
						break;
					}
				}

				UpdateDisplayedTimestamp();
				NotifyDataLoadFinished(didLoad);
			}
		}

		public CalculatorList<UCM.Category> LoadOrderedCategories()
		{
			// This function should not be called
			// The model will use the categories from UnitConverterDataLoader
			return new CalculatorList<UCM.Category>();
		}

		public CalculatorList<UCM.Unit> LoadOrderedUnits(UCM.Category category)
		{
			lock (m_currencyUnitsMutex)
			{
				return m_currencyUnits;
			}
		}

		public Dictionary<UCM.Unit, UCM.ConversionData> LoadOrderedRatios(UCM.Unit unit)
		{
			lock (m_currencyUnitsMutex) ;
			{
				return m_currencyRatioMap[unit];
			}
		}

		public bool SupportsCategory(UCM.Category target)
		{
			int currencyId = NavCategory.Serialize(ViewMode.Currency);
			return target.id == currencyId;
		}

		public void SetViewModelCallback(UCM.IViewModelCurrencyCallback callback)
		{
			m_vmCallback = callback;
			OnNetworkBehaviorChanged(m_networkAccessBehavior);
		}

		public KeyValuePair<string, string> GetCurrencySymbols(UCM.Unit unit1, UCM.Unit unit2)
		{
			lock (m_currencyUnitsMutex)
			{

				string symbol1 = "";
				string symbol2 = "";

				if (m_currencyMetadata.TryGetValue(unit1, out var itr1) && m_currencyMetadata.TryGetValue(unit2, out var itr2))
				{
					symbol1 = itr1.symbol;
					symbol2 = itr2.symbol;
				}

				return new KeyValuePair<string, string>(symbol1, symbol2);
			}
		}

		public KeyValuePair<string, string> GetCurrencyRatioEquality(UCM.Unit unit1, UCM.Unit unit2)
		{
			try
			{
				if (m_currencyRatioMap.TryGetValue(unit1, out var ratioMap))
				{
					if (ratioMap.TryGetValue(unit2, out var iter2))
					{
						var ratio = iter2.ratio;

						// Round the ratio to FORMATTER_DIGIT_COUNT digits using int math.
						// Ex: to round 1.23456 to three digits, use
						//     ((int) 1.23456 * (10^3)) / (10^3)
						double scale = Math.Pow(10, FORMATTER_DIGIT_COUNT);
						double rounded = (int)(ratio * (int)(scale)) / scale;

						string digitSymbol = LocalizationSettings.GetInstance().GetDigitSymbolFromEnUsDigit('1').ToString();
						// TODO UNO: string roundedFormat = m_ratioFormatter.Format(rounded);
						string roundedFormat = rounded.ToString(CultureInfo.CurrentCulture);

						string ratioString = LocalizationStringUtil.GetLocalizedString(
							m_ratioFormat,
							digitSymbol,
							unit1.abbreviation,
							roundedFormat,
							unit2.abbreviation);

						string accessibleRatioString = LocalizationStringUtil.GetLocalizedString(
							m_ratioFormat,
							digitSymbol,
							unit1.accessibleName,
							roundedFormat,
							unit2.accessibleName);

						return new KeyValuePair<string, string>(ratioString, accessibleRatioString);
					}
				}
			}
			catch
			{
			}

			return new KeyValuePair<string, string>("", "");
		}

#pragma optimize("", off) // Turn off optimizations to work around DevDiv 393321
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
				m_cacheTimestamp = DateTime.Parse((string)localSettings.Values[CurrencyDataLoaderConstants.CacheTimestampKey]);
				if (Utils.IsDateTimeOlderThan(m_cacheTimestamp, DAY_DURATION) && m_networkAccessBehavior == NetworkAccessBehavior.Normal)
				{
					loadComplete = await TryLoadDataFromWebAsync();
				}

				if (!loadComplete)
				{
					loadComplete = await TryFinishLoadFromCacheAsync();
				}

				return loadComplete;
			}
			catch (Exception e)
			{
				TraceLogger.GetInstance().LogStandardException(e);
				return false;
			}
		}

		public async Task<bool> TryFinishLoadFromCacheAsync()
		{
			var localSettings = ApplicationData.Current.LocalSettings;
			if (localSettings == null)
			{
				return false;
			}

			if (!localSettings.Values.ContainsKey(CurrencyDataLoaderConstants.CacheLangcodeKey) || !((string)localSettings.Values[CurrencyDataLoaderConstants.CacheLangcodeKey]).Equals(m_responseLanguage))
			{
				return false;
			}

			StorageFolder localCacheFolder = ApplicationData.Current.LocalCacheFolder;
			if (localCacheFolder == null)
			{
				return false;
			}

			string staticDataResponse = await Utils.ReadFileFromFolder(localCacheFolder, CurrencyDataLoaderConstants.StaticDataFilename);
			string allRatiosResponse = await Utils.ReadFileFromFolder(localCacheFolder, CurrencyDataLoaderConstants.AllRatiosDataFilename);

			CalculatorList<UCM.CurrencyStaticData> staticData = new CalculatorList<UCM.CurrencyStaticData>();
			CurrencyRatioMap ratioMap = new CurrencyRatioMap();

			bool didParse = TryParseWebResponses(staticDataResponse, allRatiosResponse, staticData, ratioMap);
			if (!didParse)
			{
				return false;
			}

			m_loadStatus = CurrencyLoadStatus.LoadedFromCache;
			await FinalizeUnits(staticData, ratioMap);

			return true;
		}

		public async Task<bool> TryLoadDataFromWebAsync()
		{
			try
			{
				ResetLoadStatus();

				if (m_client == null)
				{
					return false;
				}

				if (m_networkAccessBehavior == NetworkAccessBehavior.Offline || (m_networkAccessBehavior == NetworkAccessBehavior.OptIn && !m_meteredOverrideSet))
				{
					return false;
				}

				String staticDataResponse = await m_client.GetCurrencyMetadata();
				String allRatiosResponse = await m_client.GetCurrencyRatios();
				if (staticDataResponse == null || allRatiosResponse == null)
				{
					return false;
				}

				CalculatorList<UCM.CurrencyStaticData> staticData = new CalculatorList<UCM.CurrencyStaticData>();
				CurrencyRatioMap ratioMap = new CurrencyRatioMap();

				bool didParse = TryParseWebResponses(staticDataResponse, allRatiosResponse, staticData, ratioMap);
				if (!didParse)
				{
					return false;
				}

				// Set the timestamp before saving it below.
				m_cacheTimestamp = Utils.GetUniversalSystemTime();

				try
				{
					CalculatorList<KeyValuePair<string, string>> cachedFiles = new CalculatorList<KeyValuePair<string, string>>
					{
						new KeyValuePair<string, string>(CurrencyDataLoaderConstants.StaticDataFilename, staticDataResponse),
						new KeyValuePair<string, string>(CurrencyDataLoaderConstants.AllRatiosDataFilename, allRatiosResponse)
					};

					StorageFolder localCacheFolder = ApplicationData.Current.LocalCacheFolder;
					foreach (var fileInfo in cachedFiles)
					{
						await Utils.WriteFileToFolder(localCacheFolder, fileInfo.Key, fileInfo.Value, CreationCollisionOption.ReplaceExisting);
					}

					SaveLangCodeAndTimestamp();
				}
				catch
				{
					// If we fail to save to cache it's okay, we should still continue.
				}

				m_loadStatus = CurrencyLoadStatus.LoadedFromWeb;
				await FinalizeUnits(staticData, ratioMap);

				return true;
			}
			catch (Exception e)
			{
				TraceLogger.GetInstance().LogStandardException(e);
				return false;
			}
		}

		public async Task<bool> TryLoadDataFromWebOverrideAsync()
		{
			m_meteredOverrideSet = true;
			bool didLoad = await TryLoadDataFromWebAsync();
			if (!didLoad)
			{
				m_loadStatus = CurrencyLoadStatus.FailedToLoad;
				TraceLogger.GetInstance().LogUserRequestedRefreshFailed();
			}

			return didLoad;
		}

		bool TryParseWebResponses(
			String staticDataJson,
			String allRatiosJson,
			CalculatorList<UCM.CurrencyStaticData> staticData,
			CurrencyRatioMap allRatiosData)
		{
			return TryParseStaticData(staticDataJson, staticData) && TryParseAllRatiosData(allRatiosJson, allRatiosData);
		}

		bool TryParseStaticData(String rawJson, CalculatorList<UCM.CurrencyStaticData> staticData)
		{
#if NETFX_CORE // TODO UNO
			JsonArray data = null;
			if (!JsonArray.TryParse(rawJson, out data))
			{
				return false;
			}

			string countryCode = "";
			string countryName = "";
			string currencyCode = "";
			string currencyName = "";
			string currencySymbol = "";

			string[] values = { countryCode, countryName, currencyCode, currencyName, currencySymbol };

			Debug.Assert(values.Length == STATIC_DATA_PROPERTIES.Length);
			staticData.Clear();
			for (int i = 0; i < data.Count; i++)
			{
				JsonObject obj = data[i].GetObject();

				countryCode = obj.GetNamedString(STATIC_DATA_PROPERTIES[0]);
				countryName = obj.GetNamedString(STATIC_DATA_PROPERTIES[1]);
				currencyCode = obj.GetNamedString(STATIC_DATA_PROPERTIES[2]);
				currencyName = obj.GetNamedString(STATIC_DATA_PROPERTIES[3]);
				currencySymbol = obj.GetNamedString(STATIC_DATA_PROPERTIES[4]);

				staticData.Add(new UCM.CurrencyStaticData(countryCode, countryName, currencyCode, currencyName, currencySymbol));
			}

			// TODO - MSFT 8533667: this sort will be replaced by a WinRT call to sort localized strings
			staticData.Sort((UCM.CurrencyStaticData unit1, UCM.CurrencyStaticData unit2) => { return unit1.countryName.CompareTo(unit2.countryName) < 0; });

			return true;
#else
			try
			{
				var items = Newtonsoft.Json.JsonConvert.DeserializeObject<UCM.CurrencyStaticData[]>(rawJson);
				staticData.Clear();
				foreach (var item in items)
				{
					staticData.Add(item);
				}
				staticData.Sort((UCM.CurrencyStaticData unit1, UCM.CurrencyStaticData unit2) => { return unit1.countryName.CompareTo(unit2.countryName) < 0; });

				return true;
			}
			catch (Exception)
			{
				return false;
			}
#endif
		}

		public class JsonCurrencyRatio
		{
			public double Rt { get; set; }

			public string An { get; set; }
		}

		public bool TryParseAllRatiosData(String rawJson, CurrencyRatioMap allRatios)
		{
#if NETFX_CORE // TODO UNO
			JsonArray data = null;
			if (!JsonArray.TryParse(rawJson, out data))
			{
				return false;
			}

			string sourceCurrencyCode = DefaultCurrencyCode;

			allRatios.Clear();
			for (int i = 0; i < data.Count; i++)
			{
				JsonObject obj = data[i].GetObject();

				// Rt is ratio, An is target currency ISO code.
				double relativeRatio = obj.GetNamedNumber(RATIO_KEY);
				string targetCurrencyCode = obj.GetNamedString(CURRENCY_CODE_KEY);

				allRatios.Add(targetCurrencyCode, new UCM.CurrencyRatio(relativeRatio, sourceCurrencyCode, targetCurrencyCode));
			}

			return true;
#else
			try
			{
				string sourceCurrencyCode = DefaultCurrencyCode;

				var items = Newtonsoft.Json.JsonConvert.DeserializeObject<JsonCurrencyRatio[]>(rawJson);
				allRatios.Clear();
				foreach (var item in items)
				{
					allRatios.Add(item.An, new UCM.CurrencyRatio(item.Rt, sourceCurrencyCode, item.An));
				}

				return true;
			}
			catch (Exception)
			{
				return false;
			}

#endif
		}

		// FinalizeUnits
		//
		// There are a few ways we can get the data needed for Currency Converter, including from cache or from web.
		// This function accepts the data from any source, and acts as a 'last-steps' for the converter to be ready.
		// This includes identifying which units will be selected and building the map of currency ratios.
		public async Task FinalizeUnits(CalculatorList<UCM.CurrencyStaticData> staticData, CurrencyRatioMap ratioMap)
		{
			Dictionary<int, KeyValuePair<UCM.Unit, double>> idToUnit = new Dictionary<int, KeyValuePair<UCM.Unit, double>>();

			SelectedUnits defaultCurrencies = await GetDefaultFromToCurrency();
			string fromCurrency = defaultCurrencies.Key;
			string toCurrency = defaultCurrencies.Value;

			lock (m_currencyUnitsMutex)
			{

				int i = 1;
				m_currencyUnits.Clear();
				m_currencyMetadata.Clear();
				bool isConversionSourceSet = false;
				bool isConversionTargetSet = false;
				foreach (UCM.CurrencyStaticData currencyUnit in staticData)
				{
					if (ratioMap.TryGetValue(currencyUnit.currencyCode, out var itr) && itr.ratio > 0)
					{
						int id = (int)(UnitConverterUnits.UnitEnd + i);

						bool isConversionSource = (fromCurrency == currencyUnit.currencyCode);
						isConversionSourceSet = isConversionSourceSet || isConversionSource;

						bool isConversionTarget = (toCurrency == currencyUnit.currencyCode);
						isConversionTargetSet = isConversionTargetSet || isConversionTarget;

						UCM.Unit unit = new UCM.Unit(
							id, // id
							currencyUnit.currencyName, // currencyName
							currencyUnit.countryName, // countryName
							currencyUnit.currencyCode, // abbreviation
							m_isRtlLanguage, // isRtlLanguage
							isConversionSource, // isConversionSource
							isConversionTarget // isConversionTarget
						);

						m_currencyUnits.PushBack(unit);
						m_currencyMetadata.Add(unit, new CurrencyUnitMetadata(currencyUnit.currencySymbol));
						idToUnit.Add(unit.id, new KeyValuePair<UCM.Unit, double>(unit, itr.ratio));
						i++;
					}
				}

				if (!isConversionSourceSet || !isConversionTargetSet)
				{
					GuaranteeSelectedUnits();
					defaultCurrencies = new SelectedUnits(DEFAULT_FROM_CURRENCY, DEFAULT_TO_CURRENCY);
				}

				m_currencyRatioMap.Clear();
				foreach (var unit in m_currencyUnits)
				{
					Dictionary<UCM.Unit, UCM.ConversionData> conversions = new Dictionary<UCM.Unit, UCM.ConversionData>(EqualityComparer<UCM.Unit>.Default);
					double unitFactor = idToUnit[unit.id].Value;
					foreach (var itr in idToUnit)
					{
						UCM.Unit targetUnit = (itr.Value).Key;
						double conversionRatio = (itr.Value).Value;
						UCM.ConversionData parsedData = new UCM.ConversionData(1.0, 0.0, false);
						Debug.Assert(unitFactor > 0); // divide by zero assert
						parsedData.ratio = conversionRatio / unitFactor;
						conversions.Add(targetUnit, parsedData);
					}

					m_currencyRatioMap.Add(unit, conversions);
				}
			} // unlocked m_currencyUnitsMutex

			SaveSelectedUnitsToLocalSettings(defaultCurrencies);
		}


		void GuaranteeSelectedUnits()
		{
			bool isConversionSourceSet = false;
			bool isConversionTargetSet = false;
			foreach (UCM.Unit unit in m_currencyUnits)
			{
				unit.isConversionSource = false;
				unit.isConversionTarget = false;

				if (!isConversionSourceSet && unit.abbreviation == DEFAULT_FROM_CURRENCY)
				{
					unit.isConversionSource = true;
					isConversionSourceSet = true;
				}

				if (!isConversionTargetSet && unit.abbreviation == DEFAULT_TO_CURRENCY)
				{
					unit.isConversionTarget = true;
					isConversionTargetSet = true;
				}
			}
		}

		void NotifyDataLoadFinished(bool didLoad)
		{
			if (!didLoad)
			{
				m_loadStatus = CurrencyLoadStatus.FailedToLoad;
			}

			if (m_vmCallback != null)
			{
				m_vmCallback.CurrencyDataLoadFinished(didLoad);
			}
		}

		void SaveLangCodeAndTimestamp()
		{
			ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
			if (localSettings == null)
			{
				return;
			}

			localSettings.Values[CurrencyDataLoaderConstants.CacheTimestampKey] = m_cacheTimestamp.ToString("R");
			localSettings.Values[CurrencyDataLoaderConstants.CacheLangcodeKey] = m_responseLanguage;
		}

		void UpdateDisplayedTimestamp()
		{
			if (m_vmCallback != null)
			{
				string timestamp = GetCurrencyTimestamp();
				bool isWeekOld = Utils.IsDateTimeOlderThan(m_cacheTimestamp, WEEK_DURATION);

				m_vmCallback.CurrencyTimestampCallback(timestamp, isWeekOld);
			}
		}

		public string GetCurrencyTimestamp()
		{
			string timestamp = "";

			DateTime epoch = default(DateTime);
			if (m_cacheTimestamp.ToUniversalTime() != epoch.ToUniversalTime())
			{
				// TODO UNO
				//DateTimeFormatter dateFormatter = new DateTimeFormatter("{month.abbreviated} {day.integer}, {year.full}");
				//string date = dateFormatter.Format(m_cacheTimestamp);
				var date = m_cacheTimestamp.ToString("D");

				// TODO UNO
				//DateTimeFormatter timeFormatter = new DateTimeFormatter("shorttime");
				//string time = timeFormatter.Format(m_cacheTimestamp);
				var time = m_cacheTimestamp.ToString("t");

				timestamp = LocalizationStringUtil.GetLocalizedString(m_timestampFormat, date, time);
			}

			return timestamp;
		}

		async Task<SelectedUnits> GetDefaultFromToCurrency()
		{
			string fromCurrency = DEFAULT_FROM_CURRENCY;
			string toCurrency = DEFAULT_TO_CURRENCY;

			// First, check if we previously stored the last used currencies.
			bool foundInLocalSettings = TryGetLastUsedCurrenciesFromLocalSettings(out fromCurrency, out toCurrency);
			if (!foundInLocalSettings)
			{
				try
				{
					// Second, see if the current locale has preset defaults in DefaultFromToCurrency.json.
					Uri fileUri = new Uri(DEFAULT_FROM_TO_CURRENCY_FILE_URI);
					StorageFile defaultFromToCurrencyFile = await StorageFile.GetFileFromApplicationUriAsync(fileUri); // TODO UNO
					if (defaultFromToCurrencyFile != null)
					{
						String fileContents = await FileIO.ReadTextAsync(defaultFromToCurrencyFile);
						JsonObject fromToObject = JsonObject.Parse(fileContents);
						JsonObject regionalDefaults = fromToObject.GetNamedObject(m_responseLanguage);

						// Get both values before assignment in-case either fails.
						String selectedFrom = regionalDefaults.GetNamedString(FROM_KEY);
						String selectedTo = regionalDefaults.GetNamedString(TO_KEY);

						fromCurrency = selectedFrom;
						toCurrency = selectedTo;
					}
				}
				catch
				{
				}
			}

			return new SelectedUnits(fromCurrency, toCurrency);
		}

		bool TryGetLastUsedCurrenciesFromLocalSettings(out string fromCurrency, out string toCurrency)
		{
			fromCurrency = toCurrency = null;

			String fromKey = UnitConverterResourceKeys.CurrencyUnitFromKey;
			String toKey = UnitConverterResourceKeys.CurrencyUnitToKey;
			ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
			if (localSettings != null && localSettings.Values != null)
			{
				IPropertySet values = localSettings.Values;
				if (values.ContainsKey(fromKey) && values.ContainsKey(toKey))
				{
					fromCurrency = (String)(values[fromKey]);
					toCurrency = (String)(values[toKey]);

					return true;
				}
			}

			return false;
		}

		void SaveSelectedUnitsToLocalSettings(SelectedUnits selectedUnits)
		{
			String fromKey = UnitConverterResourceKeys.CurrencyUnitFromKey;
			String toKey = UnitConverterResourceKeys.CurrencyUnitToKey;
			ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
			if (localSettings != null && localSettings.Values != null)
			{
				IPropertySet values = localSettings.Values;
				values[fromKey] = selectedUnits.Key;
				values[toKey] = selectedUnits.Value;
			}
		}
	}
}
