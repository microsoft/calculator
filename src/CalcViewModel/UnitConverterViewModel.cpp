// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "UnitConverterViewModel.h"
#include "CalcManager/Header Files/EngineStrings.h"
#include "Common/CalculatorButtonPressedEventArgs.h"
#include "Common/CopyPasteManager.h"
#include "Common/LocalizationStringUtil.h"
#include "Common/LocalizationService.h"
#include "Common/LocalizationSettings.h"
#include "Common/TraceLogger.h"
#include "DataLoaders/CurrencyHttpClient.h"
#include "DataLoaders/CurrencyDataLoader.h"
#include "DataLoaders/UnitConverterDataLoader.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::Automation;
using namespace CalculatorApp::ViewModel;
using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::System;
using namespace Windows::System::Threading;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Storage;

constexpr int EXPECTEDVIEWMODELDATATOKENS = 8;

// interval is in 100 nanosecond units
constexpr unsigned int TIMER_INTERVAL_IN_MS = 10000;

#ifdef UNIT_TESTS
#define TIMER_CALLBACK_CONTEXT CallbackContext::Any
#else
#define TIMER_CALLBACK_CONTEXT CallbackContext::Same
#endif

const TimeSpan SUPPLEMENTARY_VALUES_INTERVAL = { 10 * TIMER_INTERVAL_IN_MS };

static Unit^ EMPTY_UNIT = ref new Unit(UCM::EMPTY_UNIT);

constexpr size_t UNIT_LIST = 0;
constexpr size_t SELECTED_SOURCE_UNIT = 1;
constexpr size_t SELECTED_TARGET_UNIT = 2;

// x millisecond delay before we consider conversion to be final
constexpr unsigned int CONVERSION_FINALIZED_DELAY_IN_MS = 1000;

namespace CalculatorApp::ViewModel
{
    namespace UnitConverterViewModelProperties
    {
        StringReference CurrentCategory(L"CurrentCategory");
        StringReference Unit1(L"Unit1");
        StringReference Unit2(L"Unit2");
        StringReference Value1Active(L"Value1Active");
        StringReference Value2Active(L"Value2Active");
        StringReference Value1(L"Value1");
        StringReference Value2(L"Value2");
        StringReference Value1AutomationName(L"Value1AutomationName");
        StringReference Value2AutomationName(L"Value2AutomationName");
        StringReference SupplementaryVisibility(L"SupplementaryVisibility");
        StringReference SupplementaryResults(L"SupplementaryResults");
        StringReference Unit1AutomationName(L"Unit1AutomationName");
        StringReference Unit2AutomationName(L"Unit2AutomationName");
        StringReference CurrencySymbol1(L"CurrencySymbol1");
        StringReference CurrencySymbol2(L"CurrencySymbol2");
        StringReference CurrencySymbolVisibility(L"CurrencySymbolVisibility");
        StringReference CurrencyRatioEquality(L"CurrencyRatioEquality");
        StringReference CurrencyRatioEqualityAutomationName(L"CurrencyRatioEqualityAutomationName");
        StringReference NetworkBehavior(L"NetworkBehavior");
        StringReference CurrencyDataLoadFailed(L"CurrencyDataLoadFailed");
        StringReference CurrencyDataIsWeekOld(L"CurrencyDataIsWeekOld");
        StringReference IsCurrencyLoadingVisible(L"IsCurrencyLoadingVisible");
    }

    namespace UnitConverterResourceKeys
    {
        StringReference ValueFromFormat(L"Format_ValueFrom");
        StringReference ValueFromDecimalFormat(L"Format_ValueFrom_Decimal");
        StringReference ValueToFormat(L"Format_ValueTo");
        StringReference ConversionResultFormat(L"Format_ConversionResult");
        StringReference InputUnit_Name(L"InputUnit_Name");
        StringReference OutputUnit_Name(L"OutputUnit_Name");
        StringReference MaxDigitsReachedFormat(L"Format_MaxDigitsReached");
        StringReference UpdatingCurrencyRates(L"UpdatingCurrencyRates");
        StringReference CurrencyRatesUpdated(L"CurrencyRatesUpdated");
        StringReference CurrencyRatesUpdateFailed(L"CurrencyRatesUpdateFailed");
    }
}

