// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Command.h"
#include "UnitConverter.h"

using namespace concurrency;
using namespace std;
using namespace UnitConversionManager;

static constexpr uint32_t EXPECTEDSERIALIZEDTOKENCOUNT = 7;
static constexpr uint32_t EXPECTEDSERIALIZEDCONVERSIONDATATOKENCOUNT = 3;
static constexpr uint32_t EXPECTEDSERIALIZEDCATEGORYTOKENCOUNT = 3;
static constexpr uint32_t EXPECTEDSERIALIZEDUNITTOKENCOUNT = 6;
static constexpr uint32_t EXPECTEDSTATEDATATOKENCOUNT = 5;
static constexpr uint32_t EXPECTEDMAPCOMPONENTTOKENCOUNT = 2;

static constexpr int32_t MAXIMUMDIGITSALLOWED = 15;
static constexpr int32_t OPTIMALDIGITSALLOWED = 7;

static constexpr wchar_t LEFTESCAPECHAR = L'{';
static constexpr wchar_t RIGHTESCAPECHAR = L'}';

static const double OPTIMALDECIMALALLOWED = pow(10, -1 * (OPTIMALDIGITSALLOWED - 1));
static const double MINIMUMDECIMALALLOWED = pow(10, -1 * (MAXIMUMDIGITSALLOWED - 1));

unordered_map<wchar_t, wstring> quoteConversions;
unordered_map<wstring, wchar_t> unquoteConversions;

/// <summary>
/// Constructor, sets up all the variables and requires a configLoader
/// </summary>
/// <param name="dataLoader">An instance of the IConverterDataLoader interface which we use to read in category/unit names and conversion data</param>
UnitConverter::UnitConverter(_In_ const shared_ptr<IConverterDataLoader>& dataLoader) :
    UnitConverter::UnitConverter(dataLoader, nullptr)
{
}

/// <summary>
/// Constructor, sets up all the variables and requires two configLoaders
/// </summary>
/// <param name="dataLoader">An instance of the IConverterDataLoader interface which we use to read in category/unit names and conversion data</param>
/// <param name="currencyDataLoader">An instance of the IConverterDataLoader interface, specialized for loading currency data from an internet service</param>
UnitConverter::UnitConverter(_In_ const shared_ptr<IConverterDataLoader>& dataLoader, _In_ const shared_ptr<IConverterDataLoader>& currencyDataLoader)
{
    m_dataLoader = dataLoader;
    m_currencyDataLoader = currencyDataLoader;
    //declaring the delimiter character conversion map
    quoteConversions[L'|'] = L"{p}";
    quoteConversions[L'['] = L"{lc}";
    quoteConversions[L']'] = L"{rc}";
    quoteConversions[L':'] = L"{co}";
    quoteConversions[L','] = L"{cm}";
    quoteConversions[L';'] = L"{sc}";
    quoteConversions[LEFTESCAPECHAR] = L"{lb}";
    quoteConversions[RIGHTESCAPECHAR] = L"{rb}";
    unquoteConversions[L"{p}"] = L'|';
    unquoteConversions[L"{lc}"] = L'[';
    unquoteConversions[L"{rc}"] = L']';
    unquoteConversions[L"{co}"] = L':';
    unquoteConversions[L"{cm}"] = L',';
    unquoteConversions[L"{sc}"] = L';';
    unquoteConversions[L"{lb}"] = LEFTESCAPECHAR;
    unquoteConversions[L"{rb}"] = RIGHTESCAPECHAR;
    Reset();
}

void UnitConverter::Initialize()
{
    m_dataLoader->LoadData();
}

bool UnitConverter::CheckLoad()
{
    if (m_categories.empty())
    {
        Reset();
    }
    return !m_categories.empty();
}

/// <summary>
/// Returns a list of the categories in use by this converter
/// </summary>
vector<Category> UnitConverter::GetCategories()
{
    CheckLoad();
    return m_categories;
}

