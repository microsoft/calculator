// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Calculator;
using CalculatorApp;
using CategorySelectionInitializer = System.Tuple<CalculatorApp.CalculatorList<UnitConversionManager.Unit>, UnitConversionManager.Unit, UnitConversionManager.Unit>;
using UnitToUnitToConversionDataMap = System.Collections.Generic.Dictionary<UnitConversionManager.Unit, System.Collections.Generic.Dictionary<UnitConversionManager.Unit, UnitConversionManager.ConversionData>>;
using CategoryToUnitVectorMap = System.Collections.Generic.Dictionary<UnitConversionManager.Category, CalculatorApp.CalculatorList<UnitConversionManager.Unit>>;

namespace UnitConversionManager
{
    public class Unit
    {
		public static readonly Unit EMPTY_UNIT = new Unit(-1, "", "", true, true, false);

		public Unit()
		{
		}

		public Unit(int id, string name, string abbreviation, bool isConversionSource, bool isConversionTarget, bool isWhimsical)
        {
            this.id = id;
            this.name = name;
            this.accessibleName = name;
            this.abbreviation = abbreviation;
            this.isConversionSource = isConversionSource;
            this.isConversionTarget = isConversionTarget;
            this.isWhimsical = isWhimsical;
        }

        public Unit(
            int id,
            string currencyName,
            string countryName,
            string abbreviation,
            bool isRtlLanguage,
            bool isConversionSource,
            bool isConversionTarget)
        {
            this.id = id;
            this.abbreviation = abbreviation;
            this.isConversionSource = isConversionSource;
            this.isConversionTarget = isConversionTarget;
            this.isWhimsical = false;

            string nameValue1 = isRtlLanguage ? currencyName : countryName;
            string nameValue2 = isRtlLanguage ? countryName : currencyName;

            name = nameValue1 + " - " + nameValue2;
            accessibleName = nameValue1 + " " + nameValue2;
        }

        public int id;
        public string name;
        public string accessibleName;
        public string abbreviation;
        public bool isConversionSource;
        public bool isConversionTarget;
        public bool isWhimsical;

		/// <inheritdoc />
		public override bool Equals(object obj)
			=> obj is Unit other && id == other.id;

		public override int GetHashCode()
			=> id;

		public static bool operator !=(Unit left, Unit right)
		{
			return left.id != right.id;
		}

		public static bool operator ==(Unit left, Unit right)
		{
			return left.id == right.id;
		}

		// class UnitHash
		public static explicit operator int(Unit x)
		{
			return x.id;
		}
	}

	public struct Category
    {
        public Category(int id, string name, bool supportsNegative)
        {
			this.id = id;
			this.name = name;
			this.supportsNegative = supportsNegative;
		}

        public int id;
        public string name;
        public bool supportsNegative;

		/// <inheritdoc />
		public override bool Equals(object obj)
			=> obj is Category other
			&& id == other.id;

		/// <inheritdoc />
		public override int GetHashCode()
			=> id;

		public static bool operator!=(Category left, Category right)
        {
            return left.id != right.id;
        }

		public static bool operator ==(Category left, Category right)
		{
			return left.id == right.id;
		}

		// class CategoryHash
		public static explicit operator int(Category x)
		{
			return x.id;
		}
	}

    public struct SuggestedValueIntermediate
    {
        public double magnitude;
        public double value;
        public Unit type;
    };

    public class ConversionData
    {
		public ConversionData()
		{
		}

		public ConversionData(double ratio, double offset, bool offsetFirst)
		{
			this.ratio = ratio;
			this.offset = offset;
			this.offsetFirst = offsetFirst;
		}

        public double ratio;
        public double offset;
        public bool offsetFirst;
    };

    public struct CurrencyStaticData
    {
		public CurrencyStaticData(
			string countryCode,
			string countryName,
			string currencyCode,
			string currencyName,
			string currencySymbol)
		{
			this.countryCode = countryCode;
			this.countryName = countryName;
			this.currencyCode = currencyCode;
			this.currencyName = currencyName;
			this.currencySymbol = currencySymbol;
		}

		public string countryCode;
        public string countryName;
        public string currencyCode;
        public string currencyName;
        public string currencySymbol;
    };

    public struct CurrencyRatio
    {
		public CurrencyRatio(
			double ratio,
			string sourceCurrencyCode,
			string targetCurrencyCode)
		{
			this.ratio = ratio;
			this.sourceCurrencyCode = sourceCurrencyCode;
			this.targetCurrencyCode = targetCurrencyCode;
		}

		public double ratio;
        public string sourceCurrencyCode;
        public string targetCurrencyCode;
    };

	// typedef std.tuple<CalculatorList<UnitConversionManager.Unit>, UnitConversionManager.Unit, UnitConversionManager.Unit> CategorySelectionInitializer;
	// typedef std.unordered_map<
	//     UnitConversionManager.Unit,
	//     std.unordered_map<UnitConversionManager.Unit, UnitConversionManager.ConversionData, UnitConversionManager.UnitHash>,
	//     UnitConversionManager.UnitHash>
	//     UnitToUnitToConversionDataMap;
	// typedef std.unordered_map<UnitConversionManager.Category, CalculatorList<UnitConversionManager.Unit>, UnitConversionManager.CategoryHash>
	//     CategoryToUnitVectorMap;
	// 

	public interface IViewModelCurrencyCallback
	{
		void CurrencyDataLoadFinished(bool didLoad);
        void CurrencySymbolsCallback(string fromSymbol, string toSymbol);
        void CurrencyRatiosCallback(string ratioEquality, string accRatioEquality);
        void CurrencyTimestampCallback(string timestamp, bool isWeekOldData);
        void NetworkBehaviorChanged( int newBehavior);
    };

    public interface IConverterDataLoader
    {
        void LoadData(); // prepare data if necessary before calling other functions
        CalculatorList<Category> LoadOrderedCategories();
		CalculatorList<Unit> LoadOrderedUnits(Category c);
        Dictionary<Unit, ConversionData> LoadOrderedRatios(Unit u);
        bool SupportsCategory(Category target);
    };