UnitConverterViewModel::UnitConverterViewModel(const shared_ptr<UCM::IUnitConverter>& model) :
    m_model(model),
    m_resettingTimer(false),
    m_value1cp(ConversionParameter::Source),
    m_Value1Active(true),
    m_Value2Active(false),
    m_Value1("0"),
    m_Value2("0"),
    m_valueToUnlocalized(L"0"),
    m_valueFromUnlocalized(L"0"),
    m_relocalizeStringOnSwitch(false),
    m_Categories(ref new Vector<Category^>()),
    m_Units(ref new Vector<Unit^>()),
    m_SupplementaryResults(ref new Vector<SupplementaryResult^>),
    m_IsDropDownOpen(false),
    m_IsDropDownEnabled(true),
    m_IsCurrencyLoadingVisible(false),
    m_isCurrencyDataLoaded(false),
    m_lastAnnouncedFrom(L""),
    m_lastAnnouncedTo(L""),
    m_lastAnnouncedConversionResult(L""),
    m_isValue1Updating(false),
    m_isValue2Updating(false),
    m_Announcement(nullptr),
    m_Mode(ViewMode::None),
    m_CurrencySymbol1(L""),
    m_CurrencySymbol2(L""),
    m_IsCurrencyCurrentCategory(false),
    m_CurrencyRatioEquality(L""),
    m_CurrencyRatioEqualityAutomationName(L""),
    m_isInputBlocked(false),
    m_CurrencyDataLoadFailed(false)
{
    m_model->SetViewModelCallback(make_shared<UnitConverterVMCallback>(this));
    m_model->SetViewModelCurrencyCallback(make_shared<ViewModelCurrencyCallback>(this));
    m_decimalFormatter = LocalizationService::GetRegionalSettingsAwareDecimalFormatter();
    m_decimalFormatter->FractionDigits = 0;
    m_decimalFormatter->IsGrouped = true;
    m_decimalSeparator = LocalizationSettings::GetInstance().GetDecimalSeparator();

    m_currencyFormatter = LocalizationService::GetRegionalSettingsAwareCurrencyFormatter();
    m_currencyFormatter->IsGrouped = true;
    m_currencyFormatter->Mode = CurrencyFormatterMode::UseCurrencyCode;
    m_currencyFormatter->ApplyRoundingForCurrency(RoundingAlgorithm::RoundHalfDown);
    m_currencyMaxFractionDigits = m_currencyFormatter->FractionDigits;
    
    auto resourceLoader = AppResourceProvider::GetInstance();
    m_localizedValueFromFormat = resourceLoader.GetResourceString(UnitConverterResourceKeys::ValueFromFormat);
    m_localizedValueToFormat = resourceLoader.GetResourceString(UnitConverterResourceKeys::ValueToFormat);
    m_localizedConversionResultFormat = resourceLoader.GetResourceString(UnitConverterResourceKeys::ConversionResultFormat);
    m_localizedValueFromDecimalFormat = resourceLoader.GetResourceString(UnitConverterResourceKeys::ValueFromDecimalFormat);
    m_localizedInputUnitName = resourceLoader.GetResourceString(UnitConverterResourceKeys::InputUnit_Name);
    m_localizedOutputUnitName = resourceLoader.GetResourceString(UnitConverterResourceKeys::OutputUnit_Name);

    Unit1AutomationName = m_localizedInputUnitName;
    Unit2AutomationName = m_localizedOutputUnitName;
    IsDecimalEnabled = true;

    m_IsFirstTime = true;
    m_model->Initialize();
    PopulateData();
}

void UnitConverterViewModel::ResetView()
{
    m_model->SendCommand(UCM::Command::Reset);
    m_IsFirstTime = true;
    OnCategoryChanged(nullptr);
}

void UnitConverterViewModel::PopulateData()
{
    InitializeView();
}

void UnitConverterViewModel::OnCategoryChanged(Object^ parameter)
{
    UCM::Category currentCategory = CurrentCategory->GetModelCategory();
    IsCurrencyCurrentCategory = currentCategory.id == NavCategory::Serialize(ViewMode::Currency);

    m_model->SendCommand(UCM::Command::Clear);

    m_isInputBlocked = false;
    SetSelectedUnits();

    IsCurrencyLoadingVisible = m_IsCurrencyCurrentCategory && !m_isCurrencyDataLoaded;
    IsDropDownEnabled = m_Units->GetAt(0) != EMPTY_UNIT;

    UnitChanged->Execute(nullptr);
}

void UnitConverterViewModel::SetSelectedUnits()
{
    UCM::CategorySelectionInitializer categoryInitializer = m_model->SetCurrentCategory(CurrentCategory->GetModelCategory());
    BuildUnitList(get<UNIT_LIST>(categoryInitializer));

    UnitFrom = FindUnitInList(get<SELECTED_SOURCE_UNIT>(categoryInitializer));
    UnitTo = FindUnitInList(get<SELECTED_TARGET_UNIT>(categoryInitializer));
}

void UnitConverterViewModel::BuildUnitList(const vector<UCM::Unit>& modelUnitList)
{
    m_Units->Clear();
    for (const UCM::Unit& modelUnit : modelUnitList)
    {
        if (modelUnit.isWhimsical)
        {
            continue;
        }

        m_Units->Append(ref new Unit(modelUnit));
    }

    if (m_Units->Size == 0)
    {
        m_Units->Append(EMPTY_UNIT);
    }
}

Unit^ UnitConverterViewModel::FindUnitInList(UCM::Unit target)
{
    for (Unit^ vmUnit : m_Units)
    {
        UCM::Unit modelUnit = vmUnit->GetModelUnit();
        if (modelUnit.id == target.id)
        {
            return vmUnit;
        }
    }

    return EMPTY_UNIT;
}

void UnitConverterViewModel::OnUnitChanged(Object^ parameter)
{
    if ((m_Unit1 == nullptr) || (m_Unit2 == nullptr))
    {
        // Return if both Unit1 & Unit2 are not set
        return;
    }

    m_model->SetCurrentUnitTypes(UnitFrom->GetModelUnit(), UnitTo->GetModelUnit());
    if (m_supplementaryResultsTimer != nullptr)
    {
        // End timer to show results immediately
        m_supplementaryResultsTimer->Cancel();
    }
    if (!m_IsFirstTime)
    {
        SaveUserPreferences();
    }
    else
    {
        RestoreUserPreferences();
        m_IsFirstTime = false;
    }
}