/// <summary>
/// Sets the current category in use by this converter, 
/// and returns a list of unit types that exist under the given category.
/// </summary>
/// <param name="input">Category struct which we are setting</param>
CategorySelectionInitializer UnitConverter::SetCurrentCategory(const Category& input)
{
    if (m_currencyDataLoader != nullptr && m_currencyDataLoader->SupportsCategory(input))
    {
        m_currencyDataLoader->LoadData();
    }

    vector<Unit> newUnitList{};
    if (CheckLoad())
    {
        if (m_currentCategory.id != input.id)
        {
            vector<Unit>& unitVector = m_categoryToUnits[m_currentCategory];
            for (unsigned int i = 0; i < unitVector.size(); i++)
            {
                if (unitVector[i].id == m_fromType.id)
                {
                    unitVector[i].isConversionSource = true;
                }
                else
                {
                    unitVector[i].isConversionSource = false;
                }
                if (unitVector[i].id == m_toType.id)
                {
                    unitVector[i].isConversionTarget = true;
                }
                else
                {
                    unitVector[i].isConversionTarget = false;
                }
            }
            m_currentCategory = input;
            if (!m_currentCategory.supportsNegative && m_currentDisplay.front() == L'-')
            {
                m_currentDisplay.erase(0, 1);
            }
        }

        newUnitList = m_categoryToUnits[input];
    }

    InitializeSelectedUnits();
    return make_tuple(newUnitList, m_fromType, m_toType);
}

/// <summary>
/// Gets the category currently being used
/// </summary>
Category UnitConverter::GetCurrentCategory()
{
    return m_currentCategory;
}

/// <summary>
/// Sets the current unit types to be used, indicates a likely change in the
/// display values, so we re-calculate and callback the updated values
/// </summary>
/// <param name="fromType">Unit struct which the user is modifying</param>
/// <param name="toType">Unit struct we are converting to</param>
void UnitConverter::SetCurrentUnitTypes(const Unit& fromType, const Unit& toType)
{
    if (CheckLoad())
    {
        m_fromType = fromType;
        m_toType = toType;
        Calculate();

        UpdateCurrencySymbols();
        UpdateViewModel();
    }
}

/// <summary>
/// Switches the active field, indicating that we are now entering data into
/// what was originally the return field, and storing results into what was
/// originally the current field. We swap appropriate values, 
/// but do not callback, as values have not changed.
/// </summary>
/// <param name="newValue">
/// wstring representing the value user had in the field they've just activated.
/// We use this to handle cases where the front-end may choose to trim more digits
/// than we have been storing internally, in which case appending will not function
/// as expected without the use of this parameter.
/// </param>
void UnitConverter::SwitchActive(const wstring& newValue)
{
    if (CheckLoad())
    {
        swap(m_fromType, m_toType);
        swap(m_currentHasDecimal, m_returnHasDecimal);
        m_returnDisplay = m_currentDisplay;
        m_currentDisplay = newValue;
        m_currentHasDecimal = (m_currentDisplay.find(L'.') != m_currentDisplay.npos);
        m_switchedActive = true;

        if (m_currencyDataLoader != nullptr && m_vmCurrencyCallback != nullptr)
        {
            shared_ptr<ICurrencyConverterDataLoader> currencyDataLoader = GetCurrencyConverterDataLoader();
            const pair<wstring, wstring> currencyRatios = currencyDataLoader->GetCurrencyRatioEquality(m_fromType, m_toType);

            m_vmCurrencyCallback->CurrencyRatiosCallback(currencyRatios.first, currencyRatios.second);
        }
    }
}

wstring UnitConverter::CategoryToString(const Category& c, const wchar_t * delimiter)
{
    wstringstream out(wstringstream::out);
    out << Quote(std::to_wstring(c.id)) << delimiter << Quote(std::to_wstring(c.supportsNegative)) << delimiter << Quote(c.name) << delimiter;
    return out.str();
}

vector<wstring> UnitConverter::StringToVector(const wstring& w, const wchar_t * delimiter, bool addRemainder)
{
    size_t delimiterIndex = w.find(delimiter);
    size_t startIndex = 0;
    vector<wstring> serializedTokens = vector<wstring>();
    while (delimiterIndex != w.npos)
    {
        serializedTokens.push_back(w.substr(startIndex, delimiterIndex - startIndex));
        startIndex = delimiterIndex + (int)wcslen(delimiter);
        delimiterIndex = w.find(delimiter, startIndex);
    }
    if (addRemainder)
    {
        delimiterIndex = w.size();
        serializedTokens.push_back(w.substr(startIndex, delimiterIndex - startIndex));
    }
    return serializedTokens;
}

Category UnitConverter::StringToCategory(const wstring& w)
{
    vector<wstring> tokenList = StringToVector(w, L";");
    assert(tokenList.size() == EXPECTEDSERIALIZEDCATEGORYTOKENCOUNT);
    Category serializedCategory;
    serializedCategory.id = _wtoi(Unquote(tokenList[0]).c_str());
    serializedCategory.supportsNegative = (tokenList[1].compare(L"1") == 0);
    serializedCategory.name = Unquote(tokenList[2]);
    return serializedCategory;
}