    public interface ICurrencyConverterDataLoader
    {
        void SetViewModelCallback(IViewModelCurrencyCallback callback);
        KeyValuePair<string, string> GetCurrencySymbols( Unit unit1,  Unit unit2);
		KeyValuePair<string, string> GetCurrencyRatioEquality( Unit unit1,  Unit unit2);
        string GetCurrencyTimestamp();

        Task<bool> TryLoadDataFromCacheAsync();
        Task<bool> TryLoadDataFromWebAsync();
        Task<bool> TryLoadDataFromWebOverrideAsync();
    };

    public interface IUnitConverterVMCallback
    {
        void DisplayCallback(string from, string to);
        void SuggestedValueCallback(CalculatorList<Tuple<string, Unit>> suggestedValues);
        void MaxDigitsReached();
    };

    public interface IUnitConverter
    {
        void Initialize(); // Use to initialize first time, use deserialize instead to rehydrate
        CalculatorList<Category> GetCategories();
        CategorySelectionInitializer SetCurrentCategory(Category input);
        Category GetCurrentCategory();
        void SetCurrentUnitTypes(Unit fromType, Unit toType);
        void SwitchActive(string newValue);
        string Serialize();
        void DeSerialize(string serializedData);
        string SaveUserPreferences();
        void RestoreUserPreferences( string userPreferences);
        void SendCommand(Command command);
        void SetViewModelCallback(IUnitConverterVMCallback newCallback);
        void SetViewModelCurrencyCallback(IViewModelCurrencyCallback newCallback);
        Task<KeyValuePair<bool, string>> RefreshCurrencyRatios();
        void Calculate();
        void ResetCategoriesAndRatios();
    };

	// .h
    public partial class UnitConverter : IUnitConverter//, public std.enable_shared_from_this<UnitConverter>
    {
        //public UnitConverter(IConverterDataLoader dataLoader);
        //public UnitConverter(IConverterDataLoader dataLoader,  IConverterDataLoader currencyDataLoader);

        // IUnitConverter
        //public partial void Initialize() ;
        //public partial CalculatorList<Category> GetCategories() ;
        //public partial CategorySelectionInitializer SetCurrentCategory(Category input) ;
        //public partial Category GetCurrentCategory() ;
        //public partial void SetCurrentUnitTypes(Unit fromType, Unit toType) ;
        //public partial void SwitchActive(string newValue) ;
        //public partial string Serialize() ;
        //public partial void DeSerialize(string serializedData) ;
        //public partial string SaveUserPreferences() ;
        //public partial void RestoreUserPreferences(string userPreference) ;
        //public partial void SendCommand(Command command) ;
        //public partial void SetViewModelCallback( IUnitConverterVMCallback newCallback) ;
        //public partial void SetViewModelCurrencyCallback( IViewModelCurrencyCallback newCallback) ;
        //public partial Task<KeyValuePair<bool, string>> RefreshCurrencyRatios() ;
        //public partial void Calculate() ;
        //public partial void ResetCategoriesAndRatios() ;
        //// IUnitConverter

        //static CalculatorList<string> StringToVector(string w, char delimiter, bool addRemainder = false);
        //static string Quote(string s);
        //static string Unquote(string s);

    //private:
    //    bool CheckLoad();
    //    double Convert(double value, ConversionData conversionData);
    //    CalculatorList<std.tuple<string, Unit>> CalculateSuggested();
    //    void ClearValues();
    //    void TrimString(string& input);
    //    void InitializeSelectedUnits();
    //    string RoundSignificant(double num, int numSignificant);
    //    Category StringToCategory(string w);
    //    string CategoryToString(Category c, char delimiter);
    //    string UnitToString(Unit u, char delimiter);
    //    Unit StringToUnit(string w);
    //    ConversionData StringToConversionData(string w);
    //    string ConversionDataToString(ConversionData d, char delimiter);
    //    void UpdateCurrencySymbols();
    //    void UpdateViewModel();
    //    bool AnyUnitIsEmpty();
    //    std.IConverterDataLoader GetDataLoaderForCategory(Category category);
    //    std.ICurrencyConverterDataLoader GetCurrencyConverterDataLoader();

        private IConverterDataLoader m_dataLoader;
        private IConverterDataLoader m_currencyDataLoader;
        private IUnitConverterVMCallback m_vmCallback;
        private IViewModelCurrencyCallback m_vmCurrencyCallback;
        private CalculatorList<Category> m_categories = new CalculatorList<Category>();
        private CategoryToUnitVectorMap m_categoryToUnits = new CategoryToUnitVectorMap(EqualityComparer<Category>.Default);
        private UnitToUnitToConversionDataMap m_ratioMap = new UnitToUnitToConversionDataMap(EqualityComparer<Unit>.Default);
        private Category m_currentCategory;
        private Unit m_fromType = Unit.EMPTY_UNIT;
        private Unit m_toType = Unit.EMPTY_UNIT;
        private string m_currentDisplay;
        private string m_returnDisplay;
        private bool m_currentHasDecimal;
        private bool m_returnHasDecimal;
        private bool m_switchedActive;
    };

	// .cpp
	partial class UnitConverter
	{
		private static readonly Unit EMPTY_UNIT = Unit.EMPTY_UNIT;

		const uint EXPECTEDSERIALIZEDTOKENCOUNT = 7;
		const uint EXPECTEDSERIALIZEDCONVERSIONDATATOKENCOUNT = 3;
		const uint EXPECTEDSERIALIZEDCATEGORYTOKENCOUNT = 3;
		const uint EXPECTEDSERIALIZEDUNITTOKENCOUNT = 6;
		const uint EXPECTEDSTATEDATATOKENCOUNT = 5;
		const uint EXPECTEDMAPCOMPONENTTOKENCOUNT = 2;

		static  int MAXIMUMDIGITSALLOWED = 15;
		static  int OPTIMALDIGITSALLOWED = 7;

		static  char LEFTESCAPECHAR = '{';
		static  char RIGHTESCAPECHAR = '}';

		static readonly double OPTIMALDECIMALALLOWED = Math.Pow(10, -1 * (OPTIMALDIGITSALLOWED - 1));
		static readonly double MINIMUMDECIMALALLOWED = Math.Pow(10, -1 * (MAXIMUMDIGITSALLOWED - 1));

		Dictionary<char, string> quoteConversions = new Dictionary<char, string>();
		Dictionary<string, char> unquoteConversions = new Dictionary<string, char>();