void UnitConverterViewModel::OnSwitchActive(Platform::Object^ unused)
{
    // this can be false if this switch occurs without the user having explicitly updated any strings 
    // (for example, during deserialization). We only want to try this cleanup if there's actually
    // something to clean up.
    if (m_relocalizeStringOnSwitch)
    {
        // clean up any ill-formed strings that were in progress before the switch
        ValueFrom = ConvertToLocalizedString(m_valueFromUnlocalized, false);
    }

    SwitchConversionParameters();
    // Now deactivate the other
    if (m_value1cp == ConversionParameter::Source)
    {
        Value2Active = false;
    }
    else
    {
        Value1Active = false;
    }

    m_valueFromUnlocalized.swap(m_valueToUnlocalized);
    Utils::Swap(&m_localizedValueFromFormat, &m_localizedValueToFormat);
    
    Utils::Swap(&m_Unit1AutomationName, &m_Unit2AutomationName);
    RaisePropertyChanged(UnitConverterViewModelProperties::Unit1AutomationName);
    RaisePropertyChanged(UnitConverterViewModelProperties::Unit2AutomationName);

    m_isInputBlocked = false;
    m_model->SwitchActive(m_valueFromUnlocalized);
}

String^ UnitConverterViewModel::ConvertToLocalizedString(const std::wstring& stringToLocalize, bool allowPartialStrings)
{
    Platform::String^ result;

    if (stringToLocalize.empty())
    {
        return result;
    }

    m_decimalFormatter->IsDecimalPointAlwaysDisplayed = false;
    m_decimalFormatter->FractionDigits = 0;
    m_currencyFormatter->IsDecimalPointAlwaysDisplayed = false;
    m_currencyFormatter->FractionDigits = 0;

    wstring::size_type posOfE = stringToLocalize.find(L'e');
    if (posOfE != wstring::npos)
    {
        wstring::size_type posOfSign = posOfE + 1;
        wchar_t signOfE = stringToLocalize.at(posOfSign);
        std::wstring significandStr(stringToLocalize.substr(0, posOfE));
        std::wstring exponentStr(stringToLocalize.substr(posOfSign + 1, stringToLocalize.length() - posOfSign));

        result += ConvertToLocalizedString(significandStr, allowPartialStrings) + "e" + signOfE + ConvertToLocalizedString(exponentStr, allowPartialStrings);
    }
    else
    {
        // stringToLocalize is in en-US and has the default decimal separator, so this is safe to do.
        wstring::size_type posOfDecimal = stringToLocalize.find(L'.');

        bool hasDecimal = wstring::npos != posOfDecimal;

        if (hasDecimal)
        {
            if (allowPartialStrings)
            {
                // allow "in progress" strings, like "3." that occur during the composition of
                // a final number. Without this, when typing the three characters in "3.2" 
                // you don't see the decimal point when typing it, you only see it once you've finally
                // typed a post-decimal digit.

                m_decimalFormatter->IsDecimalPointAlwaysDisplayed = true;
                m_currencyFormatter->IsDecimalPointAlwaysDisplayed = true;
            }

            // force post-decimal digits so that trailing zeroes entered by the user aren't suddenly cut off.
            m_decimalFormatter->FractionDigits = static_cast<int>(stringToLocalize.length() - (posOfDecimal + 1));
            m_currencyFormatter->FractionDigits = m_currencyMaxFractionDigits;
        }

        if (IsCurrencyCurrentCategory)
        {
            wstring currencyResult = m_currencyFormatter->Format(stod(stringToLocalize))->Data();
            wstring currencyCode = m_currencyFormatter->Currency->Data();
            
            // CurrencyFormatter always includes LangCode or Symbol. Make it include LangCode
            // because this includes a non-breaking space. Remove the LangCode.
            auto pos = currencyResult.find(currencyCode);
            if (pos != wstring::npos)
            {
                currencyResult.erase(pos, currencyCode.length());
                pos = currencyResult.find(L'\u00a0'); // non-breaking space
                if (pos != wstring::npos)
                {
                    currencyResult.erase(pos, 1);
                }
            }

            result = ref new String(currencyResult.c_str());
        }
        else
        {
            // Convert the input string to double using stod
            // Then use the decimalFormatter to reformat the double to Platform String
            result = m_decimalFormatter->Format(stod(stringToLocalize));
        }

        if (hasDecimal)
        {
            // Since the output from GetLocaleInfoEx() and DecimalFormatter are differing for decimal string
            // we are adding the below work-around of editing the string returned by DecimalFormatter 
            // and replacing the decimal separator with the one returned by GetLocaleInfoEx()
            String^ formattedSampleString = m_decimalFormatter->Format(stod("1.1"));
            wstring formattedSampleWString = wstring(formattedSampleString->Data());

            wstring resultWithDecimal = wstring(result->Data());
            size_t pos = resultWithDecimal.find(formattedSampleWString[1], 0);
            if (pos != wstring::npos)
            {
                resultWithDecimal.replace(pos, 1, &m_decimalSeparator);
            }
            
            // Copy back the edited string to the result
            result = ref new String(resultWithDecimal.c_str());
        }
    }

    wstring resultHolder = wstring(result->Data());
    if ((stringToLocalize.front() == L'-' && stod(stringToLocalize) == 0) || resultHolder.back() == L'-')
    {
        if (resultHolder.back() == L'-')
        {
            result = ref new String(resultHolder.erase(resultHolder.size() - 1, 1).c_str());
        }
        result = L"-" + result;
    }
    result = Utils::LRE + result + Utils::PDF;
    return result;
}

