// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <cassert>
#include "Header Files/CalcEngine.h"
#include "CalculatorResource.h"

using namespace std;
using namespace CalcEngine;

/**************************************************************************/
/*** Global variable declarations and initializations                   ***/
/**************************************************************************/

static constexpr int DEFAULT_MAX_DIGITS = 32;
static constexpr int DEFAULT_PRECISION = 32;
static constexpr int32_t DEFAULT_RADIX = 10;

static constexpr wchar_t DEFAULT_DEC_SEPARATOR = L'.';
static constexpr wchar_t DEFAULT_GRP_SEPARATOR = L',';
static constexpr wstring_view DEFAULT_GRP_STR = L"3;0";
static constexpr wstring_view DEFAULT_NUMBER_STR = L"0";

// Read strings for keys, errors, trig types, etc.
// These will be copied from the resources to local memory.

unordered_map<wstring_view, wstring> CCalcEngine::s_engineStrings;

void CCalcEngine::LoadEngineStrings(CalculationManager::IResourceProvider& resourceProvider)
{
    for (const auto& sid : g_sids)
    {
        auto locString = resourceProvider.GetCEngineString(sid);
        if (!locString.empty())
        {
            s_engineStrings[sid] = locString;
        }
    }
}

//////////////////////////////////////////////////
//
// InitialOneTimeOnlyNumberSetup
//
//////////////////////////////////////////////////
void CCalcEngine::InitialOneTimeOnlySetup(CalculationManager::IResourceProvider& resourceProvider)
{
    LoadEngineStrings(resourceProvider);

    // we must now set up all the ratpak constants and our arrayed pointers
    // to these constants.
    ChangeBaseConstants(DEFAULT_RADIX, DEFAULT_MAX_DIGITS, DEFAULT_PRECISION);
}

//////////////////////////////////////////////////
//
// CCalcEngine::CCalcEngine
//
//////////////////////////////////////////////////
CCalcEngine::CCalcEngine(
    bool fPrecedence,
    bool fIntegerMode,
    CalculationManager::IResourceProvider* const pResourceProvider,
    __in_opt ICalcDisplay* pCalcDisplay,
    __in_opt shared_ptr<IHistoryDisplay> pHistoryDisplay)
    : m_fPrecedence(fPrecedence)
    , m_fIntegerMode(fIntegerMode)
    , m_pCalcDisplay(pCalcDisplay)
    , m_resourceProvider(pResourceProvider)
    , m_nOpCode(0)
    , m_nPrevOpCode(0)
    , m_bChangeOp(false)
    , m_bRecord(false)
    , m_bFlagSign(false)
    , m_bSetCalcState(false)
    , m_input(DEFAULT_DEC_SEPARATOR)
    , m_nFE(NumberFormat::Float)
    , m_memoryValue{ make_unique<Rational>() }
    , m_holdVal{}
    , m_currentVal{}
    , m_lastVal{}
    , m_parenVals{}
    , m_precedenceVals{}
    , m_bError(false)
    , m_bInv(false)
    , m_bNoPrevEqu(true)
    , m_radix(DEFAULT_RADIX)
    , m_precision(DEFAULT_PRECISION)
    , m_cIntDigitsSav(DEFAULT_MAX_DIGITS)
    , m_decGrouping()
    , m_numberString(DEFAULT_NUMBER_STR)
    , m_nTempCom(0)
    , m_openParenCount(0)
    , m_nOp()
    , m_nPrecOp()
    , m_precedenceOpCount(0)
    , m_nLastCom(0)
    , m_angletype(AngleType::Degrees)
    , m_numwidth(NUM_WIDTH::QWORD_WIDTH)
    , m_HistoryCollector(pCalcDisplay, pHistoryDisplay, DEFAULT_DEC_SEPARATOR)
    , m_groupSeparator(DEFAULT_GRP_SEPARATOR)
{
    InitChopNumbers();

    m_dwWordBitWidth = DwWordBitWidthFromNumWidth(m_numwidth);

    m_maxTrigonometricNum = RationalMath::Pow(10, 100);

    SetRadixTypeAndNumWidth(RadixType::Decimal, m_numwidth);
    SettingsChanged();
    DisplayNum();
}