		/// <summary>
		/// Constructor, sets up all the variables and requires a configLoader
		/// </summary>
		/// <param name="dataLoader">An instance of the IConverterDataLoader interface which we use to read in category/unit names and conversion data</param>
		public UnitConverter( IConverterDataLoader dataLoader)
			: this(dataLoader, null)
		{
		}

		/// <summary>
		/// Constructor, sets up all the variables and requires two configLoaders
		/// </summary>
		/// <param name="dataLoader">An instance of the IConverterDataLoader interface which we use to read in category/unit names and conversion data</param>
		/// <param name="currencyDataLoader">An instance of the IConverterDataLoader interface, specialized for loading currency data from an internet service</param>
		public UnitConverter( IConverterDataLoader dataLoader,  IConverterDataLoader currencyDataLoader)
		{
			m_dataLoader = dataLoader;
			m_currencyDataLoader = currencyDataLoader;
			// declaring the delimiter character conversion map
			quoteConversions['|'] = "{p}";
			quoteConversions['['] = "{lc}";
			quoteConversions[']'] = "{rc}";
			quoteConversions[':'] = "{co}";
			quoteConversions[','] = "{cm}";
			quoteConversions[';'] = "{sc}";
			quoteConversions[LEFTESCAPECHAR] = "{lb}";
			quoteConversions[RIGHTESCAPECHAR] = "{rb}";
			unquoteConversions["{p}"] = '|';
			unquoteConversions["{lc}"] = '[';
			unquoteConversions["{rc}"] = ']';
			unquoteConversions["{co}"] = ':';
			unquoteConversions["{cm}"] = ',';
			unquoteConversions["{sc}"] = ';';
			unquoteConversions["{lb}"] = LEFTESCAPECHAR;
			unquoteConversions["{rb}"] = RIGHTESCAPECHAR;
			ClearValues();
			ResetCategoriesAndRatios();
		}

		public void Initialize()
		{
			m_dataLoader.LoadData();
		}

		bool CheckLoad()
		{
			if (m_categories.IsEmpty())
			{
				ResetCategoriesAndRatios();
			}
			return !m_categories.IsEmpty();
		}

		/// <summary>
		/// Returns a list of the categories in use by this converter
		/// </summary>
		public CalculatorList<Category> GetCategories()
		{
			CheckLoad();
			return m_categories;
		}

		/// <summary>
		/// Sets the current category in use by this converter,
		/// and returns a list of unit types that exist under the given category.
		/// </summary>
		/// <param name="input">Category struct which we are setting</param>
		public CategorySelectionInitializer SetCurrentCategory(Category input)
		{
			if (m_currencyDataLoader != null && m_currencyDataLoader.SupportsCategory(input))
			{
				m_currencyDataLoader.LoadData();
			}

			CalculatorList<Unit> newUnitList = new CalculatorList<Unit>();
			if (CheckLoad())
			{
				if (m_currentCategory.id != input.id)
				{
					CalculatorList<Unit> unitVector = m_categoryToUnits[m_currentCategory];
					for (uint  i = 0; i < unitVector.Size(); i++)
					{
						unitVector[i].isConversionSource = (unitVector[i].id == m_fromType.id);
						unitVector[i].isConversionTarget = (unitVector[i].id == m_toType.id);
					}
					m_currentCategory = input;
					if (!m_currentCategory.supportsNegative && m_currentDisplay.First() == '-')
					{
						m_currentDisplay = m_currentDisplay.Remove(0, 1);
					}
				}

				newUnitList = m_categoryToUnits[input];
			}

			InitializeSelectedUnits();
			return Tuple.Create(newUnitList, m_fromType, m_toType);
		}

		/// <summary>
		/// Gets the category currently being used
		/// </summary>
		public Category GetCurrentCategory()
		{
			return m_currentCategory;
		}

		/// <summary>
		/// Sets the current unit types to be used, indicates a likely change in the
		/// display values, so we re-calculate and callback the updated values
		/// </summary>
		/// <param name="fromType">Unit struct which the user is modifying</param>
		/// <param name="toType">Unit struct we are converting to</param>
		public void SetCurrentUnitTypes(Unit fromType, Unit toType)
		{
			if (!CheckLoad())
			{
				return;
			}

			m_fromType = fromType;
			m_toType = toType;
			Calculate();

			UpdateCurrencySymbols();
		}

		/// <summary>
		/// Switches the active field, indicating that we are now entering data into
		/// what was originally the return field, and storing results into what was
		/// originally the current field. We swap appropriate values,
		/// but do not callback, as values have not changed.
		/// </summary>
		/// <param name="newValue">
		/// string representing the value user had in the field they've just activated.
		/// We use this to handle cases where the front-end may choose to trim more digits
		/// than we have been storing internally, in which case appending will not function
		/// as expected without the use of this parameter.
		/// </param>
		public void SwitchActive(string newValue)
		{
			if (!CheckLoad())
			{
				return;
			}

			Utils.Swap(ref m_fromType, ref m_toType);
			Utils.Swap(ref m_currentHasDecimal, ref m_returnHasDecimal);
			m_returnDisplay = m_currentDisplay;
			m_currentDisplay = newValue;
			m_currentHasDecimal = (m_currentDisplay.IndexOf('.') != m_currentDisplay.npos());
			m_switchedActive = true;

			if (m_currencyDataLoader != null && m_vmCurrencyCallback != null)
			{
				ICurrencyConverterDataLoader currencyDataLoader = GetCurrencyConverterDataLoader();
				KeyValuePair<string, string> currencyRatios = currencyDataLoader.GetCurrencyRatioEquality(m_fromType, m_toType);

				m_vmCurrencyCallback.CurrencyRatiosCallback(currencyRatios.Key, currencyRatios.Value);
			}
		}

		string CategoryToString(Category c, char delimiter)
		{
			using (var @out = new StringWriter())
			{

				@out.Write(Quote(c.id.ToString()));
				@out.Write(delimiter);
				@out.Write(Quote(c.supportsNegative.ToString()));
				@out.Write(delimiter);
				@out.Write(Quote(c.name));
				@out.Write(delimiter);

				return @out.ToString();
			}
		}