void UnitConverterViewModel::DisplayPasteError()
{
    String^ errorMsg = AppResourceProvider::GetInstance().GetCEngineString(SIDS_DOMAIN); /*SIDS_DOMAIN is for "invalid input"*/
    Value1 = errorMsg;
    Value2 = errorMsg;
    m_relocalizeStringOnSwitch = false;
}

void UnitConverterViewModel::UpdateDisplay(const wstring& from, const wstring& to)
{
    String^ fromStr = this->ConvertToLocalizedString(from, true);
    UpdateInputBlocked(from);
    String^ toStr = this->ConvertToLocalizedString(to, true);

    bool updatedValueFrom = ValueFrom != fromStr;
    bool updatedValueTo = ValueTo != toStr;
    if (updatedValueFrom)
    {
        m_valueFromUnlocalized = from;
        // once we've updated the unlocalized from string, we'll potentially need to clean it back up when switching between fields
        // to eliminate dangling decimal points.
        m_relocalizeStringOnSwitch = true;
    }

    if (updatedValueTo)
    {
        // This is supposed to use trimming logic, but that's highly dependent
        // on the auto-scaling textbox control which we dont have yet. For now,
        // not doing anything. It will have to be integrated once that control is
        // created.
        m_valueToUnlocalized = to;
    }

    m_isValue1Updating = m_Value1Active ? updatedValueFrom : updatedValueTo;
    m_isValue2Updating = m_Value2Active ? updatedValueFrom : updatedValueTo;

    // Setting these properties before setting the member variables above causes
    // a chain of properties that can result in the wrong result being announced
    // to Narrator. We need to know which values are updating before setting the
    // below properties, so that we know when to announce the result.
    if (updatedValueFrom)
    {
        ValueFrom = fromStr;
    }

    if (updatedValueTo)
    {
        ValueTo = toStr;
    }
}

void UnitConverterViewModel::UpdateSupplementaryResults(const std::vector<std::tuple<std::wstring, UnitConversionManager::Unit>>& suggestedValues)
{
    m_cacheMutex.lock();
    m_cachedSuggestedValues = suggestedValues;
    m_cacheMutex.unlock();

    // If we're already "ticking", reset the timer
    if (m_supplementaryResultsTimer != nullptr)
    {
        m_resettingTimer = true;
        m_supplementaryResultsTimer->Cancel();
        m_resettingTimer = false;
    }

    // Schedule the timer
    m_supplementaryResultsTimer = ThreadPoolTimer::CreateTimer(
        ref new TimerElapsedHandler(this, &UnitConverterViewModel::SupplementaryResultsTimerTick, TIMER_CALLBACK_CONTEXT),
        SUPPLEMENTARY_VALUES_INTERVAL,
        ref new TimerDestroyedHandler(this, &UnitConverterViewModel::SupplementaryResultsTimerCancel, TIMER_CALLBACK_CONTEXT));
}

void UnitConverterViewModel::OnValueActivated(IActivatable^ control)
{
    control->IsActive = true;
}

void UnitConverterViewModel::OnButtonPressed(Platform::Object^ parameter)
{
    NumbersAndOperatorsEnum numOpEnum = CalculatorButtonPressedEventArgs::GetOperationFromCommandParameter(parameter);
    UCM::Command command = CommandFromButtonId(numOpEnum);

    //Don't clear the display if combo box is open and escape is pressed
    if (command == UCM::Command::Clear && IsDropDownOpen)
    {
        return;
    }

    static const vector<UCM::Command> OPERANDS = {
        UCM::Command::Zero,
        UCM::Command::One,
        UCM::Command::Two,
        UCM::Command::Three,
        UCM::Command::Four,
        UCM::Command::Five,
        UCM::Command::Six,
        UCM::Command::Seven,
        UCM::Command::Eight,
        UCM::Command::Nine
    };

    if (find(begin(OPERANDS), end(OPERANDS), command) != OPERANDS.end())
    {
        if (m_isInputBlocked)
        {
            return;
        }

        if (m_IsCurrencyCurrentCategory)
        {
            StartConversionResultTimer();
        }
    }

    m_model->SendCommand(command);
}

void UnitConverterViewModel::OnCopyCommand(Platform::Object^ parameter)
{
    //EventWriteClipboardCopy_Start();
    CopyPasteManager::CopyToClipboard(ref new Platform::String(m_valueFromUnlocalized.c_str()));
    //EventWriteClipboardCopy_Stop();
}

void UnitConverterViewModel::OnPasteCommand(Platform::Object^ parameter)
{
    // if there's nothing to copy early out
    if (!CopyPasteManager::HasStringToPaste())
    {
        return;
    }

    // Ensure that the paste happens on the UI thread
    //EventWriteClipboardPaste_Start();
    // Any converter ViewMode is fine here.
    CopyPasteManager::GetStringToPaste(m_Mode, NavCategory::GetGroupType(m_Mode)).then(
        [this](String^ pastedString)
    {
        OnPaste(pastedString, m_Mode);
    }, concurrency::task_continuation_context::use_current());
}

