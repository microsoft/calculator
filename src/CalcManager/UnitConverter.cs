// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CalcManager.UnitConversionManager
{
    public struct Unit : IEquatable<Unit>
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string AccessibleName { get; set; }
        public string Abbreviation { get; set; }
        public bool IsConversionSource { get; set; }
        public bool IsConversionTarget { get; set; }
        public bool IsWhimsical { get; set; }

        public Unit(int id, string name, string abbreviation, bool isConversionSource, bool isConversionTarget, bool isWhimsical)
        {
            Id = id;
            Name = name ?? string.Empty;
            AccessibleName = name ?? string.Empty;
            Abbreviation = abbreviation ?? string.Empty;
            IsConversionSource = isConversionSource;
            IsConversionTarget = isConversionTarget;
            IsWhimsical = isWhimsical;
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
            Id = id;
            Abbreviation = abbreviation ?? string.Empty;
            IsConversionSource = isConversionSource;
            IsConversionTarget = isConversionTarget;
            IsWhimsical = false;

            string nameValue1 = isRtlLanguage ? currencyName : countryName;
            string nameValue2 = isRtlLanguage ? countryName : currencyName;

            Name = $"{nameValue1} - {nameValue2}";
            AccessibleName = $"{nameValue1} {nameValue2}";
        }

        public static bool operator ==(Unit left, Unit right) => left.Id == right.Id;
        public static bool operator !=(Unit left, Unit right) => left.Id != right.Id;

        public override bool Equals(object obj) => obj is Unit u && this == u;
        public bool Equals(Unit other) => this == other;
        public override int GetHashCode() => Id.GetHashCode();
    }

    public struct Category : IEquatable<Category>
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public bool SupportsNegative { get; set; }

        public Category(int id, string name, bool supportsNegative)
        {
            Id = id;
            Name = name ?? string.Empty;
            SupportsNegative = supportsNegative;
        }

        public static bool operator ==(Category left, Category right) => left.Id == right.Id;
        public static bool operator !=(Category left, Category right) => left.Id != right.Id;

        public override bool Equals(object obj) => obj is Category c && this == c;
        public bool Equals(Category other) => this == other;
        public override int GetHashCode() => Id.GetHashCode();
    }

    public struct SuggestedValueIntermediate
    {
        public double Magnitude;
        public double Value;
        public Unit Type;
    }

    public struct ConversionData
    {
        public double Ratio;
        public double Offset;
        public bool OffsetFirst;

        public ConversionData(double ratio, double offset, bool offsetFirst)
        {
            Ratio = ratio;
            Offset = offset;
            OffsetFirst = offsetFirst;
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

    public interface IViewModelCurrencyCallback
    {
        void CurrencyDataLoadFinished(bool didLoad);
        void CurrencySymbolsCallback(string fromSymbol, string toSymbol);
        void CurrencyRatiosCallback(string ratioEquality, string accRatioEquality);
        void CurrencyTimestampCallback(string timestamp, bool isWeekOldData);
        void NetworkBehaviorChanged(int newBehavior);
    }

    public interface IConverterDataLoader
    {
        void LoadData();
        List<Category> GetOrderedCategories();
        List<Unit> GetOrderedUnits(Category c);
        Dictionary<Unit, ConversionData> LoadOrderedRatios(Unit u);
        bool SupportsCategory(Category target);
    }

    public interface ICurrencyConverterDataLoader
    {
        void SetViewModelCallback(IViewModelCurrencyCallback callback);
        Tuple<string, string> GetCurrencySymbols(Unit unit1, Unit unit2);
        Tuple<string, string> GetCurrencyRatioEquality(Unit unit1, Unit unit2);
        string GetCurrencyTimestamp();

        Task<bool> TryLoadDataFromCacheAsync();
        Task<bool> TryLoadDataFromWebAsync();
        Task<bool> TryLoadDataFromWebOverrideAsync();
    }

    public interface IUnitConverterVMCallback
    {
        void DisplayCallback(string from, string to);
        void SuggestedValueCallback(List<Tuple<string, Unit>> suggestedValues);
        void MaxDigitsReached();
    }

    public interface IUnitConverter
    {
        void Initialize();
        List<Category> GetCategories();
        Tuple<List<Unit>, Unit, Unit> SetCurrentCategory(Category input);
        Category GetCurrentCategory();
        void SetCurrentUnitTypes(Unit fromType, Unit toType);
        void SwitchActive(string newValue);
        bool IsSwitchedActive();
        string SaveUserPreferences();
        void RestoreUserPreferences(string userPreferences);
        void SendCommand(Command command);
        void SetViewModelCallback(IUnitConverterVMCallback newCallback);
        void SetViewModelCurrencyCallback(IViewModelCurrencyCallback newCallback);
        Task<Tuple<bool, string>> RefreshCurrencyRatios();
        void Calculate();
        void ResetCategoriesAndRatios();
    }

    public class UnitConverter : IUnitConverter
    {
        public static readonly Unit EMPTY_UNIT = new Unit(-1, "", "", true, true, false);

        private const uint EXPECTEDSERIALIZEDCATEGORYTOKENCOUNT = 3;
        private const uint EXPECTEDSERIALIZEDUNITTOKENCOUNT = 6;
        private const uint MAXIMUMDIGITSALLOWED = 15;
        private const uint OPTIMALDIGITSALLOWED = 7;

        private const char LEFTESCAPECHAR = '{';
        private const char RIGHTESCAPECHAR = '}';

        private static readonly double OPTIMALDECIMALALLOWED = 1e-6;
        private static readonly double MINIMUMDECIMALALLOWED = 1e-14;

        private static readonly Dictionary<char, string> quoteConversions = new Dictionary<char, string>
        {
            { '|', "{p}" },
            { '[', "{lc}" },
            { ']', "{rc}" },
            { ':', "{co}" },
            { ',', "{cm}" },
            { ';', "{sc}" },
            { LEFTESCAPECHAR, "{lb}" },
            { RIGHTESCAPECHAR, "{rb}" }
        };

        private static readonly Dictionary<string, char> unquoteConversions = new Dictionary<string, char>
        {
            { "{p}", '|' },
            { "{lc}", '[' },
            { "{rc}", ']' },
            { "{co}", ':' },
            { "{cm}", ',' },
            { "{sc}", ';' },
            { "{lb}", LEFTESCAPECHAR },
            { "{rb}", RIGHTESCAPECHAR }
        };

        private readonly IConverterDataLoader m_dataLoader;
        private readonly IConverterDataLoader m_currencyDataLoader;
        private IUnitConverterVMCallback m_vmCallback;
        private IViewModelCurrencyCallback m_vmCurrencyCallback;
        private List<Category> m_categories = new List<Category>();
        private readonly Dictionary<int, List<Unit>> m_categoryToUnits = new Dictionary<int, List<Unit>>();
        private readonly Dictionary<Unit, Dictionary<Unit, ConversionData>> m_ratioMap = new Dictionary<Unit, Dictionary<Unit, ConversionData>>();
        private Category m_currentCategory;
        private Unit m_fromType;
        private Unit m_toType;
        private string m_currentDisplay = "0";
        private string m_returnDisplay = "0";
        private bool m_currentHasDecimal;
        private bool m_returnHasDecimal;
        private bool m_switchedActive;

        public UnitConverter(IConverterDataLoader dataLoader)
            : this(dataLoader, null)
        {
        }

        public UnitConverter(IConverterDataLoader dataLoader, IConverterDataLoader currencyDataLoader)
        {
            m_dataLoader = dataLoader;
            m_currencyDataLoader = currencyDataLoader;
            ClearValues();
            ResetCategoriesAndRatios();
        }

        public void Initialize()
        {
            m_dataLoader?.LoadData();
        }

        private bool CheckLoad()
        {
            if (m_categories.Count == 0)
            {
                ResetCategoriesAndRatios();
            }
            return m_categories.Count > 0;
        }

        public List<Category> GetCategories()
        {
            CheckLoad();
            return m_categories;
        }

        public Tuple<List<Unit>, Unit, Unit> SetCurrentCategory(Category input)
        {
            if (m_currencyDataLoader != null && m_currencyDataLoader.SupportsCategory(input))
            {
                m_currencyDataLoader.LoadData();
            }

            var newUnitList = new List<Unit>();
            if (CheckLoad())
            {
                if (m_currentCategory.Id != input.Id)
                {
                    if (m_categoryToUnits.TryGetValue(m_currentCategory.Id, out var curList))
                    {
                        for (int i = 0; i < curList.Count; i++)
                        {
                            var unit = curList[i];
                            unit.IsConversionSource = (unit.Id == m_fromType.Id);
                            unit.IsConversionTarget = (unit.Id == m_toType.Id);
                            curList[i] = unit;
                        }
                    }

                    m_currentCategory = input;
                    if (!m_currentCategory.SupportsNegative && m_currentDisplay.StartsWith("-"))
                    {
                        m_currentDisplay = m_currentDisplay.Substring(1);
                    }
                }

                if (m_categoryToUnits.TryGetValue(input.Id, out var units))
                {
                    newUnitList = units;
                }
            }

            InitializeSelectedUnits();
            return Tuple.Create(newUnitList, m_fromType, m_toType);
        }

        public Category GetCurrentCategory() => m_currentCategory;

        public void SetCurrentUnitTypes(Unit fromType, Unit toType)
        {
            if (!CheckLoad()) return;

            if (m_fromType != fromType)
            {
                m_switchedActive = true;
            }

            m_fromType = fromType;
            m_toType = toType;
            Calculate();
            UpdateCurrencySymbols();
        }

        public void SwitchActive(string newValue)
        {
            if (!CheckLoad()) return;

            var tempUnit = m_fromType;
            m_fromType = m_toType;
            m_toType = tempUnit;

            bool tempDec = m_currentHasDecimal;
            m_currentHasDecimal = m_returnHasDecimal;
            m_returnHasDecimal = tempDec;

            m_returnDisplay = m_currentDisplay;
            m_currentDisplay = newValue;
            m_currentHasDecimal = m_currentDisplay.Contains(".");
            m_switchedActive = true;

            if (m_currencyDataLoader != null && m_vmCurrencyCallback != null)
            {
                var currencyDataLoader = GetCurrencyConverterDataLoader();
                var currencyRatios = currencyDataLoader.GetCurrencyRatioEquality(m_fromType, m_toType);
                m_vmCurrencyCallback.CurrencyRatiosCallback(currencyRatios.Item1, currencyRatios.Item2);
            }
        }

        public bool IsSwitchedActive() => m_switchedActive;

        public static string CategoryToString(Category c, string delimiter)
        {
            return Quote(c.Id.ToString()) + delimiter +
                   Quote(c.SupportsNegative ? "1" : "0") + delimiter +
                   Quote(c.Name) + delimiter;
        }

        public static List<string> StringToVector(string w, string delimiter, bool addRemainder = false)
        {
            var serializedTokens = new List<string>();
            if (string.IsNullOrEmpty(w)) return serializedTokens;

            int delimiterIndex = w.IndexOf(delimiter, StringComparison.Ordinal);
            int startIndex = 0;
            while (delimiterIndex != -1)
            {
                serializedTokens.Add(w.Substring(startIndex, delimiterIndex - startIndex));
                startIndex = delimiterIndex + delimiter.Length;
                delimiterIndex = w.IndexOf(delimiter, startIndex, StringComparison.Ordinal);
            }
            if (addRemainder)
            {
                serializedTokens.Add(w.Substring(startIndex));
            }
            return serializedTokens;
        }

        public static string UnitToString(Unit u, string delimiter)
        {
            return Quote(u.Id.ToString()) + delimiter +
                   Quote(u.Name) + delimiter +
                   Quote(u.Abbreviation) + delimiter +
                   (u.IsConversionSource ? "1" : "0") + delimiter +
                   (u.IsConversionTarget ? "1" : "0") + delimiter +
                   (u.IsWhimsical ? "1" : "0") + delimiter;
        }

        public static Unit StringToUnit(string w)
        {
            var tokenList = StringToVector(w, ";");
            var serializedUnit = new Unit
            {
                Id = int.Parse(Unquote(tokenList[0])),
                Name = Unquote(tokenList[1])
            };
            serializedUnit.AccessibleName = serializedUnit.Name;
            serializedUnit.Abbreviation = Unquote(tokenList[2]);
            serializedUnit.IsConversionSource = (tokenList[3] == "1");
            serializedUnit.IsConversionTarget = (tokenList[4] == "1");
            serializedUnit.IsWhimsical = (tokenList[5] == "1");
            return serializedUnit;
        }

        public static Category StringToCategory(string w)
        {
            var tokenList = StringToVector(w, ";");
            var serializedCategory = new Category
            {
                Id = int.Parse(Unquote(tokenList[0])),
                SupportsNegative = (tokenList[1] == "1"),
                Name = Unquote(tokenList[2])
            };
            return serializedCategory;
        }

        public void RestoreUserPreferences(string userPreferences)
        {
            if (string.IsNullOrEmpty(userPreferences)) return;

            var outerTokens = StringToVector(userPreferences, "|");
            if (outerTokens.Count != 3) return;

            var fromType = StringToUnit(outerTokens[0]);
            var toType = StringToUnit(outerTokens[1]);
            m_currentCategory = StringToCategory(outerTokens[2]);

            if (m_categoryToUnits.TryGetValue(m_currentCategory.Id, out var curUnits))
            {
                if (curUnits.Contains(fromType))
                {
                    m_fromType = fromType;
                }
                if (curUnits.Contains(toType))
                {
                    m_toType = toType;
                }
            }
        }

        public string SaveUserPreferences()
        {
            const string delimiter = ";";
            const string pipe = "|";
            return UnitToString(m_fromType, delimiter) + pipe +
                   UnitToString(m_toType, delimiter) + pipe +
                   CategoryToString(m_currentCategory, delimiter) + pipe;
        }

        public static string Quote(string s)
        {
            if (string.IsNullOrEmpty(s)) return string.Empty;
            var sb = new StringBuilder();
            foreach (char ch in s)
            {
                if (quoteConversions.TryGetValue(ch, out var converted))
                {
                    sb.Append(converted);
                }
                else
                {
                    sb.Append(ch);
                }
            }
            return sb.ToString();
        }

        public static string Unquote(string s)
        {
            if (string.IsNullOrEmpty(s)) return string.Empty;
            var sb = new StringBuilder();
            int i = 0;
            while (i < s.Length)
            {
                if (s[i] == LEFTESCAPECHAR)
                {
                    int end = s.IndexOf(RIGHTESCAPECHAR, i);
                    if (end == -1) break;
                    string sub = s.Substring(i, end - i + 1);
                    if (unquoteConversions.TryGetValue(sub, out char orig))
                    {
                        sb.Append(orig);
                    }
                    i = end + 1;
                }
                else
                {
                    sb.Append(s[i]);
                    i++;
                }
            }
            return sb.ToString();
        }

        public void SendCommand(Command command)
        {
            if (!CheckLoad()) return;

            bool clearFront;
            bool clearBack;
            if (command != Command.Negate && m_switchedActive)
            {
                ClearValues();
                m_switchedActive = false;
                clearFront = true;
                clearBack = false;
            }
            else
            {
                clearFront = (m_currentDisplay == "0");
                clearBack = ((m_currentHasDecimal && m_currentDisplay.Length - 1 >= MAXIMUMDIGITSALLOWED) ||
                             (!m_currentHasDecimal && m_currentDisplay.Length >= MAXIMUMDIGITSALLOWED));
            }

            switch (command)
            {
                case Command.Zero:
                    m_currentDisplay += '0';
                    break;
                case Command.One:
                    m_currentDisplay += '1';
                    break;
                case Command.Two:
                    m_currentDisplay += '2';
                    break;
                case Command.Three:
                    m_currentDisplay += '3';
                    break;
                case Command.Four:
                    m_currentDisplay += '4';
                    break;
                case Command.Five:
                    m_currentDisplay += '5';
                    break;
                case Command.Six:
                    m_currentDisplay += '6';
                    break;
                case Command.Seven:
                    m_currentDisplay += '7';
                    break;
                case Command.Eight:
                    m_currentDisplay += '8';
                    break;
                case Command.Nine:
                    m_currentDisplay += '9';
                    break;
                case Command.Decimal:
                    clearFront = false;
                    clearBack = false;
                    if (!m_currentHasDecimal)
                    {
                        m_currentDisplay += '.';
                        m_currentHasDecimal = true;
                    }
                    break;
                case Command.Backspace:
                    clearFront = false;
                    clearBack = false;
                    if ((!m_currentDisplay.StartsWith("-") && m_currentDisplay.Length > 1) || m_currentDisplay.Length > 2)
                    {
                        if (m_currentDisplay.EndsWith("."))
                        {
                            m_currentHasDecimal = false;
                        }
                        m_currentDisplay = m_currentDisplay.Substring(0, m_currentDisplay.Length - 1);
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
                    if (m_currentCategory.SupportsNegative)
                    {
                        if (m_currentDisplay.StartsWith("-"))
                        {
                            m_currentDisplay = m_currentDisplay.Substring(1);
                        }
                        else
                        {
                            m_currentDisplay = "-" + m_currentDisplay;
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
            }

            if (clearFront && m_currentDisplay.Length > 1 && m_currentDisplay.StartsWith("0") && m_currentDisplay[1] != '.')
            {
                m_currentDisplay = m_currentDisplay.Substring(1);
            }
            if (clearBack)
            {
                m_currentDisplay = m_currentDisplay.Substring(0, m_currentDisplay.Length - 1);
                m_vmCallback?.MaxDigitsReached();
            }

            Calculate();
        }

        public void SetViewModelCallback(IUnitConverterVMCallback newCallback)
        {
            m_vmCallback = newCallback;
            if (CheckLoad())
            {
                UpdateViewModel();
            }
        }

        public void SetViewModelCurrencyCallback(IViewModelCurrencyCallback newCallback)
        {
            m_vmCurrencyCallback = newCallback;
            var currencyDataLoader = GetCurrencyConverterDataLoader();
            currencyDataLoader?.SetViewModelCallback(newCallback);
        }

        public async Task<Tuple<bool, string>> RefreshCurrencyRatios()
        {
            var currencyDataLoader = GetCurrencyConverterDataLoader();
            bool didLoad = false;
            if (currencyDataLoader != null)
            {
                didLoad = await currencyDataLoader.TryLoadDataFromWebOverrideAsync();
            }

            string timestamp = currencyDataLoader?.GetCurrencyTimestamp() ?? string.Empty;
            return Tuple.Create(didLoad, timestamp);
        }

        private ICurrencyConverterDataLoader GetCurrencyConverterDataLoader()
        {
            return m_currencyDataLoader as ICurrencyConverterDataLoader;
        }

        private double Convert(double value, ConversionData conversionData)
        {
            if (conversionData.OffsetFirst)
            {
                return (value + conversionData.Offset) * conversionData.Ratio;
            }
            else
            {
                return (value * conversionData.Ratio) + conversionData.Offset;
            }
        }

        private List<Tuple<string, Unit>> CalculateSuggested()
        {
            if (m_currencyDataLoader != null && m_currencyDataLoader.SupportsCategory(m_currentCategory))
            {
                return new List<Tuple<string, Unit>>();
            }

            var returnVector = new List<Tuple<string, Unit>>();
            var intermediateVector = new List<SuggestedValueIntermediate>();
            var intermediateWhimsicalVector = new List<SuggestedValueIntermediate>();

            if (m_ratioMap.TryGetValue(m_fromType, out var ratios))
            {
                if (double.TryParse(m_currentDisplay, NumberStyles.Float, CultureInfo.InvariantCulture, out double curDisplayVal))
                {
                    foreach (var cur in ratios)
                    {
                        if (cur.Key != m_fromType && cur.Key != m_toType)
                        {
                            double convertedValue = Convert(curDisplayVal, cur.Value);
                            var newEntry = new SuggestedValueIntermediate
                            {
                                Magnitude = Math.Log10(Math.Abs(convertedValue)),
                                Value = convertedValue,
                                Type = cur.Key
                            };
                            if (newEntry.Type.IsWhimsical)
                            {
                                intermediateWhimsicalVector.Add(newEntry);
                            }
                            else
                            {
                                intermediateVector.Add(newEntry);
                            }
                        }
                    }
                }
            }

            intermediateVector.Sort((first, second) =>
            {
                if (Math.Abs(first.Magnitude) == Math.Abs(second.Magnitude))
                {
                    return second.Magnitude.CompareTo(first.Magnitude);
                }
                return Math.Abs(first.Magnitude).CompareTo(Math.Abs(second.Magnitude));
            });

            foreach (var entry in intermediateVector)
            {
                string roundedString;
                if (Math.Abs(entry.Value) < 100)
                {
                    roundedString = NumberFormattingUtils.RoundSignificantDigits(entry.Value, 2);
                }
                else if (Math.Abs(entry.Value) < 1000)
                {
                    roundedString = NumberFormattingUtils.RoundSignificantDigits(entry.Value, 1);
                }
                else
                {
                    roundedString = NumberFormattingUtils.RoundSignificantDigits(entry.Value, 0);
                }

                if (double.TryParse(roundedString, NumberStyles.Float, CultureInfo.InvariantCulture, out double parsed) && (parsed != 0.0 || m_currentCategory.SupportsNegative))
                {
                    NumberFormattingUtils.TrimTrailingZeros(ref roundedString);
                    returnVector.Add(Tuple.Create(roundedString, entry.Type));
                }
            }

            intermediateWhimsicalVector.Sort((first, second) =>
            {
                if (Math.Abs(first.Magnitude) == Math.Abs(second.Magnitude))
                {
                    return second.Magnitude.CompareTo(first.Magnitude);
                }
                return Math.Abs(first.Magnitude).CompareTo(Math.Abs(second.Magnitude));
            });

            var whimsicalReturnVector = new List<Tuple<string, Unit>>();
            foreach (var entry in intermediateWhimsicalVector)
            {
                string roundedString;
                if (Math.Abs(entry.Value) < 100)
                {
                    roundedString = NumberFormattingUtils.RoundSignificantDigits(entry.Value, 2);
                }
                else if (Math.Abs(entry.Value) < 1000)
                {
                    roundedString = NumberFormattingUtils.RoundSignificantDigits(entry.Value, 1);
                }
                else
                {
                    roundedString = NumberFormattingUtils.RoundSignificantDigits(entry.Value, 0);
                }

                if (double.TryParse(roundedString, NumberStyles.Float, CultureInfo.InvariantCulture, out double parsed) && parsed != 0.0)
                {
                    NumberFormattingUtils.TrimTrailingZeros(ref roundedString);
                    whimsicalReturnVector.Add(Tuple.Create(roundedString, entry.Type));
                }
            }

            if (whimsicalReturnVector.Count > 0)
            {
                returnVector.Add(whimsicalReturnVector[0]);
            }

            return returnVector;
        }

        public void ResetCategoriesAndRatios()
        {
            m_switchedActive = false;
            m_categories = m_dataLoader?.GetOrderedCategories() ?? new List<Category>();
            if (m_categories.Count == 0) return;

            m_currentCategory = m_categories[0];
            m_categoryToUnits.Clear();
            m_ratioMap.Clear();
            bool readyCategoryFound = false;

            foreach (var category in m_categories)
            {
                var activeDataLoader = GetDataLoaderForCategory(category);
                if (activeDataLoader == null) continue;

                var units = activeDataLoader.GetOrderedUnits(category);
                m_categoryToUnits[category.Id] = units;

                if (units.Count > 0)
                {
                    foreach (var u in units)
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

        private IConverterDataLoader GetDataLoaderForCategory(Category category)
        {
            if (m_currencyDataLoader != null && m_currencyDataLoader.SupportsCategory(category))
            {
                return m_currencyDataLoader;
            }
            return m_dataLoader;
        }

        private void InitializeSelectedUnits()
        {
            if (m_categoryToUnits.Count == 0) return;

            if (!m_categoryToUnits.TryGetValue(m_currentCategory.Id, out var curUnits) || curUnits.Count == 0)
            {
                return;
            }

            bool isFromUnitValid = m_fromType != EMPTY_UNIT && curUnits.Contains(m_fromType);
            bool isToUnitValid = m_toType != EMPTY_UNIT && curUnits.Contains(m_toType);

            if (isFromUnitValid && isToUnitValid) return;

            bool conversionSourceSet = false;
            bool conversionTargetSet = false;

            foreach (var cur in curUnits)
            {
                if (!conversionSourceSet && cur.IsConversionSource && !isFromUnitValid)
                {
                    m_fromType = cur;
                    conversionSourceSet = true;
                }

                if (!conversionTargetSet && cur.IsConversionTarget && !isToUnitValid)
                {
                    m_toType = cur;
                    conversionTargetSet = true;
                }

                if (conversionSourceSet && conversionTargetSet) return;
            }

            m_fromType = EMPTY_UNIT;
            m_toType = EMPTY_UNIT;
        }

        private void ClearValues()
        {
            m_currentHasDecimal = false;
            m_returnHasDecimal = false;
            m_currentDisplay = "0";
        }

        private bool AnyUnitIsEmpty()
        {
            return m_fromType == EMPTY_UNIT || m_toType == EMPTY_UNIT;
        }

        public void Calculate()
        {
            if (AnyUnitIsEmpty())
            {
                m_returnDisplay = m_currentDisplay;
                m_returnHasDecimal = m_currentHasDecimal;
                NumberFormattingUtils.TrimTrailingZeros(ref m_returnDisplay);
                UpdateViewModel();
                return;
            }

            if (!m_ratioMap.TryGetValue(m_fromType, out var conversionTable) ||
                !conversionTable.TryGetValue(m_toType, out var convData) ||
                (convData.Ratio == 1.0 && convData.Offset == 0.0))
            {
                m_returnDisplay = m_currentDisplay;
                m_returnHasDecimal = m_currentHasDecimal;
                NumberFormattingUtils.TrimTrailingZeros(ref m_returnDisplay);
            }
            else
            {
                if (double.TryParse(m_currentDisplay, NumberStyles.Float, CultureInfo.InvariantCulture, out double currentValue))
                {
                    double returnValue = Convert(currentValue, convData);
                    bool isCurrencyConverter = m_currencyDataLoader != null && m_currencyDataLoader.SupportsCategory(m_currentCategory);
                    if (isCurrencyConverter)
                    {
                        m_returnDisplay = NumberFormattingUtils.RoundSignificantDigits(returnValue, MAXIMUMDIGITSALLOWED);
                        NumberFormattingUtils.TrimTrailingZeros(ref m_returnDisplay);
                    }
                    else
                    {
                        uint numPreDecimal = NumberFormattingUtils.GetNumberDigitsWholeNumberPart(returnValue);
                        if (numPreDecimal > MAXIMUMDIGITSALLOWED || (returnValue != 0 && Math.Abs(returnValue) < MINIMUMDECIMALALLOWED))
                        {
                            m_returnDisplay = NumberFormattingUtils.ToScientificNumber(returnValue);
                        }
                        else
                        {
                            uint currentNumberSignificantDigits = NumberFormattingUtils.GetNumberDigits(m_currentDisplay);
                            uint precision;
                            if (Math.Abs(returnValue) < OPTIMALDECIMALALLOWED)
                            {
                                precision = MAXIMUMDIGITSALLOWED;
                            }
                            else
                            {
                                uint numberDigits = Math.Max(OPTIMALDIGITSALLOWED, Math.Min(MAXIMUMDIGITSALLOWED, currentNumberSignificantDigits));
                                precision = numberDigits > numPreDecimal ? numberDigits - numPreDecimal : 0;
                            }

                            m_returnDisplay = NumberFormattingUtils.RoundSignificantDigits(returnValue, precision);
                            NumberFormattingUtils.TrimTrailingZeros(ref m_returnDisplay);
                        }
                        m_returnHasDecimal = m_returnDisplay.Contains(".");
                    }
                }
            }

            UpdateViewModel();
        }

        private void UpdateCurrencySymbols()
        {
            if (m_currencyDataLoader != null && m_vmCurrencyCallback != null)
            {
                var currencyDataLoader = GetCurrencyConverterDataLoader();
                if (currencyDataLoader != null)
                {
                    var currencySymbols = currencyDataLoader.GetCurrencySymbols(m_fromType, m_toType);
                    var currencyRatios = currencyDataLoader.GetCurrencyRatioEquality(m_fromType, m_toType);
                    m_vmCurrencyCallback.CurrencySymbolsCallback(currencySymbols.Item1, currencySymbols.Item2);
                    m_vmCurrencyCallback.CurrencyRatiosCallback(currencyRatios.Item1, currencyRatios.Item2);
                }
            }
        }

        private void UpdateViewModel()
        {
            m_vmCallback?.DisplayCallback(m_currentDisplay, m_returnDisplay);
            m_vmCallback?.SuggestedValueCallback(CalculateSuggested());
        }
    }
}