		public static CalculatorList<string> StringToVector(string w, char delimiter, bool addRemainder = false)
		{
			int delimiterIndex = w.IndexOf(delimiter);
			int startIndex = 0;
			CalculatorList<string> serializedTokens = new CalculatorList<string>();
			while (delimiterIndex != w.npos())
			{
				serializedTokens.PushBack(w.substr(startIndex, delimiterIndex - startIndex));
				startIndex = delimiterIndex + 1 /*(int)wcslen(delimiter)*/;
				delimiterIndex = w.IndexOf(delimiter, startIndex);
			}
			if (addRemainder)
			{
				delimiterIndex = w.Length;
				serializedTokens.PushBack(w.substr(startIndex, delimiterIndex - startIndex));
			}
			return serializedTokens;
		}

		public static CalculatorList<string> StringToVector(string w, string delimiter, bool addRemainder = false)
		{
			int delimiterIndex = w.IndexOf(delimiter);
			int startIndex = 0;
			CalculatorList<string> serializedTokens = new CalculatorList<string>();
			while (delimiterIndex != w.npos())
			{
				serializedTokens.PushBack(w.substr(startIndex, delimiterIndex - startIndex));
				startIndex = delimiterIndex + 1 /*(int)wcslen(delimiter)*/;
				delimiterIndex = w.IndexOf(delimiter, startIndex);
			}
			if (addRemainder)
			{
				delimiterIndex = w.Length;
				serializedTokens.PushBack(w.substr(startIndex, delimiterIndex - startIndex));
			}
			return serializedTokens;
		}

		Category StringToCategory(string w)
		{
			CalculatorList<string> tokenList = StringToVector(w, ';');
			Debug.Assert(tokenList.Size() == EXPECTEDSERIALIZEDCATEGORYTOKENCOUNT);
			Category serializedCategory = new Category();
			serializedCategory.id = System.Convert.ToInt32(Unquote(tokenList[0]));
			serializedCategory.supportsNegative = (tokenList[1].CompareTo("1") == 0);
			serializedCategory.name = Unquote(tokenList[2]);
			return serializedCategory;
		}

		string UnitToString(Unit u, char delimiter)
		{
			using (var @out = new StringWriter())
			{
				@out.Write(Quote(u.id.ToString()));
				@out.Write(delimiter);
				@out.Write(Quote(u.name));
				@out.Write(delimiter);
				@out.Write(Quote(u.abbreviation));
				@out.Write(delimiter);
				@out.Write(u.isConversionSource.ToString());
				@out.Write(delimiter);
				@out.Write(u.isConversionTarget.ToString());
				@out.Write(delimiter);
				@out.Write(u.isWhimsical.ToString());
				@out.Write(delimiter);

				return @out.ToString();
			}
		}

		Unit StringToUnit(string w)
		{
			CalculatorList<string> tokenList = StringToVector(w, ';');
			Debug.Assert(tokenList.Size() == EXPECTEDSERIALIZEDUNITTOKENCOUNT);
			Unit serializedUnit = new Unit();
			serializedUnit.id = System.Convert.ToInt32(Unquote(tokenList[0]));
			serializedUnit.name = Unquote(tokenList[1]);
			serializedUnit.accessibleName = serializedUnit.name;
			serializedUnit.abbreviation = Unquote(tokenList[2]);
			serializedUnit.isConversionSource = (tokenList[3].CompareTo("1") == 0);
			serializedUnit.isConversionTarget = (tokenList[4].CompareTo("1") == 0);
			serializedUnit.isWhimsical = (tokenList[5].CompareTo("1") == 0);
			return serializedUnit;
		}

		ConversionData StringToConversionData(string w)
		{
			CalculatorList<string> tokenList = StringToVector(w, ';');
			Debug.Assert(tokenList.Size() == EXPECTEDSERIALIZEDCONVERSIONDATATOKENCOUNT);
			ConversionData serializedConversionData = new ConversionData();
			serializedConversionData.ratio = System.Convert.ToDouble(Unquote(tokenList[0]), CultureInfo.InvariantCulture);
			serializedConversionData.offset = System.Convert.ToDouble(Unquote(tokenList[1]), CultureInfo.InvariantCulture);
			serializedConversionData.offsetFirst = (tokenList[2].CompareTo("1") == 0);
			return serializedConversionData;
		}

		string ConversionDataToString(ConversionData d, char delimiter)
		{
			using (var @out = new StringWriter())
			{
				string ratio = d.ratio.ToString("F32", CultureInfo.InvariantCulture);
				string offset = d.offset.ToString("F32", CultureInfo.InvariantCulture);
				TrimString(ref ratio);
				TrimString(ref offset);
				@out.Write(Quote(ratio));
				@out.Write(delimiter);
				@out.Write(Quote(offset));
				@out.Write(delimiter);
				@out.Write(d.offsetFirst);
				@out.Write(delimiter);

				return @out.ToString();
			}
		}

		/// <summary>
		/// Serializes the data in the converter and returns it as a string
		/// </summary>
		public string Serialize()
		{
			if (!CheckLoad())
			{
				return string.Empty;
			}

			using (var @out = new StringWriter())
			{
				char delimiter = ';';

				@out.Write(UnitToString(m_fromType, delimiter));
				@out.Write('|');
				@out.Write(UnitToString(m_toType, delimiter));
				@out.Write('|');
				@out.Write(CategoryToString(m_currentCategory, delimiter));
				@out.Write('|');
				@out.Write(m_currentHasDecimal);
				@out.Write(delimiter);
				@out.Write(m_returnHasDecimal);
				@out.Write(delimiter);
				@out.Write(m_switchedActive);
				@out.Write(delimiter);
				@out.Write(m_currentDisplay);
				@out.Write(delimiter);
				@out.Write(m_returnDisplay);
				@out.Write(delimiter);
				@out.Write('|');

				using (var categoryString = new StringWriter())
				using (var categoryToUnitString = new StringWriter())
				using (var unitToUnitToDoubleString = new StringWriter())
				{
					foreach (Category c in m_categories)
					{
						categoryString.Write(CategoryToString(c, delimiter));
						categoryString.Write(',');
					}

					foreach (var cur in m_categoryToUnits)
					{
						categoryToUnitString.Write(CategoryToString(cur.Key, delimiter));
						categoryToUnitString.Write('[');
						foreach (Unit u in cur.Value)
						{
							categoryToUnitString.Write(UnitToString(u, delimiter));
							categoryToUnitString.Write(',');
						}
						categoryToUnitString.Write("[]");
					}

					foreach (var cur in m_ratioMap)
					{
						unitToUnitToDoubleString.Write(UnitToString(cur.Key, delimiter));
						categoryToUnitString.Write('[');
						foreach (var curConversion in cur.Value)
						{
							unitToUnitToDoubleString.Write(UnitToString(curConversion.Key, delimiter));
							unitToUnitToDoubleString.Write(':');
							unitToUnitToDoubleString.Write(ConversionDataToString(curConversion.Value, delimiter));
							unitToUnitToDoubleString.Write(":,");
						}
						unitToUnitToDoubleString.Write("[]");
					}

					@out.Write(categoryString.ToString());
					@out.Write('|');
					@out.Write(categoryToUnitString.ToString());
					@out.Write('|');
					@out.Write(unitToUnitToDoubleString.ToString());
					@out.Write('|');

					return @out.ToString();
				}
			}
		}