void UnitConverterViewModel::InitializeView()
{
    vector<UCM::Category> categories = m_model->GetCategories();
    for (UINT i = 0; i < categories.size(); i++)
    {
        Category^ category = ref new Category(categories[i]);
        m_Categories->Append(category);
    }

    RestoreUserPreferences();
    CurrentCategory = ref new Category(m_model->GetCurrentCategory());
}

void UnitConverterViewModel::OnPropertyChanged(Platform::String^ prop)
{
    static bool isCategoryChanging = false;

    if (prop->Equals(UnitConverterViewModelProperties::CurrentCategory))
    {
        isCategoryChanging = true;
        CategoryChanged->Execute(nullptr);
        isCategoryChanging = false;
    }
    else if (prop->Equals(UnitConverterViewModelProperties::Unit1) || prop->Equals(UnitConverterViewModelProperties::Unit2))
    {
        // Category changes will handle updating units after they've both been updated.
        // This event should only be used to update units from explicit user interaction.
        if (!isCategoryChanging)
        {
            UnitChanged->Execute(nullptr);
        }
        // Get the localized automation name for each CalculationResults field
        if (prop->Equals(UnitConverterViewModelProperties::Unit1))
        {
            UpdateValue1AutomationName();
        }
        else
        {
            UpdateValue2AutomationName();
        }
    }
    else if (prop->Equals(UnitConverterViewModelProperties::Value1))
    {
        UpdateValue1AutomationName();
    }
    else if (prop->Equals(UnitConverterViewModelProperties::Value2))
    {
        UpdateValue2AutomationName();
    }
    else if (prop->Equals(UnitConverterViewModelProperties::Value1Active) || prop->Equals(UnitConverterViewModelProperties::Value2Active))
    {
        // if one of the values is activated, and as a result both are true, it means
        // that we're trying to switch.
        if (Value1Active && Value2Active)
        {
            SwitchActive->Execute(nullptr);
        }

        UpdateValue1AutomationName();
        UpdateValue2AutomationName();
    }
    else if (prop->Equals(UnitConverterViewModelProperties::SupplementaryResults))
    {
        RaisePropertyChanged(UnitConverterViewModelProperties::SupplementaryVisibility);
    }
    else if (prop->Equals(UnitConverterViewModelProperties::Value1AutomationName))
    {
        m_isValue1Updating = false;
        if (!m_isValue2Updating)
        {
            AnnounceConversionResult();
        }
    }
    else if (prop->Equals(UnitConverterViewModelProperties::Value2AutomationName))
    {
        m_isValue2Updating = false;
        if (!m_isValue1Updating)
        {
            AnnounceConversionResult();
        }
    }
    else if (prop->Equals(UnitConverterViewModelProperties::CurrencySymbol1) || prop->Equals(UnitConverterViewModelProperties::CurrencySymbol2))
    {
        RaisePropertyChanged(UnitConverterViewModelProperties::CurrencySymbolVisibility);
    }
}

String^ UnitConverterViewModel::Serialize()
{
    wstringstream out(wstringstream::out);
    const wchar_t * delimiter = L"[;;;]";
    out << std::to_wstring(m_resettingTimer) << delimiter;
    out << std::to_wstring(static_cast<int>(m_value1cp)) << delimiter;
    out << m_Value1Active << delimiter << m_Value2Active << delimiter;
    out << m_Value1->Data() << delimiter << m_Value2->Data() << delimiter;
    out << m_valueFromUnlocalized << delimiter << m_valueToUnlocalized << delimiter << L"[###]";
    wstring unitConverterSerializedData = m_model->Serialize();

    if (!unitConverterSerializedData.empty())
    {
        out << m_model->Serialize() << L"[###]";
        String^ serializedData = ref new String(wstring(out.str()).c_str());
        return serializedData;
    }
    else
    {
        return nullptr;
    }
}

void UnitConverterViewModel::Deserialize(Platform::String^ state)
{
    wstring serializedData = wstring(state->Data());
    vector<wstring> tokens = UCM::UnitConverter::StringToVector(serializedData, L"[###]");
    assert(tokens.size() >= 2);
    vector<wstring> viewModelData = UCM::UnitConverter::StringToVector(tokens[0], L"[;;;]");
    assert(viewModelData.size() == EXPECTEDVIEWMODELDATATOKENS);
    m_resettingTimer = (viewModelData[0].compare(L"1") == 0);
    m_value1cp = (ConversionParameter)_wtoi(viewModelData[1].c_str());
    m_Value1Active = (viewModelData[2].compare(L"1") == 0);
    m_Value2Active = (viewModelData[3].compare(L"1") == 0);
    m_Value1 = ref new String(viewModelData[4].c_str());
    m_Value2 = ref new String(viewModelData[5].c_str());
    m_valueFromUnlocalized = viewModelData[6];
    m_valueToUnlocalized = viewModelData[7];
    wstringstream modelData(wstringstream::out);
    for (unsigned int i = 1; i < tokens.size(); i++)
    {
        modelData << tokens[i] << L"[###]";
    }
    m_model->DeSerialize(modelData.str());
    InitializeView();
    RaisePropertyChanged(nullptr);  // Update since all props have been updated.
}

