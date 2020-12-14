// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <cassert>
#include <cmath>
#include <sstream>
#include <algorithm> // for std::sort
#include "Command.h"
#include "UnitConverter.h"
#include "NumberFormattingUtils.h"

using namespace std;
using namespace UnitConversionManager;
using namespace CalcManager::NumberFormattingUtils;

static constexpr uint32_t EXPECTEDSERIALIZEDCATEGORYTOKENCOUNT = 3U;
static constexpr uint32_t EXPECTEDSERIALIZEDUNITTOKENCOUNT = 6U;
static constexpr uint32_t EXPECTEDSTATEDATATOKENCOUNT = 5U;
static constexpr uint32_t EXPECTEDMAPCOMPONENTTOKENCOUNT = 2U;

static constexpr uint32_t MAXIMUMDIGITSALLOWED = 15U;
static constexpr uint32_t OPTIMALDIGITSALLOWED = 7U;

static constexpr wchar_t LEFTESCAPECHAR = L'{';
static constexpr wchar_t RIGHTESCAPECHAR = L'}';

static const double OPTIMALDECIMALALLOWED = 1e-6;  // pow(10, -1 * (OPTIMALDIGITSALLOWED - 1));
static const double MINIMUMDECIMALALLOWED = 1e-14; // pow(10, -1 * (MAXIMUMDIGITSALLOWED - 1));

unordered_map<wchar_t, wstring> quoteConversions;
unordered_map<wstring, wchar_t> unquoteConversions;

/// <summary>
/// Constructor, sets up all the variables and requires a configLoader
/// </summary>
/// <param name="dataLoader">An instance of the IConverterDataLoader interface which we use to read in category/unit names and conversion data</param>
UnitConverter::UnitConverter(_In_ const shared_ptr<IConverterDataLoader>& dataLoader)
    : UnitConverter::UnitConverter(dataLoader, nullptr)
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
    // declaring the delimiter character conversion map
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
    ClearValues();
    ResetCategoriesAndRatios();
}

void UnitConverter::Initialize()
{
    m_dataLoader->LoadData();
}