		/// <summary>
		/// De-Serializes the data in the converter from a string
		/// </summary>
		/// <param name="serializedData">string holding the serialized data. If it does not have expected number of parameters, we will ignore it</param>
		public void DeSerialize(string serializedData)
		{
			ClearValues();
			ResetCategoriesAndRatios();

			if (string.IsNullOrEmpty(serializedData))
			{
				return;
			}

			CalculatorList<string> outerTokens = StringToVector(serializedData, '|');
			Debug.Assert(outerTokens.Size() == EXPECTEDSERIALIZEDTOKENCOUNT);
			m_fromType = StringToUnit(outerTokens[0]);
			m_toType = StringToUnit(outerTokens[1]);
			m_currentCategory = StringToCategory(outerTokens[2]);
			CalculatorList<string> stateDataTokens = StringToVector(outerTokens[3], ';');
			Debug.Assert(stateDataTokens.Size() == EXPECTEDSTATEDATATOKENCOUNT);
			m_currentHasDecimal = (stateDataTokens[0].CompareTo('1') == 0);
			m_returnHasDecimal = (stateDataTokens[1].CompareTo('1') == 0);
			m_switchedActive = (stateDataTokens[2].CompareTo('1') == 0);
			m_currentDisplay = stateDataTokens[3];
			m_returnDisplay = stateDataTokens[4];
			CalculatorList<string> categoryListTokens = StringToVector(outerTokens[4], ',');
			foreach (string token in categoryListTokens)
			{
				m_categories.PushBack(StringToCategory(token));
			}
			CalculatorList<string> unitVectorTokens = StringToVector(outerTokens[5], ']');
			foreach (string unitVector in unitVectorTokens)
			{
				CalculatorList<string> mapcomponents = StringToVector(unitVector, '[');
				Debug.Assert(mapcomponents.Size() == EXPECTEDMAPCOMPONENTTOKENCOUNT);
				Category key = StringToCategory(mapcomponents[0]);
				CalculatorList<string> units = StringToVector(mapcomponents[1], ',');
				foreach (string unit in units)
				{
					m_categoryToUnits[key].PushBack(StringToUnit(unit));
				}
			}
			CalculatorList<string> ratioMapTokens = StringToVector(outerTokens[6], ']');
			foreach (string token in ratioMapTokens)
			{
				CalculatorList<string> ratioMapComponentTokens = StringToVector(token, '[');
				Debug.Assert(ratioMapComponentTokens.Size() == EXPECTEDMAPCOMPONENTTOKENCOUNT);
				Unit key = StringToUnit(ratioMapComponentTokens[0]);
				CalculatorList<string> ratioMapList = StringToVector(ratioMapComponentTokens[1], ',');
				foreach (string subtoken in ratioMapList)
				{
					CalculatorList<string> ratioMapSubComponentTokens = StringToVector(subtoken, ':');
					Debug.Assert(ratioMapSubComponentTokens.Size() == EXPECTEDMAPCOMPONENTTOKENCOUNT);
					Unit subkey = StringToUnit(ratioMapSubComponentTokens[0]);
					ConversionData conversion = StringToConversionData(ratioMapSubComponentTokens[1]);
					m_ratioMap[key][subkey] = conversion;
				}
			}
			UpdateViewModel();
		}

		/// <summary>
		/// De-Serializes the data in the converter from a string
		/// </summary>
		/// <param name="userPreferences">string holding the serialized data. If it does not have expected number of parameters, we will ignore it</param>
		public void RestoreUserPreferences(string userPreferences)
		{
			if (string.IsNullOrEmpty(userPreferences))
			{
				return;
			}

			CalculatorList<string> outerTokens = StringToVector(userPreferences, '|');
			if (outerTokens.Size() != 3)
			{
				return;
			}

			var fromType = StringToUnit(outerTokens[0]);
			var toType = StringToUnit(outerTokens[1]);
			m_currentCategory = StringToCategory(outerTokens[2]);

			// Only restore from the saved units if they are valid in the current available units.
			if (m_categoryToUnits.TryGetValue(m_currentCategory, out var curUnits))
			{
				if (curUnits.IndexOf(fromType) != curUnits.Count)
				{
					m_fromType = fromType;
				}
				if (curUnits.IndexOf(toType) != curUnits.Count)
				{
					m_toType = toType;
				}
			}
		}

		/// <summary>
		/// Serializes the Category and Associated Units in the converter and returns it as a string
		/// </summary>
		public string SaveUserPreferences()
		{
			char delimiter = ';';

			using (var writer = new StringWriter())
			{
				writer.Write(UnitToString(m_fromType, delimiter));
				writer.Write('|');
				writer.Write(UnitToString(m_toType, delimiter));
				writer.Write('|');
				writer.Write(CategoryToString(m_currentCategory, delimiter));
				writer.Write('|');

				return writer.ToString();
			}
		}