//Saving User Preferences of Category and Associated-Units across Sessions. 
void UnitConverterViewModel::SaveUserPreferences()
{
    if (UnitsAreValid())
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
        if (!m_IsCurrencyCurrentCategory)
        {
            auto userPreferences = m_model->SaveUserPreferences();
            localSettings->Values->Insert(ref new String(L"UnitConverterPreferences"), ref new String(userPreferences.c_str()));
        }
        else
        {
            // Currency preferences shouldn't be saved in the same way as standard converter modes because
            // the delay loading creates a big mess of issues that are better to avoid.
            localSettings->Values->Insert(UnitConverterResourceKeys::CurrencyUnitFromKey, UnitFrom->Abbreviation);
            localSettings->Values->Insert(UnitConverterResourceKeys::CurrencyUnitToKey, UnitTo->Abbreviation);
        }
    }
}

//Restoring User Preferences of Category and Associated-Units.
void UnitConverterViewModel::RestoreUserPreferences()
{
    if (!IsCurrencyCurrentCategory)
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
        if (localSettings->Values->HasKey(ref new String(L"UnitConverterPreferences")))
        {
            String^ userPreferences = safe_cast<String^>(localSettings->Values->Lookup(ref new String(L"UnitConverterPreferences")));
            m_model->RestoreUserPreferences(userPreferences->Data());
        }
    }
}

void UnitConverterViewModel::OnCurrencyDataLoadFinished(bool didLoad)
{
    m_isCurrencyDataLoaded = true;
    CurrencyDataLoadFailed = !didLoad;
    ResetView();

    StringReference key = didLoad ? UnitConverterResourceKeys::CurrencyRatesUpdated : UnitConverterResourceKeys::CurrencyRatesUpdateFailed;
    String^ announcement = AppResourceProvider::GetInstance().GetResourceString(key);
    Announcement = CalculatorAnnouncement::GetUpdateCurrencyRatesAnnouncement(announcement);
}

void UnitConverterViewModel::OnCurrencyTimestampUpdated(_In_ const wstring& timestamp, bool isWeekOld)
{
    CurrencyDataIsWeekOld = isWeekOld;
    CurrencyTimestamp = ref new String(timestamp.c_str());
}

void UnitConverterViewModel::RefreshCurrencyRatios()
{
    m_isCurrencyDataLoaded = false;
    IsCurrencyLoadingVisible = true;

    String^ announcement = AppResourceProvider::GetInstance().GetResourceString(UnitConverterResourceKeys::UpdatingCurrencyRates);
    Announcement = CalculatorAnnouncement::GetUpdateCurrencyRatesAnnouncement(announcement);

    auto refreshTask = create_task(m_model->RefreshCurrencyRatios());
    refreshTask.then([this](const pair<bool, wstring>& refreshResult)
    {
        bool didLoad = refreshResult.first;
        wstring timestamp = refreshResult.second;

        OnCurrencyTimestampUpdated(timestamp, false /*isWeekOldData*/);
        OnCurrencyDataLoadFinished(didLoad);
    }, task_continuation_context::use_current());
}

void UnitConverterViewModel::OnNetworkBehaviorChanged(_In_ NetworkAccessBehavior newBehavior)
{
    CurrencyDataLoadFailed = false;
    NetworkBehavior = newBehavior;
}

UnitConversionManager::Command UnitConverterViewModel::CommandFromButtonId(NumbersAndOperatorsEnum button)
{
    UCM::Command command;

    switch (button)
    {
    case NumbersAndOperatorsEnum::Zero:
        command = UCM::Command::Zero;
        break;
    case NumbersAndOperatorsEnum::One:
        command = UCM::Command::One;
        break;
    case NumbersAndOperatorsEnum::Two:
        command = UCM::Command::Two;
        break;
    case NumbersAndOperatorsEnum::Three:
        command = UCM::Command::Three;
        break;
    case NumbersAndOperatorsEnum::Four:
        command = UCM::Command::Four;
        break;
    case NumbersAndOperatorsEnum::Five:
        command = UCM::Command::Five;
        break;
    case NumbersAndOperatorsEnum::Six:
        command = UCM::Command::Six;
        break;
    case NumbersAndOperatorsEnum::Seven:
        command = UCM::Command::Seven;
        break;
    case NumbersAndOperatorsEnum::Eight:
        command = UCM::Command::Eight;
        break;
    case NumbersAndOperatorsEnum::Nine:
        command = UCM::Command::Nine;
        break;
    case NumbersAndOperatorsEnum::Decimal:
        command = UCM::Command::Decimal;
        break;
    case NumbersAndOperatorsEnum::Negate:
        command = UCM::Command::Negate;
        break;
    case NumbersAndOperatorsEnum::Backspace:
        command = UCM::Command::Backspace;
        break;
    case NumbersAndOperatorsEnum::Clear:
        command = UCM::Command::Clear;
        break;
    default:
        command = UCM::Command::None;
        break;
    }

    return command;
}

void UnitConverterViewModel::SupplementaryResultsTimerTick(ThreadPoolTimer^ timer)
{
    timer->Cancel();
}

void UnitConverterViewModel::SupplementaryResultsTimerCancel(ThreadPoolTimer^ timer)
{
    if (!m_resettingTimer)
    {
        RefreshSupplementaryResults();
    }
}