wstring UnitConverter::UnitToString(const Unit& u, const wchar_t * delimiter)
{
    wstringstream out(wstringstream::out);
    out << Quote(std::to_wstring(u.id)) << delimiter << Quote(u.name) << delimiter << Quote(u.abbreviation) << delimiter << std::to_wstring(u.isConversionSource) << delimiter << std::to_wstring(u.isConversionTarget) << delimiter << std::to_wstring(u.isWhimsical) << delimiter;
    return out.str();
}

Unit UnitConverter::StringToUnit(const wstring& w)
{
    vector<wstring> tokenList = StringToVector(w, L";");
    assert(tokenList.size() == EXPECTEDSERIALIZEDUNITTOKENCOUNT);
    Unit serializedUnit;
    serializedUnit.id = _wtoi(Unquote(tokenList[0]).c_str());
    serializedUnit.name = Unquote(tokenList[1]);
    serializedUnit.accessibleName = serializedUnit.name;
    serializedUnit.abbreviation = Unquote(tokenList[2]);
    serializedUnit.isConversionSource = (tokenList[3].compare(L"1") == 0);
    serializedUnit.isConversionTarget = (tokenList[4].compare(L"1") == 0);
    serializedUnit.isWhimsical = (tokenList[5].compare(L"1") == 0);
    return serializedUnit;
}

ConversionData UnitConverter::StringToConversionData(const wstring& w)
{
    vector<wstring> tokenList = StringToVector(w, L";");
    assert(tokenList.size() == EXPECTEDSERIALIZEDCONVERSIONDATATOKENCOUNT);
    ConversionData serializedConversionData;
    serializedConversionData.ratio = stod(Unquote(tokenList[0]).c_str());
    serializedConversionData.offset = stod(Unquote(tokenList[1]).c_str());
    serializedConversionData.offsetFirst = (tokenList[2].compare(L"1") == 0);
    return serializedConversionData;
}

wstring UnitConverter::ConversionDataToString(ConversionData d, const wchar_t * delimiter)
{
    wstringstream out(wstringstream::out);
    out.precision(32);
    out << fixed << d.ratio;
    wstring ratio = out.str();
    out.str(L"");
    out << fixed << d.offset;
    wstring offset = out.str();
    out.str(L"");
    TrimString(ratio);
    TrimString(offset);
    out << Quote(ratio) << delimiter << Quote(offset) << delimiter << std::to_wstring(d.offsetFirst) << delimiter;
    return out.str();
}

/// <summary>
/// Serializes the data in the converter and returns it as a string
/// </summary>
wstring UnitConverter::Serialize()
{
    if (CheckLoad())
    {
        wstringstream out(wstringstream::out);
        const wchar_t * delimiter = L";";

        out << UnitToString(m_fromType, delimiter) << "|";
        out << UnitToString(m_toType, delimiter) << "|";
        out << CategoryToString(m_currentCategory, delimiter) << "|";
        out << std::to_wstring(m_currentHasDecimal) << delimiter << std::to_wstring(m_returnHasDecimal) << delimiter << std::to_wstring(m_switchedActive) << delimiter;
        out << m_currentDisplay << delimiter << m_returnDisplay << delimiter << "|";
        wstringstream categoryString(wstringstream::out);
        wstringstream categoryToUnitString(wstringstream::out);
        wstringstream unitToUnitToDoubleString(wstringstream::out);
        for (const Category& c : m_categories)
        {
            categoryString << CategoryToString(c, delimiter) << ",";
        }

        for (const auto& cur : m_categoryToUnits)
        {
            categoryToUnitString << CategoryToString(cur.first, delimiter) << "[";
            for (const Unit& u : cur.second)
            {
                categoryToUnitString << UnitToString(u, delimiter) << ",";
            }
            categoryToUnitString << "[" << "]";
        }

        for (const auto& cur : m_ratioMap)
        {
            unitToUnitToDoubleString << UnitToString(cur.first, delimiter) << "[";
            for (const auto& curConversion : cur.second)
            {
                unitToUnitToDoubleString << UnitToString(curConversion.first, delimiter) << ":";
                unitToUnitToDoubleString << ConversionDataToString(curConversion.second, delimiter) << ":,";
            }
            unitToUnitToDoubleString << "[" << "]";
        }

        out << categoryString.str() << "|";
        out << categoryToUnitString.str() << "|";
        out << unitToUnitToDoubleString.str() << "|";
        wstring test = out.str();
        return test;
    }
    else
    {
        return wstring();
    }
}