		/// <summary>
		/// Sanitizes the input string, escape quoting any symbols we rely on for our delimiters, and returns the sanitized string.
		/// </summary>
		/// <param name="s">string to be sanitized</param>
		public string Quote(string s)
		{
			using (var quotedString = new StringWriter())
			using (var cursor = s.GetEnumerator())
			{
				while (cursor.MoveNext())
				{
					if (quoteConversions.TryGetValue(cursor.Current, out var conversion))
					{
						quotedString.Write(conversion);
					}
					else
					{
						quotedString.Write(cursor.Current);
					}
				}

				return quotedString.ToString();
			}
		}

		/// <summary>
		/// Unsanitizes the sanitized input string, returning it to its original contents before we had quoted it.
		/// </summary>
		/// <param name="s">string to be unsanitized</param>
		public string Unquote(string s)
		{
			using (var unquotedString = new StringWriter())
			using (var cursor = s.GetEnumerator())
			{
				while (cursor.MoveNext())
				{
					if (cursor.Current == LEFTESCAPECHAR)
					{
						using (var quotedSubString = new StringWriter())
						{
							while (cursor.MoveNext() && cursor.Current != RIGHTESCAPECHAR)
							{
								quotedSubString.Write(cursor.Current);
							}

							if (unquoteConversions.TryGetValue(quotedSubString.ToString(), out var conversion))
							{
								unquotedString.Write(conversion);
							}
							else
							{
								// Badly formatted
								break;
							}
						}
					}
					else
					{
						unquotedString.Write(cursor.Current);
					}
				}

				return unquotedString.ToString();
			}
		}

		/// <summary>
		/// Handles inputs to the converter from the view-model, corresponding to a given button or keyboard press
		/// </summary>
		/// <param name="command">Command enum representing the command that was entered</param>
		public void SendCommand(Command command)
		{
			if (!CheckLoad())
			{
				return;
			}

			// TODO: Localization of characters
			bool clearFront = false;
			if (m_currentDisplay == "0")
			{
				clearFront = true;
			}
			bool clearBack = false;
			if ((m_currentHasDecimal && m_currentDisplay.Length - 1 >= MAXIMUMDIGITSALLOWED)
				|| (!m_currentHasDecimal && m_currentDisplay.Length >= MAXIMUMDIGITSALLOWED))
			{
				clearBack = true;
			}
			if (command != Command.Negate && m_switchedActive)
			{
				ClearValues();
				m_switchedActive = false;
				clearFront = true;
				clearBack = false;
			}
			switch (command)
			{
			case Command.Zero:
				m_currentDisplay += "0";
				break;

			case Command.One:
				m_currentDisplay += "1";
				break;

			case Command.Two:
				m_currentDisplay += "2";
				break;

			case Command.Three:
				m_currentDisplay += "3";
				break;

			case Command.Four:
				m_currentDisplay += "4";
				break;

			case Command.Five:
				m_currentDisplay += "5";
				break;

			case Command.Six:
				m_currentDisplay += "6";
				break;

			case Command.Seven:
				m_currentDisplay += "7";
				break;

			case Command.Eight:
				m_currentDisplay += "8";
				break;

			case Command.Nine:
				m_currentDisplay += "9";
				break;

			case Command.Decimal:
				clearFront = false;
				clearBack = false;
				if (!m_currentHasDecimal)
				{
					m_currentDisplay += ".";
					m_currentHasDecimal = true;
				}
				break;

			case Command.Backspace:
				clearFront = false;
				clearBack = false;
				if ((m_currentDisplay.First() != '-' && m_currentDisplay.Length > 1) || m_currentDisplay.Length > 2)
				{
					if (m_currentDisplay.Last() == '.')
					{
						m_currentHasDecimal = false;
					}
					m_currentDisplay = m_currentDisplay.Remove(m_currentDisplay.Length - 1);
				}
				else
				{
					m_currentDisplay = "0";
					m_currentHasDecimal = false;
				}
				break;

			case Command.Negate:
				clearFront = false;
				clearBack = false;
				if (m_currentCategory.supportsNegative)
				{
					if (m_currentDisplay.First() == '-')
					{
						m_currentDisplay = m_currentDisplay.Remove(0, 1);
					}
					else
					{
						m_currentDisplay = m_currentDisplay.Insert(0, "-");
					}
				}
				break;

			case Command.Clear:
				clearFront = false;
				clearBack = false;
				ClearValues();
				break;

			case Command.Reset:
				clearFront = false;
				clearBack = false;
				ClearValues();
				ResetCategoriesAndRatios();
				break;

			default:
				break;
			}

			if (clearFront)
			{
				m_currentDisplay = m_currentDisplay.Remove(0, 1);
			}
			if (clearBack)
			{
				m_currentDisplay = m_currentDisplay.Remove(m_currentDisplay.Length - 1, 1);
				m_vmCallback.MaxDigitsReached();
			}

			Calculate();
		}

		/// <summary>
		/// Sets the callback interface to send display update calls to
		/// </summary>
		/// <param name="newCallback">instance of IDisplayCallback interface that receives our update calls</param>
		public void SetViewModelCallback( IUnitConverterVMCallback newCallback)
		{
			m_vmCallback = newCallback;
			if (CheckLoad())
			{
				UpdateViewModel();
			}
		}

		public void SetViewModelCurrencyCallback( IViewModelCurrencyCallback newCallback)
		{
			m_vmCurrencyCallback = newCallback;

			ICurrencyConverterDataLoader currencyDataLoader = GetCurrencyConverterDataLoader();
			if (currencyDataLoader != null)
			{
				currencyDataLoader.SetViewModelCallback(newCallback);
			}
		}

		public async Task<KeyValuePair<bool, string>> RefreshCurrencyRatios()
		{
			ICurrencyConverterDataLoader currencyDataLoader = GetCurrencyConverterDataLoader();
			bool didLoad;
			if (currencyDataLoader != null)
			{
				didLoad = await currencyDataLoader.TryLoadDataFromWebOverrideAsync();
			}
			else
			{
				didLoad = false;
			}

			string timestamp = "";
			if (currencyDataLoader != null)
			{
				timestamp = currencyDataLoader.GetCurrencyTimestamp();
			}

			return new KeyValuePair<bool, string>(didLoad, timestamp);
		}

		ICurrencyConverterDataLoader GetCurrencyConverterDataLoader()
		{
			return (ICurrencyConverterDataLoader)(m_currencyDataLoader);
		}