void UnitConverterViewModel::RefreshSupplementaryResults()
{
    m_cacheMutex.lock();
    m_SupplementaryResults->Clear();

    vector<SupplementaryResult^> whimsicals;

    for (tuple<wstring, UCM::Unit> suggestedValue : m_cachedSuggestedValues)
    {
        SupplementaryResult^ result =
            ref new SupplementaryResult(
            this->ConvertToLocalizedString(get<0>(suggestedValue), false),
            ref new Unit(get<1>(suggestedValue)));
        if (result->IsWhimsical())
        {
            whimsicals.push_back(result);
        }
        else
        {
            m_SupplementaryResults->Append(result);
        }
    }

    if (whimsicals.size() > 0)
    {
        m_SupplementaryResults->Append(whimsicals[0]);
    }

    m_cacheMutex.unlock();
    RaisePropertyChanged(UnitConverterViewModelProperties::SupplementaryResults);
    //EventWriteConverterSupplementaryResultsUpdated();
}

// When UpdateDisplay is called, the ViewModel will remember the From/To unlocalized display values
// This function will announce the conversion result after the ValueTo/ValueFrom automation names update,
// only if the new unlocalized display values are different from the last announced values, and if the
// values are not both zero.
void UnitConverterViewModel::AnnounceConversionResult()
{
    if ((m_valueFromUnlocalized != m_lastAnnouncedFrom
        || m_valueToUnlocalized != m_lastAnnouncedTo)
        && Unit1 != nullptr
        && Unit2 != nullptr)
    {
        m_lastAnnouncedFrom = m_valueFromUnlocalized;
        m_lastAnnouncedTo = m_valueToUnlocalized;

        Unit^ unitFrom = Value1Active ? Unit1 : Unit2;
        Unit^ unitTo = (unitFrom == Unit1) ? Unit2 : Unit1;
        m_lastAnnouncedConversionResult = GetLocalizedConversionResultStringFormat(ValueFrom, unitFrom->Name, ValueTo, unitTo->Name);

        Announcement = CalculatorAnnouncement::GetDisplayUpdatedAnnouncement(m_lastAnnouncedConversionResult);
    }
}

void UnitConverterViewModel::UpdateInputBlocked(_In_ const wstring& currencyInput)
{
    // currencyInput is in en-US and has the default decimal separator, so this is safe to do.
    auto posOfDecimal = currencyInput.find(L'.');
    if (posOfDecimal != wstring::npos && IsCurrencyCurrentCategory)
    {
        m_isInputBlocked = (posOfDecimal + static_cast<size_t>(m_currencyMaxFractionDigits) + 1 == currencyInput.length());
    }
    else
    {
        m_isInputBlocked = false;
    }
}

NumbersAndOperatorsEnum UnitConverterViewModel::MapCharacterToButtonId(
    const wchar_t ch,
    bool& canSendNegate)
{
    static_assert(NumbersAndOperatorsEnum::Zero < NumbersAndOperatorsEnum::One, "NumbersAndOperatorsEnum order is invalid");
    static_assert(NumbersAndOperatorsEnum::One < NumbersAndOperatorsEnum::Two, "NumbersAndOperatorsEnum order is invalid");
    static_assert(NumbersAndOperatorsEnum::Two < NumbersAndOperatorsEnum::Three, "NumbersAndOperatorsEnum order is invalid");
    static_assert(NumbersAndOperatorsEnum::Three < NumbersAndOperatorsEnum::Four, "NumbersAndOperatorsEnum order is invalid");
    static_assert(NumbersAndOperatorsEnum::Four < NumbersAndOperatorsEnum::Five, "NumbersAndOperatorsEnum order is invalid");
    static_assert(NumbersAndOperatorsEnum::Five < NumbersAndOperatorsEnum::Six, "NumbersAndOperatorsEnum order is invalid");
    static_assert(NumbersAndOperatorsEnum::Six < NumbersAndOperatorsEnum::Seven, "NumbersAndOperatorsEnum order is invalid");
    static_assert(NumbersAndOperatorsEnum::Seven < NumbersAndOperatorsEnum::Eight, "NumbersAndOperatorsEnum order is invalid");
    static_assert(NumbersAndOperatorsEnum::Eight < NumbersAndOperatorsEnum::Nine, "NumbersAndOperatorsEnum order is invalid");
    static_assert(NumbersAndOperatorsEnum::Zero < NumbersAndOperatorsEnum::Nine, "NumbersAndOperatorsEnum order is invalid");

    NumbersAndOperatorsEnum mappedValue = NumbersAndOperatorsEnum::None;
    canSendNegate = false;

    switch (ch)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        mappedValue = NumbersAndOperatorsEnum::Zero + static_cast<NumbersAndOperatorsEnum>(ch - L'0');
        canSendNegate = true;
        break;

    case '-':
        mappedValue = NumbersAndOperatorsEnum::Negate;
        break;

    default:
        // Respect the user setting for decimal separator
        if (ch == m_decimalSeparator)
        {
            mappedValue = NumbersAndOperatorsEnum::Decimal;
            canSendNegate = true;
            break;
        }
    }

    if (mappedValue == NumbersAndOperatorsEnum::None)
    {
        if (LocalizationSettings::GetInstance().IsLocalizedDigit(ch))
        {
            mappedValue = NumbersAndOperatorsEnum::Zero + static_cast<NumbersAndOperatorsEnum>(ch - LocalizationSettings::GetInstance().GetDigitSymbolFromEnUsDigit(L'0'));
            canSendNegate = true;
        }
    }

    return mappedValue;
}