void CCalcEngine::InitChopNumbers()
{
    // these rat numbers are set only once and then never change regardless of
    // base or precision changes
    assert(m_chopNumbers.size() >= 4);
    m_chopNumbers[0] = Rational{ rat_qword };
    m_chopNumbers[1] = Rational{ rat_dword };
    m_chopNumbers[2] = Rational{ rat_word };
    m_chopNumbers[3] = Rational{ rat_byte };

    // initialize the max dec number you can support for each of the supported bit lengths
    // this is basically max num in that width / 2 in integer
    assert(m_chopNumbers.size() == m_maxDecimalValueStrings.size());
    for (size_t i = 0; i < m_chopNumbers.size(); i++)
    {
        auto maxVal = m_chopNumbers[i] / 2;
        maxVal = RationalMath::Integer(maxVal);

        m_maxDecimalValueStrings[i] = maxVal.ToString(10, NumberFormat::Float, m_precision);
    }
}

CalcEngine::Rational CCalcEngine::GetChopNumber() const
{
    return m_chopNumbers[static_cast<int>(m_numwidth)];
}

std::wstring CCalcEngine::GetMaxDecimalValueString() const
{
    return m_maxDecimalValueStrings[static_cast<int>(m_numwidth)];
}

// Gets the number in memory for UI to keep it persisted and set it again to a different instance
// of CCalcEngine. Otherwise it will get destructed with the CalcEngine
unique_ptr<Rational> CCalcEngine::PersistedMemObject()
{
    return move(m_memoryValue);
}

void CCalcEngine::PersistedMemObject(Rational const& memObject)
{
    m_memoryValue = make_unique<Rational>(memObject);
}

void CCalcEngine::SettingsChanged()
{
    wchar_t lastDec = m_decimalSeparator;
    wstring decStr = m_resourceProvider->GetCEngineString(L"sDecimal");
    m_decimalSeparator = decStr.empty() ? DEFAULT_DEC_SEPARATOR : decStr.at(0);
    // Until it can be removed, continue to set ratpak decimal here
    SetDecimalSeparator(m_decimalSeparator);

    wchar_t lastSep = m_groupSeparator;
    wstring sepStr = m_resourceProvider->GetCEngineString(L"sThousand");
    m_groupSeparator = sepStr.empty() ? DEFAULT_GRP_SEPARATOR : sepStr.at(0);

    auto lastDecGrouping = m_decGrouping;
    wstring grpStr = m_resourceProvider->GetCEngineString(L"sGrouping");
    m_decGrouping = DigitGroupingStringToGroupingVector(grpStr.empty() ? DEFAULT_GRP_STR : grpStr);

    bool numChanged = false;

    // if the grouping pattern or thousands symbol changed we need to refresh the display
    if (m_decGrouping != lastDecGrouping || m_groupSeparator != lastSep)
    {
        numChanged = true;
    }

    // if the decimal symbol has changed we always do the following things
    if (m_decimalSeparator != lastDec)
    {
        // Re-initialize member variables' decimal point.
        m_input.SetDecimalSymbol(m_decimalSeparator);
        m_HistoryCollector.SetDecimalSymbol(m_decimalSeparator);

        // put the new decimal symbol into the table used to draw the decimal key
        s_engineStrings[SIDS_DECIMAL_SEPARATOR] = m_decimalSeparator;

        // we need to redraw to update the decimal point button
        numChanged = true;
    }

    if (numChanged)
    {
        DisplayNum();
    }
}

wchar_t CCalcEngine::DecimalSeparator() const
{
    return m_decimalSeparator;
}

std::vector<std::shared_ptr<IExpressionCommand>> CCalcEngine::GetHistoryCollectorCommandsSnapshot() const
{
    auto commands = m_HistoryCollector.GetCommands();
    if (!m_HistoryCollector.FOpndAddedToHistory() && m_bRecord)
    {
        commands.push_back(m_HistoryCollector.GetOperandCommandsFromString(m_numberString, m_currentVal));
    }
    return commands;
}