		/// <summary>
		/// Converts a double value into another unit type, currently by multiplying by the given double ratio
		/// </summary>
		/// <param name="value">double input value to convert</param>
		/// <param name="ratio">double conversion ratio to use</param>
		double Convert(double value, ConversionData conversionData)
		{
			if (conversionData.offsetFirst)
			{
				return (value + conversionData.offset) * conversionData.ratio;
			}
			else
			{
				return (value * conversionData.ratio) + conversionData.offset;
			}
		}

		/// <summary>
		/// Calculates the suggested values for the current display value and returns them as a vector
		/// </summary>
		CalculatorList<Tuple<string, Unit>> CalculateSuggested()
		{
			if (m_currencyDataLoader != null && m_currencyDataLoader.SupportsCategory(m_currentCategory))
			{
				return new CalculatorList<Tuple<string, Unit>>();
			}

			CalculatorList<Tuple<string, Unit>> returnVector = new CalculatorList<Tuple<string, Unit>>();
			CalculatorList<SuggestedValueIntermediate> intermediateVector = new CalculatorList<SuggestedValueIntermediate>();
			CalculatorList<SuggestedValueIntermediate> intermediateWhimsicalVector = new CalculatorList<SuggestedValueIntermediate>();

			Dictionary<Unit, ConversionData> ratios = m_ratioMap[m_fromType];
			// Calculate converted values for every other unit type in this category, along with their magnitude
			foreach (var cur in ratios)
			{
				if (cur.Key != m_fromType && cur.Key != m_toType)
				{
					double convertedValue = Convert(System.Convert.ToDouble(m_currentDisplay, CultureInfo.InvariantCulture), cur.Value);
					SuggestedValueIntermediate newEntry;
					newEntry.magnitude = Math.Log10(convertedValue);
					newEntry.value = convertedValue;
					newEntry.type = cur.Key;
					if (newEntry.type.isWhimsical == false)
						intermediateVector.PushBack(newEntry);
					else
						intermediateWhimsicalVector.PushBack(newEntry);
				}
			}

			// Sort the resulting list by absolute magnitude, breaking ties by choosing the positive value
			intermediateVector.Sort((SuggestedValueIntermediate first, SuggestedValueIntermediate second) => {
				if (Math.Abs(first.magnitude) == Math.Abs(second.magnitude))
				{
					return first.magnitude > second.magnitude;
				}
				else
				{
					return Math.Abs(first.magnitude) < Math.Abs(second.magnitude);
				}
			});

			// Now that the list is sorted, iterate over it and populate the return vector with properly rounded and formatted return strings
			foreach (var entry in intermediateVector)
			{
				string roundedString;
				if (Math.Abs(entry.value) < 100)
				{
					roundedString = RoundSignificant(entry.value, 2);
				}
				else if (Math.Abs(entry.value) < 1000)
				{
					roundedString = RoundSignificant(entry.value, 1);
				}
				else
				{
					roundedString = RoundSignificant(entry.value, 0);
				}
				if (System.Convert.ToDouble(roundedString, CultureInfo.InvariantCulture) != 0.0 || m_currentCategory.supportsNegative)
				{
					TrimString(ref roundedString);
					returnVector.PushBack(Tuple.Create(roundedString, entry.type));
				}
			}

			// The Whimsicals are determined differently
			// Sort the resulting list by absolute magnitude, breaking ties by choosing the positive value
			intermediateWhimsicalVector.Sort((SuggestedValueIntermediate first, SuggestedValueIntermediate second) => {
				if (Math.Abs(first.magnitude) == Math.Abs(second.magnitude))
				{
					return first.magnitude > second.magnitude;
				}
				else
				{
					return Math.Abs(first.magnitude) < Math.Abs(second.magnitude);
				}
			});

			// Now that the list is sorted, iterate over it and populate the return vector with properly rounded and formatted return strings
			CalculatorList<Tuple<string, Unit>> whimsicalReturnVector = new CalculatorList<Tuple<string, Unit>>();

			foreach (var entry in intermediateWhimsicalVector)
			{
				string roundedString;
				if (Math.Abs(entry.value) < 100)
				{
					roundedString = RoundSignificant(entry.value, 2);
				}
				else if (Math.Abs(entry.value) < 1000)
				{
					roundedString = RoundSignificant(entry.value, 1);
				}
				else
				{
					roundedString = RoundSignificant(entry.value, 0);
				}

				// How to work out which is the best whimsical value to add to the vector?
				if (System.Convert.ToDouble(roundedString, CultureInfo.InvariantCulture) != 0.0)
				{
					TrimString(ref roundedString);
					whimsicalReturnVector.PushBack(Tuple.Create(roundedString, entry.type));
				}
			}
			// Pickup the 'best' whimsical value - currently the first one
			if (whimsicalReturnVector.Size() != 0)
			{
				returnVector.PushBack(whimsicalReturnVector.At(0));
			}

			return returnVector;
		}

		/// <summary>
		/// Resets categories and ratios
		/// </summary>
		public void ResetCategoriesAndRatios()
		{
			m_categories = m_dataLoader.LoadOrderedCategories();

			m_switchedActive = false;

			if (m_categories.IsEmpty())
			{
				return;
			}

			m_currentCategory = m_categories[0];

			m_categoryToUnits.Clear();
			m_ratioMap.Clear();
			bool readyCategoryFound = false;
			foreach (Category category in m_categories)
			{
				IConverterDataLoader activeDataLoader = GetDataLoaderForCategory(category);
				if (activeDataLoader == null)
				{
					// The data loader is different depending on the category, e.g. currency data loader
					// is different from the static data loader.
					// If there is no data loader for this category, continue.
					continue;
				}

				CalculatorList<Unit> units = activeDataLoader.LoadOrderedUnits(category);
				m_categoryToUnits[category] = units;

				// Just because the units are empty, doesn't mean the user can't select this category,
				// we just want to make sure we don't let an unready category be the default.
				if (!units.IsEmpty())
				{
					foreach (Unit u in units)
					{
						m_ratioMap[u] = activeDataLoader.LoadOrderedRatios(u);
					}

					if (!readyCategoryFound)
					{
						m_currentCategory = category;
						readyCategoryFound = true;
					}
				}
			}

			InitializeSelectedUnits();
		}