/// <summary>
/// De-Serializes the data in the converter from a string
/// </summary>
/// <param name="serializedData">wstring holding the serialized data. If it does not have expected number of parameters, we will ignore it</param>
void UnitConverter::DeSerialize(const wstring& serializedData)
{
    Reset();
    if (!serializedData.empty())
    {
        vector<wstring> outerTokens = StringToVector(serializedData, L"|");
        assert(outerTokens.size() == EXPECTEDSERIALIZEDTOKENCOUNT);
        m_fromType = StringToUnit(outerTokens[0]);
        m_toType = StringToUnit(outerTokens[1]);
        m_currentCategory = StringToCategory(outerTokens[2]);
        vector<wstring> stateDataTokens = StringToVector(outerTokens[3], L";");
        assert(stateDataTokens.size() == EXPECTEDSTATEDATATOKENCOUNT);
        m_currentHasDecimal = (stateDataTokens[0].compare(L"1") == 0);
        m_returnHasDecimal = (stateDataTokens[1].compare(L"1") == 0);
        m_switchedActive = (stateDataTokens[2].compare(L"1") == 0);
        m_currentDisplay = stateDataTokens[3];
        m_returnDisplay = stateDataTokens[4];
        vector<wstring> categoryListTokens = StringToVector(outerTokens[4], L",");
        for (wstring token : categoryListTokens)
        {
            m_categories.push_back(StringToCategory(token));
        }
        vector<wstring> unitVectorTokens = StringToVector(outerTokens[5], L"]");
        for (wstring unitVector : unitVectorTokens)
        {
            vector<wstring> mapcomponents = StringToVector(unitVector, L"[");
            assert(mapcomponents.size() == EXPECTEDMAPCOMPONENTTOKENCOUNT);
            Category key = StringToCategory(mapcomponents[0]);
            vector<wstring> units = StringToVector(mapcomponents[1], L",");
            for (wstring unit : units)
            {
                m_categoryToUnits[key].push_back(StringToUnit(unit));
            }
        }
        vector<wstring> ratioMapTokens = StringToVector(outerTokens[6], L"]");
        for (wstring token : ratioMapTokens)
        {
            vector<wstring> ratioMapComponentTokens = StringToVector(token, L"[");
            assert(ratioMapComponentTokens.size() == EXPECTEDMAPCOMPONENTTOKENCOUNT);
            Unit key = StringToUnit(ratioMapComponentTokens[0]);
            vector<wstring> ratioMapList = StringToVector(ratioMapComponentTokens[1], L",");
            for (wstring subtoken : ratioMapList)
            {
                vector<wstring> ratioMapSubComponentTokens = StringToVector(subtoken, L":");
                assert(ratioMapSubComponentTokens.size() == EXPECTEDMAPCOMPONENTTOKENCOUNT);
                Unit subkey = StringToUnit(ratioMapSubComponentTokens[0]);
                ConversionData conversion = StringToConversionData(ratioMapSubComponentTokens[1]);
                m_ratioMap[key][subkey] = conversion;
            }
        }
        UpdateViewModel();
    }
}

/// <summary>
/// De-Serializes the data in the converter from a string
/// </summary>
/// <param name="userPreferences">wstring holding the serialized data. If it does not have expected number of parameters, we will ignore it</param>
void UnitConverter::RestoreUserPreferences(const wstring& userPreferences)
{
    if (!userPreferences.empty())
    {
        vector<wstring> outerTokens = StringToVector(userPreferences, L"|");
        if (outerTokens.size() == 3)
        {
            m_fromType = StringToUnit(outerTokens[0]);
            m_toType = StringToUnit(outerTokens[1]);
            m_currentCategory = StringToCategory(outerTokens[2]);
        }
    }
}

/// <summary>
/// Serializes the Category and Associated Units in the converter and returns it as a string
/// </summary>    
wstring UnitConverter::SaveUserPreferences()
{
    wstringstream out(wstringstream::out);
    const wchar_t * delimiter = L";";

    out << UnitToString(m_fromType, delimiter) << "|";
    out << UnitToString(m_toType, delimiter) << "|";
    out << CategoryToString(m_currentCategory, delimiter) << "|";
    wstring test = out.str();
    return test;
}