void UnitConverterViewModel::OnPaste(String^ stringToPaste, ViewMode mode)
{
    // If pastedString is invalid("NoOp") then display pasteError else process the string
    if (stringToPaste == StringReference(CopyPasteManager::PasteErrorString))
    {
        this->DisplayPasteError();
        return;
    }

    TraceLogger::GetInstance().LogValidInputPasted(mode);
    bool isFirstLegalChar = true;
    bool sendNegate = false;
    wstring accumulation = L"";

    for (auto it = stringToPaste->Begin(); it != stringToPaste->End(); it++)
    {
        bool canSendNegate = false;

        NumbersAndOperatorsEnum op = MapCharacterToButtonId(*it, canSendNegate);

        if (NumbersAndOperatorsEnum::None != op)
        {
            if (isFirstLegalChar)
            {
                // Send Clear before sending something that will actually apply 
                // to the field.
                m_model->SendCommand(UCM::Command::Clear);
                isFirstLegalChar = false;

                // If the very first legal character is a - sign, send negate
                // after sending the next legal character.  Send nothing now, or
                // it will be ignored.
                if (NumbersAndOperatorsEnum::Negate == op)
                {
                    sendNegate = true;
                }
            }

            // Negate is only allowed if it's the first legal character, which is handled above.
            if (NumbersAndOperatorsEnum::None != op && NumbersAndOperatorsEnum::Negate != op)
            {
                UCM::Command cmd = CommandFromButtonId(op);
                m_model->SendCommand(cmd);

                if (sendNegate)
                {
                    if (canSendNegate)
                    {
                        m_model->SendCommand(UCM::Command::Negate);
                    }
                    sendNegate = false;
                }
            }

            accumulation += *it;
            UpdateInputBlocked(accumulation);
            if (m_isInputBlocked)
            {
                break;
            }
        }
        else
        {
            sendNegate = false;
        }
    }
}

String^ UnitConverterViewModel::GetLocalizedAutomationName(_In_ String^ displayvalue, _In_ String^ unitname, _In_ String^ format)
{
    String^ valueToLocalize = displayvalue;
    if (displayvalue == ValueFrom && Utils::IsLastCharacterTarget(m_valueFromUnlocalized, m_decimalSeparator))
    {
        // Need to compute a second localized value for the automation
        // name that does not include the decimal separator.
        displayvalue = ConvertToLocalizedString(m_valueFromUnlocalized, false /*allowTrailingDecimal*/);
        format = m_localizedValueFromDecimalFormat;
    }

    wstring localizedResult = LocalizationStringUtil::GetLocalizedString(format->Data(), displayvalue->Data(), unitname->Data());
    return ref new String(localizedResult.c_str());
}

String^ UnitConverterViewModel::GetLocalizedConversionResultStringFormat(_In_ String^ fromValue, _In_ String^ fromUnit, _In_ String^ toValue, _In_ String^ toUnit)
{
    String^ localizedString = ref new String(LocalizationStringUtil::GetLocalizedString(m_localizedConversionResultFormat->Data(), fromValue->Data(), fromUnit->Data(), toValue->Data(), toUnit->Data()).c_str());
    return localizedString;
}

void UnitConverterViewModel::UpdateValue1AutomationName()
{
    if (Unit1)
    {
        Value1AutomationName = GetLocalizedAutomationName(Value1, Unit1->AccessibleName, m_localizedValueFromFormat);
    }
}

void UnitConverterViewModel::UpdateValue2AutomationName()
{
    if (Unit2)
    {
        Value2AutomationName = GetLocalizedAutomationName(Value2, Unit2->AccessibleName, m_localizedValueToFormat);
    }
}

void UnitConverterViewModel::OnMaxDigitsReached()
{
    String^ format = AppResourceProvider::GetInstance().GetResourceString(UnitConverterResourceKeys::MaxDigitsReachedFormat);
    const wstring& announcement = LocalizationStringUtil::GetLocalizedString(format->Data(), m_lastAnnouncedConversionResult->Data());
    Announcement = CalculatorAnnouncement::GetMaxDigitsReachedAnnouncement(StringReference(announcement.c_str()));
}

bool UnitConverterViewModel::UnitsAreValid()
{
    return UnitFrom != nullptr && !UnitFrom->Abbreviation->IsEmpty() && UnitTo != nullptr && !UnitTo->Abbreviation->IsEmpty();
}

void UnitConverterViewModel::StartConversionResultTimer()
{
    m_conversionResultTaskHelper = make_unique<ConversionResultTaskHelper>(
        CONVERSION_FINALIZED_DELAY_IN_MS, [this]()
    {
        if (UnitsAreValid())
        {
            String^ valueFrom = m_Value1Active ? m_Value1 : m_Value2;
            String^ valueTo = m_Value1Active ? m_Value2 : m_Value1;
            TraceLogger::GetInstance().LogConversionResult(
                valueFrom->Data(),
                UnitFrom->ToString()->Data(),
                valueTo->Data(),
                UnitTo->ToString()->Data());
        }
    });
}

String^ SupplementaryResult::GetLocalizedAutomationName()
{
    auto format = AppResourceProvider::GetInstance().GetResourceString("SupplementaryUnit_AutomationName");
    return ref new String(LocalizationStringUtil::GetLocalizedString(
        format->Data(),
        this->Value->Data(),
        this->Unit->Name->Data()).c_str());
}