bool UnitConverter::CheckLoad()
{
    if (m_categories.empty())
    {
        ResetCategoriesAndRatios();
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
            for (auto& unit : m_categoryToUnits[m_currentCategory.id])
            {
                unit.isConversionSource = (unit.id == m_fromType.id);
                unit.isConversionTarget = (unit.id == m_toType.id);
            }
            m_currentCategory = input;
            if (!m_currentCategory.supportsNegative && m_currentDisplay.front() == L'-')
            {
                m_currentDisplay.erase(0, 1);
            }
        }

        newUnitList = m_categoryToUnits[input.id];
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
    if (!CheckLoad())
    {
        return;
    }

    if (m_fromType != fromType)
    {
        m_switchedActive = true;
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
/// wstring representing the value user had in the field they've just activated.
/// We use this to handle cases where the front-end may choose to trim more digits
/// than we have been storing internally, in which case appending will not function
/// as expected without the use of this parameter.
/// </param>
void UnitConverter::SwitchActive(const wstring& newValue)
{
    if (!CheckLoad())
    {
        return;
    }

    swap(m_fromType, m_toType);
    swap(m_currentHasDecimal, m_returnHasDecimal);
    m_returnDisplay = m_currentDisplay;
    m_currentDisplay = newValue;
    m_currentHasDecimal = (m_currentDisplay.find(L'.') != wstring::npos);
    m_switchedActive = true;

    if (m_currencyDataLoader != nullptr && m_vmCurrencyCallback != nullptr)
    {
        shared_ptr<ICurrencyConverterDataLoader> currencyDataLoader = GetCurrencyConverterDataLoader();
        const pair<wstring, wstring> currencyRatios = currencyDataLoader->GetCurrencyRatioEquality(m_fromType, m_toType);

        m_vmCurrencyCallback->CurrencyRatiosCallback(currencyRatios.first, currencyRatios.second);
    }
}

bool UnitConversionManager::UnitConverter::IsSwitchedActive() const
{
    return m_switchedActive;
}

wstring UnitConverter::CategoryToString(const Category& c, wstring_view delimiter)
{
    return Quote(std::to_wstring(c.id))
        .append(delimiter)
        .append(Quote(std::to_wstring(c.supportsNegative)))
        .append(delimiter)
        .append(Quote(c.name))
        .append(delimiter);
}

vector<wstring> UnitConverter::StringToVector(wstring_view w, wstring_view delimiter, bool addRemainder)
{
    size_t delimiterIndex = w.find(delimiter);
    size_t startIndex = 0;
    vector<wstring> serializedTokens;
    while (delimiterIndex != wstring_view::npos)
    {
        serializedTokens.emplace_back(w.substr(startIndex, delimiterIndex - startIndex));
        startIndex = delimiterIndex + static_cast<int>(delimiter.size());
        delimiterIndex = w.find(delimiter, startIndex);
    }
    if (addRemainder)
    {
        delimiterIndex = w.size();
        serializedTokens.emplace_back(w.substr(startIndex, delimiterIndex - startIndex));
    }
    return serializedTokens;
}
wstring UnitConverter::UnitToString(const Unit& u, wstring_view delimiter)
{
    return Quote(std::to_wstring(u.id))
        .append(delimiter)
        .append(Quote(u.name))
        .append(delimiter)
        .append(Quote(u.abbreviation))
        .append(delimiter)
        .append(std::to_wstring(u.isConversionSource))
        .append(delimiter)
        .append(std::to_wstring(u.isConversionTarget))
        .append(delimiter)
        .append(std::to_wstring(u.isWhimsical))
        .append(delimiter);
}

Unit UnitConverter::StringToUnit(wstring_view w)
{
    vector<wstring> tokenList = StringToVector(w, L";");
    assert(tokenList.size() == EXPECTEDSERIALIZEDUNITTOKENCOUNT);
    Unit serializedUnit;
    serializedUnit.id = wcstol(Unquote(tokenList[0]).c_str(), nullptr, 10);
    serializedUnit.name = Unquote(tokenList[1]);
    serializedUnit.accessibleName = serializedUnit.name;
    serializedUnit.abbreviation = Unquote(tokenList[2]);
    serializedUnit.isConversionSource = (tokenList[3] == L"1");
    serializedUnit.isConversionTarget = (tokenList[4] == L"1");
    serializedUnit.isWhimsical = (tokenList[5] == L"1");
    return serializedUnit;
}

Category UnitConverter::StringToCategory(wstring_view w)
{
    vector<wstring> tokenList = StringToVector(w, L";");
    assert(tokenList.size() == EXPECTEDSERIALIZEDCATEGORYTOKENCOUNT);
    Category serializedCategory;
    serializedCategory.id = wcstol(Unquote(tokenList[0]).c_str(), nullptr, 10);
    serializedCategory.supportsNegative = (tokenList[1] == L"1");
    serializedCategory.name = Unquote(tokenList[2]);
    return serializedCategory;
}

/// <summary>
/// De-Serializes the data in the converter from a string
/// </summary>
/// <param name="userPreferences">wstring_view holding the serialized data. If it does not have expected number of parameters, we will ignore it</param>
void UnitConverter::RestoreUserPreferences(wstring_view userPreferences)
{
    if (userPreferences.empty())
    {
        return;
    }

    vector<wstring> outerTokens = StringToVector(userPreferences, L"|");
    if (outerTokens.size() != 3)
    {
        return;
    }

    auto fromType = StringToUnit(outerTokens[0]);
    auto toType = StringToUnit(outerTokens[1]);
    m_currentCategory = StringToCategory(outerTokens[2]);

    // Only restore from the saved units if they are valid in the current available units.
    auto itr = m_categoryToUnits.find(m_currentCategory.id);
    if (itr != m_categoryToUnits.end())
    {
        const auto& curUnits = itr->second;
        if (find(curUnits.begin(), curUnits.end(), fromType) != curUnits.end())
        {
            m_fromType = fromType;
        }
        if (find(curUnits.begin(), curUnits.end(), toType) != curUnits.end())
        {
            m_toType = toType;
        }
    }
}

/// <summary>
/// Serializes the Category and Associated Units in the converter and returns it as a string
/// </summary>
wstring UnitConverter::SaveUserPreferences()
{
    constexpr auto delimiter = L";"sv;
    constexpr auto pipe = L"|"sv;
    return UnitToString(m_fromType, delimiter)
        .append(pipe)
        .append(UnitToString(m_toType, delimiter))
        .append(pipe)
        .append(CategoryToString(m_currentCategory, delimiter))
        .append(pipe);
}

/// <summary>
/// Sanitizes the input string, escape quoting any symbols we rely on for our delimiters, and returns the sanitized string.
/// </summary>
/// <param name="s">wstring_view to be sanitized</param>
wstring UnitConverter::Quote(wstring_view s)
{
    wstring quotedString;

    // Iterate over the delimiter characters we need to quote
    for (const auto ch : s)
    {
        if (quoteConversions.find(ch) != quoteConversions.end())
        {
            quotedString += quoteConversions[ch];
        }
        else
        {
            quotedString += ch;
        }
    }

    return quotedString;
}

/// <summary>
/// Unsanitizes the sanitized input string, returning it to its original contents before we had quoted it.
/// </summary>
/// <param name="s">wstring_view to be unsanitized</param>
wstring UnitConverter::Unquote(wstring_view s)
{
    wstring quotedSubString;
    wstring unquotedString;
    wstring_view::const_iterator cursor = s.begin();
    while (cursor != s.end())
    {
        if (*cursor == LEFTESCAPECHAR)
        {
            quotedSubString.clear();
            while (cursor != s.end() && *cursor != RIGHTESCAPECHAR)
            {
                quotedSubString += *cursor;
                ++cursor;
            }
            if (cursor == s.end())
            {
                // Badly formatted
                break;
            }
            else
            {
                quotedSubString += *cursor;
                unquotedString += unquoteConversions[quotedSubString];
            }
        }
        else
        {
            unquotedString += *cursor;
        }
        ++cursor;
    }
    return unquotedString;
}

/// <summary>
/// Handles inputs to the converter from the view-model, corresponding to a given button or keyboard press
/// </summary>
/// <param name="command">Command enum representing the command that was entered</param>
void UnitConverter::SendCommand(Command command)
{
    if (!CheckLoad())
    {
        return;
    }

    // TODO: Localization of characters
    bool clearFront = false;
    bool clearBack = false;
    if (command != Command::Negate && m_switchedActive)
    {
        ClearValues();
        m_switchedActive = false;
        clearFront = true;
        clearBack = false;
    }
    else
    {
        clearFront = (m_currentDisplay == L"0");
        clearBack =
            ((m_currentHasDecimal && m_currentDisplay.size() - 1 >= MAXIMUMDIGITSALLOWED)
             || (!m_currentHasDecimal && m_currentDisplay.size() >= MAXIMUMDIGITSALLOWED));
    }

    switch (command)
    {
    case Command::Zero:
        m_currentDisplay += L'0';
        break;

    case Command::One:
        m_currentDisplay += L'1';
        break;

    case Command::Two:
        m_currentDisplay += L'2';
        break;

    case Command::Three:
        m_currentDisplay += L'3';
        break;

    case Command::Four:
        m_currentDisplay += L'4';
        break;

    case Command::Five:
        m_currentDisplay += L'5';
        break;

    case Command::Six:
        m_currentDisplay += L'6';
        break;

    case Command::Seven:
        m_currentDisplay += L'7';
        break;

    case Command::Eight:
        m_currentDisplay += L'8';
        break;

    case Command::Nine:
        m_currentDisplay += L'9';
        break;

    case Command::Decimal:
        clearFront = false;
        clearBack = false;
        if (!m_currentHasDecimal)
        {
            m_currentDisplay += L'.';
            m_currentHasDecimal = true;
        }
        break;

    case Command::Backspace:
        clearFront = false;
        clearBack = false;
        if ((m_currentDisplay.front() != L'-' && m_currentDisplay.size() > 1) || m_currentDisplay.size() > 2)
        {
            if (m_currentDisplay.back() == L'.')
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
            if (m_currentDisplay.front() == L'-')
            {
                m_currentDisplay.erase(0, 1);
            }
            else
            {
                m_currentDisplay.insert(0, 1, L'-');
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
        ResetCategoriesAndRatios();
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
        m_currentDisplay.pop_back();
        m_vmCallback->MaxDigitsReached();
    }

    Calculate();
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

future<pair<bool, wstring>> UnitConverter::RefreshCurrencyRatios()
{
    shared_ptr<ICurrencyConverterDataLoader> currencyDataLoader = GetCurrencyConverterDataLoader();
    future<bool> loadDataResult;
    if (currencyDataLoader != nullptr)
    {
        loadDataResult = currencyDataLoader->TryLoadDataFromWebOverrideAsync();
    }
    else
    {
        loadDataResult = async([] { return false; });
    }

    shared_future<bool> sharedLoadResult = loadDataResult.share();
    return async([currencyDataLoader, sharedLoadResult]() {
        sharedLoadResult.wait();
        bool didLoad = sharedLoadResult.get();
        wstring timestamp;
        if (currencyDataLoader != nullptr)
        {
            timestamp = currencyDataLoader->GetCurrencyTimestamp();
        }

        return make_pair(didLoad, timestamp);
    });
}

shared_ptr<ICurrencyConverterDataLoader> UnitConverter::GetCurrencyConverterDataLoader()
{
    return dynamic_pointer_cast<ICurrencyConverterDataLoader>(m_currencyDataLoader);
}

/// <summary>
/// Converts a double value into another unit type
/// </summary>
/// <param name="value">double input value to convert</param>
/// <param name="conversionData">offset and ratio to use</param>
double UnitConverter::Convert(double value, const ConversionData& conversionData)
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
    // Calculate converted values for every other unit type in this category, along with their magnitude
    for (const auto& cur : ratios)
    {
        if (cur.first != m_fromType && cur.first != m_toType)
        {
            double convertedValue = Convert(stod(m_currentDisplay), cur.second);
            SuggestedValueIntermediate newEntry;
            newEntry.magnitude = log10(convertedValue);
            newEntry.value = convertedValue;
            newEntry.type = cur.first;
            if (newEntry.type.isWhimsical)
                intermediateWhimsicalVector.push_back(newEntry);
            else
                intermediateVector.push_back(newEntry);
        }
    }

    // Sort the resulting list by absolute magnitude, breaking ties by choosing the positive value
    sort(intermediateVector.begin(), intermediateVector.end(), [](SuggestedValueIntermediate first, SuggestedValueIntermediate second) {
        if (abs(first.magnitude) == abs(second.magnitude))
        {
            return first.magnitude > second.magnitude;
        }
        else
        {
            return abs(first.magnitude) < abs(second.magnitude);
        }
    });

    // Now that the list is sorted, iterate over it and populate the return vector with properly rounded and formatted return strings
    for (const auto& entry : intermediateVector)
    {
        wstring roundedString;
        if (abs(entry.value) < 100)
        {
            roundedString = RoundSignificantDigits(entry.value, 2U);
        }
        else if (abs(entry.value) < 1000)
        {
            roundedString = RoundSignificantDigits(entry.value, 1U);
        }
        else
        {
            roundedString = RoundSignificantDigits(entry.value, 0U);
        }
        if (stod(roundedString) != 0.0 || m_currentCategory.supportsNegative)
        {
            TrimTrailingZeros(roundedString);
            returnVector.emplace_back(roundedString, entry.type);
        }
    }

    // The Whimsicals are determined differently
    // Sort the resulting list by absolute magnitude, breaking ties by choosing the positive value
    sort(intermediateWhimsicalVector.begin(), intermediateWhimsicalVector.end(), [](SuggestedValueIntermediate first, SuggestedValueIntermediate second) {
        if (abs(first.magnitude) == abs(second.magnitude))
        {
            return first.magnitude > second.magnitude;
        }
        else
        {
            return abs(first.magnitude) < abs(second.magnitude);
        }
    });

    // Now that the list is sorted, iterate over it and populate the return vector with properly rounded and formatted return strings
    vector<tuple<wstring, Unit>> whimsicalReturnVector;

    for (const auto& entry : intermediateWhimsicalVector)
    {
        wstring roundedString;
        if (abs(entry.value) < 100)
        {
            roundedString = RoundSignificantDigits(entry.value, 2U);
        }
        else if (abs(entry.value) < 1000)
        {
            roundedString = RoundSignificantDigits(entry.value, 1U);
        }
        else
        {
            roundedString = RoundSignificantDigits(entry.value, 0U);
        }

        // How to work out which is the best whimsical value to add to the vector?
        if (stod(roundedString) != 0.0)
        {
            TrimTrailingZeros(roundedString);
            whimsicalReturnVector.emplace_back(roundedString, entry.type);
        }
    }
    // Pickup the 'best' whimsical value - currently the first one
    if (!whimsicalReturnVector.empty())
    {
        returnVector.push_back(whimsicalReturnVector.front());
    }

    return returnVector;
}

/// <summary>
/// Resets categories and ratios
/// </summary>
void UnitConverter::ResetCategoriesAndRatios()
{
    m_switchedActive = false;
    m_categories = m_dataLoader->GetOrderedCategories();
    if (m_categories.empty())
    {
        return;
    }

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

        vector<Unit> units = activeDataLoader->GetOrderedUnits(category);
        m_categoryToUnits[category.id] = units;

        // Just because the units are empty, doesn't mean the user can't select this category,
        // we just want to make sure we don't let an unready category be the default.
        if (!units.empty())
        {
            for (const Unit& u : units)
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

    auto itr = m_categoryToUnits.find(m_currentCategory.id);
    if (itr == m_categoryToUnits.end())
    {
        return;
    }

    const vector<Unit>& curUnits = itr->second;
    if (!curUnits.empty())
    {
        // Units may already have been initialized through UnitConverter::RestoreUserPreferences().
        // Check if they have been, and if so, do not override restored units.
        const bool isFromUnitValid = m_fromType != EMPTY_UNIT && find(curUnits.begin(), curUnits.end(), m_fromType) != curUnits.end();
        const bool isToUnitValid = m_toType != EMPTY_UNIT && find(curUnits.begin(), curUnits.end(), m_toType) != curUnits.end();

        if (isFromUnitValid && isToUnitValid)
        {
            return;
        }

        bool conversionSourceSet = false;
        bool conversionTargetSet = false;
        for (const Unit& cur : curUnits)
        {
            if (!conversionSourceSet && cur.isConversionSource && !isFromUnitValid)
            {
                m_fromType = cur;
                conversionSourceSet = true;
            }

            if (!conversionTargetSet && cur.isConversionTarget && !isToUnitValid)
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
    if (AnyUnitIsEmpty())
    {
        m_returnDisplay = m_currentDisplay;
        m_returnHasDecimal = m_currentHasDecimal;
        TrimTrailingZeros(m_returnDisplay);
        UpdateViewModel();
        return;
    }

    unordered_map<Unit, ConversionData, UnitHash> conversionTable = m_ratioMap[m_fromType];
    if (AnyUnitIsEmpty() || (conversionTable[m_toType].ratio == 1.0 && conversionTable[m_toType].offset == 0.0))
    {
        m_returnDisplay = m_currentDisplay;
        m_returnHasDecimal = m_currentHasDecimal;
        TrimTrailingZeros(m_returnDisplay);
    }
    else
    {
        double currentValue = stod(m_currentDisplay);
        const double returnValue = Convert(currentValue, conversionTable[m_toType]);

        const auto isCurrencyConverter = m_currencyDataLoader != nullptr && m_currencyDataLoader->SupportsCategory(this->m_currentCategory);
        if (isCurrencyConverter)
        {
            // We don't need to trim the value when it's a currency.
            m_returnDisplay = RoundSignificantDigits(returnValue, MAXIMUMDIGITSALLOWED);
            TrimTrailingZeros(m_returnDisplay);
        }
        else
        {
            const unsigned int numPreDecimal = GetNumberDigitsWholeNumberPart(returnValue);
            if (numPreDecimal > MAXIMUMDIGITSALLOWED || (returnValue != 0 && abs(returnValue) < MINIMUMDECIMALALLOWED))
            {
                m_returnDisplay = ToScientificNumber(returnValue);
            }
            else
            {
                const unsigned int currentNumberSignificantDigits = GetNumberDigits(m_currentDisplay);
                unsigned int precision;
                if (abs(returnValue) < OPTIMALDECIMALALLOWED)
                {
                    precision = MAXIMUMDIGITSALLOWED;
                }
                else
                {
                    // Fewer digits are needed following the decimal if the number is large,
                    // we calculate the number of decimals necessary based on the number of digits in the integer part.
                    auto numberDigits = max(OPTIMALDIGITSALLOWED, min(MAXIMUMDIGITSALLOWED, currentNumberSignificantDigits));
                    precision = numberDigits > numPreDecimal ? numberDigits - numPreDecimal : 0;
                }

                m_returnDisplay = RoundSignificantDigits(returnValue, precision);
                TrimTrailingZeros(m_returnDisplay);
            }
            m_returnHasDecimal = (m_returnDisplay.find(L'.') != wstring::npos);
        }
    }
    UpdateViewModel();
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