/// <summary>
/// Sanitizes the input string, escape quoting any symbols we rely on for our delimiters, and returns the sanitized string.
/// </summary>
/// <param name="s">wstring to be sanitized</param>
wstring UnitConverter::Quote(const wstring& s)
{
    wstringstream quotedString(wstringstream::out);

    //Iterate over the delimiter characters we need to quote
    wstring::const_iterator cursor = s.begin();
    while(cursor != s.end())
    {
        if (quoteConversions.find(*cursor) != quoteConversions.end())
        {
            quotedString << quoteConversions[*cursor];
        }
        else
        {
            quotedString << *cursor;
        }
        ++cursor;
    }
    return quotedString.str();
}

/// <summary>
/// Unsanitizes the sanitized input string, returning it to its original contents before we had quoted it.
/// </summary>
/// <param name="s">wstring to be unsanitized</param>
wstring UnitConverter::Unquote(const wstring& s)
{
    wstringstream quotedSubString(wstringstream::out);
    wstringstream unquotedString(wstringstream::out);
    wstring::const_iterator cursor = s.begin();
    while(cursor != s.end())
    {
        if(*cursor == LEFTESCAPECHAR)
        {
            quotedSubString.str(L"");
            while (cursor != s.end() && *cursor != RIGHTESCAPECHAR)
            {
                quotedSubString << *cursor;
                ++cursor;
            }
            if (cursor == s.end())
            {
                //badly formatted
                break;
            }
            else
            {
                quotedSubString << *cursor;
                unquotedString << unquoteConversions[quotedSubString.str()];
            }
        }
        else
        {
            unquotedString << *cursor;
        }
        ++cursor;
    }
    return unquotedString.str();
}

/// <summary>
/// Handles inputs to the converter from the view-model, corresponding to a given button or keyboard press
/// </summary>
/// <param name="command">Command enum representing the command that was entered</param>
void UnitConverter::SendCommand(Command command)
{
    if (CheckLoad())
    {
        //TODO: Localization of characters
        bool clearFront = false;
        if (m_currentDisplay == L"0")
        {
            clearFront = true;
        }
        bool clearBack = false;
        if ((m_currentHasDecimal && m_currentDisplay.size() - 1 >= MAXIMUMDIGITSALLOWED) || (!m_currentHasDecimal && m_currentDisplay.size() >= MAXIMUMDIGITSALLOWED))
        {
            clearBack = true;
        }
        if (command != Command::Negate && m_switchedActive)
        {
            ClearValues();
            m_switchedActive = false;
            clearFront = true;
            clearBack = false;
        }
        switch (command)
        {
        case Command::Zero:
            m_currentDisplay += L"0";
            break;

        case Command::One:
            m_currentDisplay += L"1";
            break;

        case Command::Two:
            m_currentDisplay += L"2";
            break;

        case Command::Three:
            m_currentDisplay += L"3";
            break;

        case Command::Four:
            m_currentDisplay += L"4";
            break;

        case Command::Five:
            m_currentDisplay += L"5";
            break;

        case Command::Six:
            m_currentDisplay += L"6";
            break;

        case Command::Seven:
            m_currentDisplay += L"7";
            break;

        case Command::Eight:
            m_currentDisplay += L"8";
            break;

        case Command::Nine:
            m_currentDisplay += L"9";
            break;

        case Command::Decimal:
            clearFront = false;
            clearBack = false;
            if (!m_currentHasDecimal)
            {
                m_currentDisplay += L".";
                m_currentHasDecimal = true;
            }
            break;

        case Command::Backspace:
            clearFront = false;
            clearBack = false;
            if ((m_currentDisplay.front() != '-' && m_currentDisplay.size() > 1) || m_currentDisplay.size() > 2)
            {
                if (m_currentDisplay.back() == '.')
                {
                    m_currentHasDecimal = false;
                }
                m_currentDisplay.pop_back();
            }
            else
            {
                m_currentDisplay = L"0";
                m_currentHasDecimal = false;
            }
            break;

        case Command::Negate:
            clearFront = false;
            clearBack = false;
            if (m_currentCategory.supportsNegative)
            {
                if (m_currentDisplay.front() == '-')
                {
                    m_currentDisplay.erase(0, 1);
                }
                else
                {
                    m_currentDisplay.insert(0, 1, '-');
                }
            }
            break;

        case Command::Clear:
            clearFront = false;
            clearBack = false;
            ClearValues();
            break;

        case Command::Reset:
            clearFront = false;
            clearBack = false;
            ClearValues();
            Reset();
            break;

        default:
            break;
        }
        

        if (clearFront)
        {
            m_currentDisplay.erase(0, 1);
        }
        if (clearBack)
        {
            m_currentDisplay.erase(m_currentDisplay.size() - 1, 1);
            m_vmCallback->MaxDigitsReached();
        }

        Calculate();

        UpdateViewModel();
    }
}