		/// <summary>
		/// Sets the active data loader based on the input category.
		/// </summary>
		IConverterDataLoader GetDataLoaderForCategory(Category category)
		{
			if (m_currencyDataLoader != null && m_currencyDataLoader.SupportsCategory(category))
			{
				return m_currencyDataLoader;
			}
			else
			{
				return m_dataLoader;
			}
		}

		/// <summary>
		/// Sets the initial values for m_fromType and m_toType.
		/// This is an internal helper method as opposed to SetCurrentUnits
		/// which is for external use by clients.
		/// If we fail to set units, we will fallback to the EMPTY_UNIT.
		/// </summary>
		void InitializeSelectedUnits()
		{
			if (m_categoryToUnits.Count == 0)
			{
				return;
			}

			if (!m_categoryToUnits.TryGetValue(m_currentCategory, out var curUnits))
			{
				return;
			}

			if (!curUnits.IsEmpty())
			{
				bool conversionSourceSet = false;
				bool conversionTargetSet = false;
				foreach (Unit cur in curUnits)
				{
					if (!conversionSourceSet && cur.isConversionSource)
					{
						m_fromType = cur;
						conversionSourceSet = true;
					}

					if (!conversionTargetSet && cur.isConversionTarget)
					{
						m_toType = cur;
						conversionTargetSet = true;
					}

					if (conversionSourceSet && conversionTargetSet)
					{
						return;
					}
				}
			}

			m_fromType = EMPTY_UNIT;
			m_toType = EMPTY_UNIT;
		}

		/// <summary>
		/// Resets the value fields to 0
		/// </summary>
		void ClearValues()
		{
			m_currentHasDecimal = false;
			m_returnHasDecimal = false;
			m_currentDisplay = "0";
		}

		/// <summary>
		/// Checks if either unit is EMPTY_UNIT.
		/// </summary>
		bool AnyUnitIsEmpty()
		{
			return m_fromType == EMPTY_UNIT || m_toType == EMPTY_UNIT;
		}

		/// <summary>
		/// Calculates a new return value based on the current display value
		/// </summary>
		public void Calculate()
		{
			if (AnyUnitIsEmpty())
			{
				m_returnDisplay = m_currentDisplay;
				m_returnHasDecimal = m_currentHasDecimal;
				TrimString(ref m_returnDisplay);
				UpdateViewModel();
				return;
			}

			Dictionary<Unit, ConversionData> conversionTable = m_ratioMap[m_fromType];
			double returnValue = System.Convert.ToDouble(m_currentDisplay, CultureInfo.InvariantCulture);
			if (conversionTable[m_toType].ratio == 1.0 && conversionTable[m_toType].offset == 0.0)
			{
				m_returnDisplay = m_currentDisplay;
				m_returnHasDecimal = m_currentHasDecimal;
				TrimString(ref m_returnDisplay);
			}
			else
			{
				returnValue = Convert(returnValue, conversionTable[m_toType]);
				m_returnDisplay = RoundSignificant(returnValue, MAXIMUMDIGITSALLOWED);
				TrimString(ref m_returnDisplay);
				int numPreDecimal = (int)m_returnDisplay.size();
				if (m_returnDisplay.IndexOf('.') != m_returnDisplay.npos())
				{
					numPreDecimal = (int)m_returnDisplay.find('.');
				}
				if (returnValue < 0)
				{
					numPreDecimal--;
				}

				if (numPreDecimal > MAXIMUMDIGITSALLOWED || (returnValue != 0 && Math.Abs(returnValue) < MINIMUMDECIMALALLOWED))
				{
					m_returnDisplay = returnValue.ToString("e", CultureInfo.InvariantCulture);
				}
				else
				{
					returnValue = System.Convert.ToDouble(m_returnDisplay, CultureInfo.InvariantCulture);
					string returnString;
					if (m_currentDisplay.size() <= OPTIMALDIGITSALLOWED && Math.Abs(returnValue) >= OPTIMALDECIMALALLOWED)
					{
						returnString = RoundSignificant(returnValue, OPTIMALDIGITSALLOWED - Math.Min(numPreDecimal, OPTIMALDIGITSALLOWED));
					}
					else
					{
						returnString = RoundSignificant(returnValue, MAXIMUMDIGITSALLOWED - Math.Min(numPreDecimal, MAXIMUMDIGITSALLOWED));
					}
					m_returnDisplay = returnString;
					TrimString(ref m_returnDisplay);
				}
				m_returnHasDecimal = (m_returnDisplay.IndexOf('.') != m_returnDisplay.npos());
			}
			UpdateViewModel();
		}

		/// <summary>
		/// Trims out any trailing zeros or decimals in the given input string
		/// </summary>
		/// <param name="input">string to trim</param>
		void TrimString(ref string returnString)
		{
			if (returnString.IndexOf('.') == m_returnDisplay.npos())
			{
				return;
			}

			returnString = returnString.TrimEnd('0');
			returnString = returnString.TrimEnd('.');
		}

		/// <summary>
		/// Rounds the given double to the given number of significant digits
		/// </summary>
		/// <param name="num">input double</param>
		/// <param name="numSignificant">int number of significant digits to round to</param>
		string RoundSignificant(double num, int numSignificant)
		{
			return num.ToString($"F{numSignificant}", CultureInfo.InvariantCulture);
		}

		void UpdateCurrencySymbols()
		{
			if (m_currencyDataLoader != null && m_vmCurrencyCallback != null)
			{
				ICurrencyConverterDataLoader currencyDataLoader = GetCurrencyConverterDataLoader();
				KeyValuePair<string, string> currencySymbols = currencyDataLoader.GetCurrencySymbols(m_fromType, m_toType);
				KeyValuePair<string, string> currencyRatios = currencyDataLoader.GetCurrencyRatioEquality(m_fromType, m_toType);

				m_vmCurrencyCallback.CurrencySymbolsCallback(currencySymbols.Key, currencySymbols.Value);
				m_vmCurrencyCallback.CurrencyRatiosCallback(currencyRatios.Key, currencyRatios.Value);
			}
		}

		void UpdateViewModel()
		{
			m_vmCallback.DisplayCallback(m_currentDisplay, m_returnDisplay);
			m_vmCallback.SuggestedValueCallback(CalculateSuggested());
		}
	}
}