/// <summary>
/// Sets the callback interface to send display update calls to
/// </summary>
/// <param name="newCallback">instance of IDisplayCallback interface that receives our update calls</param>
void UnitConverter::SetViewModelCallback(_In_ const shared_ptr<IUnitConverterVMCallback>& newCallback)
{
    m_vmCallback = newCallback;
    if (CheckLoad())
    {
        UpdateViewModel();
    }
}

void UnitConverter::SetViewModelCurrencyCallback(_In_ const shared_ptr<IViewModelCurrencyCallback>& newCallback)
{
    m_vmCurrencyCallback = newCallback;

    shared_ptr<ICurrencyConverterDataLoader> currencyDataLoader = GetCurrencyConverterDataLoader();
    if (currencyDataLoader != nullptr)
    {
        currencyDataLoader->SetViewModelCallback(newCallback);
    }
}

task<pair<bool, wstring>> UnitConverter::RefreshCurrencyRatios()
{
    shared_ptr<ICurrencyConverterDataLoader> currencyDataLoader = GetCurrencyConverterDataLoader();
    return create_task([this, currencyDataLoader]()
    {
        if (currencyDataLoader != nullptr)
        {
            return currencyDataLoader->TryLoadDataFromWebOverrideAsync();
        }
        else
        {
            return task_from_result(false);
        }
    }).then([this, currencyDataLoader](bool didLoad)
    {
        wstring timestamp = L"";
        if (currencyDataLoader != nullptr)
        {
            timestamp = currencyDataLoader->GetCurrencyTimestamp();
        }

        return make_pair(didLoad, timestamp);
    }, task_continuation_context::use_default());
}

shared_ptr<ICurrencyConverterDataLoader> UnitConverter::GetCurrencyConverterDataLoader()
{
    return dynamic_pointer_cast<ICurrencyConverterDataLoader>(m_currencyDataLoader);
}

/// <summary>
/// Converts a double value into another unit type, currently by multiplying by the given double ratio
/// </summary>
/// <param name="value">double input value to convert</param>
/// <param name="ratio">double conversion ratio to use</param>
double UnitConverter::Convert(double value, ConversionData conversionData)
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
vector<tuple<wstring, Unit>> UnitConverter::CalculateSuggested()
{
    if (m_currencyDataLoader != nullptr && m_currencyDataLoader->SupportsCategory(m_currentCategory))
    {
        return vector<tuple<wstring, Unit>>();
    }

    vector<tuple<wstring, Unit>> returnVector;
    vector<SuggestedValueIntermediate> intermediateVector;
    vector<SuggestedValueIntermediate> intermediateWhimsicalVector;
    unordered_map<Unit, ConversionData, UnitHash> ratios = m_ratioMap[m_fromType];
    //Calculate converted values for every other unit type in this category, along with their magnitude
    for (const auto& cur : ratios)
    {
        if (cur.first != m_fromType && cur.first != m_toType)
        {
            double convertedValue = Convert(stod(m_currentDisplay), cur.second);
            SuggestedValueIntermediate newEntry;
            newEntry.magnitude = log10(convertedValue);
            newEntry.value = convertedValue;
            newEntry.type = cur.first;
            if(newEntry.type.isWhimsical == false)
                intermediateVector.push_back(newEntry);
            else
                intermediateWhimsicalVector.push_back(newEntry);
        }
    }

    //Sort the resulting list by absolute magnitude, breaking ties by choosing the positive value
    sort(intermediateVector.begin(), intermediateVector.end(), []
    (SuggestedValueIntermediate first, SuggestedValueIntermediate second)
    { 
        if (abs(first.magnitude) == abs(second.magnitude)) 
        { 
            return first.magnitude > second.magnitude; 
        } 
        else 
        { 
            return abs(first.magnitude) < abs(second.magnitude);
        }
    });

    //Now that the list is sorted, iterate over it and populate the return vector with properly rounded and formatted return strings
    for (const auto& entry : intermediateVector)
    {
        wstring roundedString;
        if (abs(entry.value) < 100)
        {
            roundedString = RoundSignificant(entry.value, 2);
        }
        else if (abs(entry.value) < 1000)
        {
        roundedString = RoundSignificant(entry.value, 1);
        }
        else
        {
            roundedString = RoundSignificant(entry.value, 0);
        }
        if (stod(roundedString) != 0.0 || m_currentCategory.supportsNegative)
        {
            TrimString(roundedString);
            returnVector.push_back(make_tuple(roundedString, entry.type));
        }
    }

    // The Whimsicals are determined differently
    //Sort the resulting list by absolute magnitude, breaking ties by choosing the positive value
    sort(intermediateWhimsicalVector.begin(), intermediateWhimsicalVector.end(), []
    (SuggestedValueIntermediate first, SuggestedValueIntermediate second)
    {
        if (abs(first.magnitude) == abs(second.magnitude))
        {
            return first.magnitude > second.magnitude;
        }
        else
        {
            return abs(first.magnitude) < abs(second.magnitude);
        }
    });

    //Now that the list is sorted, iterate over it and populate the return vector with properly rounded and formatted return strings
    vector<tuple<wstring, Unit>> whimsicalReturnVector;

    for (const auto& entry : intermediateWhimsicalVector)
    {
        wstring roundedString;
        if (abs(entry.value) < 100)
        {
            roundedString = RoundSignificant(entry.value, 2);
        }
        else if (abs(entry.value) < 1000)
        {
            roundedString = RoundSignificant(entry.value, 1);
        }
        else
        {
            roundedString = RoundSignificant(entry.value, 0);
        }

        // How to work out which is the best whimsical value to add to the vector?
        if (stod(roundedString) != 0.0)
        {
            TrimString(roundedString);
            whimsicalReturnVector.push_back(make_tuple(roundedString, entry.type));
        }
    }
    // Pickup the 'best' whimsical value - currently the first one
    if (whimsicalReturnVector.size() != 0)
    {
        returnVector.push_back(whimsicalReturnVector.at(0));
    }

    //

    return returnVector;
}

/// <summary>
/// Resets the converter to its initial state
/// </summary>
void UnitConverter::Reset()
{
    m_categories = m_dataLoader->LoadOrderedCategories();

    ClearValues();
    m_switchedActive = false;

    if (!m_categories.empty())
    {
        m_currentCategory = m_categories[0];

        m_categoryToUnits.clear();
        m_ratioMap.clear();
        bool readyCategoryFound = false;
        for (const Category& category : m_categories)
        {
            shared_ptr<IConverterDataLoader> activeDataLoader = GetDataLoaderForCategory(category);
            if (activeDataLoader == nullptr)
            {
                // The data loader is different depending on the category, e.g. currency data loader
                // is different from the static data loader.
                // If there is no data loader for this category, continue.
                continue;
            }

            vector<Unit> units = activeDataLoader->LoadOrderedUnits(category);
            m_categoryToUnits[category] = units;

            // Just because the units are empty, doesn't mean the user can't select this category,
            // we just want to make sure we don't let an unready category be the default.
            if (!units.empty())
            {
                for (Unit u : units)
                {
                    m_ratioMap[u] = activeDataLoader->LoadOrderedRatios(u);
                }

                if (!readyCategoryFound)
                {
                    m_currentCategory = category;
                    readyCategoryFound = true;
                }
            }
        }

        InitializeSelectedUnits();
        Calculate();
    }
}

/// <summary>
/// Sets the active data loader based on the input category.
/// </summary>
shared_ptr<IConverterDataLoader> UnitConverter::GetDataLoaderForCategory(const Category& category)
{
    if (m_currencyDataLoader != nullptr && m_currencyDataLoader->SupportsCategory(category))
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
void UnitConverter::InitializeSelectedUnits()
{

    if (m_categoryToUnits.empty())
    {
        return;
    }

    auto itr = m_categoryToUnits.find(m_currentCategory);
    if (itr == m_categoryToUnits.end())
    {
        return;
    }

    vector<Unit> curUnits = itr->second;
    if (!curUnits.empty())
    {
        bool conversionSourceSet = false;
        bool conversionTargetSet = false;
        for (const Unit& cur : curUnits)
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
void UnitConverter::ClearValues()
{
    m_currentHasDecimal = false;
    m_returnHasDecimal = false;
    m_currentDisplay = L"0";
}

/// <summary>
/// Checks if either unit is EMPTY_UNIT.
/// </summary>
bool UnitConverter::AnyUnitIsEmpty()
{
    return m_fromType == EMPTY_UNIT || m_toType == EMPTY_UNIT;
}

/// <summary>
/// Calculates a new return value based on the current display value
/// </summary>
void UnitConverter::Calculate()
{
    unordered_map<Unit, ConversionData, UnitHash> conversionTable = m_ratioMap[m_fromType];
    double returnValue = stod(m_currentDisplay);
    if (AnyUnitIsEmpty() || (conversionTable[m_toType].ratio == 1.0 && conversionTable[m_toType].offset == 0.0))
    {
        m_returnDisplay = m_currentDisplay;
        TrimString(m_returnDisplay);
    }
    else
    {
        returnValue = Convert(returnValue, conversionTable[m_toType]);
        m_returnDisplay = RoundSignificant(returnValue, MAXIMUMDIGITSALLOWED);
        TrimString(m_returnDisplay);
        int numPreDecimal = (int)m_returnDisplay.size();
        if (m_returnDisplay.find(L'.') != m_returnDisplay.npos)
        {
            numPreDecimal = (int)m_returnDisplay.find(L'.');
        }
        if (returnValue < 0)
        {
            numPreDecimal--;
        }

        if (numPreDecimal > MAXIMUMDIGITSALLOWED || (returnValue != 0 && abs(returnValue) < MINIMUMDECIMALALLOWED))
        {
            wstringstream out(wstringstream::out);
            out << scientific << returnValue;
            m_returnDisplay = out.str();
        }
        else
        {
            returnValue = stod(m_returnDisplay);
            wstring returnString;
            if (m_currentDisplay.size() <= OPTIMALDIGITSALLOWED && abs(returnValue) >= OPTIMALDECIMALALLOWED)
            {
                returnString = RoundSignificant(returnValue, OPTIMALDIGITSALLOWED - min(numPreDecimal, OPTIMALDIGITSALLOWED));
            }
            else
            {
                returnString = RoundSignificant(returnValue, MAXIMUMDIGITSALLOWED - min(numPreDecimal, MAXIMUMDIGITSALLOWED));
            }
            m_returnDisplay = returnString;
            TrimString(m_returnDisplay);
        }
    }

    m_returnHasDecimal = (m_returnDisplay.find(L'.') != m_returnDisplay.npos);
}

/// <summary>
/// Trims out any trailing zeros or decimals in the given input string
/// </summary>
/// <param name="input">wstring to trim</param>
void UnitConverter::TrimString(wstring& returnString)
{
    if (returnString.find(L'.') != m_returnDisplay.npos)
    {
        wstring::iterator iter;
        for (iter = returnString.end() - 1; ;iter--)
        {
            if (*iter != L'0')
            {
                returnString.erase(iter + 1, returnString.end());
                break;
            }
        }
        if (*(returnString.end()-1) == L'.')
        {
            returnString.erase(returnString.end()-1, returnString.end());
        }
    }
}

/// <summary>
/// Rounds the given double to the given number of significant digits
/// </summary>
/// <param name="num">input double</param>
/// <param name="numSignificant">int number of significant digits to round to</param>
wstring UnitConverter::RoundSignificant(double num, int numSignificant)
{
    wstringstream out(wstringstream::out);
    out << fixed;
    out.precision(numSignificant);
    out << num;
    return out.str();
}

void UnitConverter::UpdateCurrencySymbols()
{
    if (m_currencyDataLoader != nullptr && m_vmCurrencyCallback != nullptr)
    {
        shared_ptr<ICurrencyConverterDataLoader> currencyDataLoader = GetCurrencyConverterDataLoader();
        const pair<wstring, wstring> currencySymbols = currencyDataLoader->GetCurrencySymbols(m_fromType, m_toType);
        const pair<wstring, wstring> currencyRatios = currencyDataLoader->GetCurrencyRatioEquality(m_fromType, m_toType);

        m_vmCurrencyCallback->CurrencySymbolsCallback(currencySymbols.first, currencySymbols.second);
        m_vmCurrencyCallback->CurrencyRatiosCallback(currencyRatios.first, currencyRatios.second);
    }
}

void UnitConverter::UpdateViewModel()
{
    m_vmCallback->DisplayCallback(m_currentDisplay, m_returnDisplay);
    m_vmCallback->SuggestedValueCallback(CalculateSuggested());
}
