// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/****************************Module*Header***********************************\
* Module Name: SCIDISP.C
*
* Module Description:
*
* Warnings:
*
* Created:
*
* Author:
\****************************************************************************/
#include "pch.h"
#include "Header Files/CalcEngine.h"

using namespace std;
using namespace CalcEngine;

constexpr int MAX_EXPONENT = 4;
constexpr uint32_t MAX_GROUPING_SIZE = 16;
constexpr wstring_view c_decPreSepStr = L"[+-]?(\\d*)[";
constexpr wstring_view c_decPostSepStr = L"]?(\\d*)(?:e[+-]?(\\d*))?$";


/****************************************************************************\
* void DisplayNum(void)
*
* Convert m_currentVal to a string in the current radix.
*
* Updates the following variables:
*   m_currentVal, m_numberString
\****************************************************************************/
//
// State of calc last time DisplayNum was called
//
typedef struct {
    Rational value;
    int32_t precision;
    uint32_t radix;
    INT         nFE;
    NUM_WIDTH   numwidth;
    bool        fIntMath;
    bool        bRecord;
    bool        bUseSep;
} LASTDISP;

LASTDISP gldPrevious = { 0, -1, 0, -1, (NUM_WIDTH)-1, false, false, false };

// Truncates if too big, makes it a non negative - the number in rat. Doesn't do anything if not in INT mode
CalcEngine::Rational CCalcEngine::TruncateNumForIntMath(CalcEngine::Rational const& rat)
{
    if (!m_fIntegerMode)
    {
        return rat;
    }

    // Truncate to an integer. Do not round here.
    auto result = RationalMath::Integer(rat);

    // Can be converting a dec negative number to Hex/Oct/Bin rep. Use 2's complement form
    // Check the range.
    if (result < 0)
    {
        // if negative make positive by doing a twos complement
        result = -(result) - 1;
        result ^= m_chopNumbers[m_numwidth];
    }

    result &= m_chopNumbers[m_numwidth];

    return result;
}

void CCalcEngine::DisplayNum(void)
{
    //
    // Only change the display if
    //  we are in record mode                               -OR-
    //  this is the first time DisplayNum has been called,  -OR-
    //  something important has changed since the last time DisplayNum was
    //  called.
    //
    if (m_bRecord ||
        gldPrevious.value != m_currentVal ||
        gldPrevious.precision != m_precision ||
        gldPrevious.radix != m_radix ||
        gldPrevious.nFE != (int)m_nFE ||
        gldPrevious.bUseSep != true ||
        gldPrevious.numwidth != m_numwidth ||
        gldPrevious.fIntMath != m_fIntegerMode ||
        gldPrevious.bRecord != m_bRecord)
    {
        gldPrevious.precision = m_precision;
        gldPrevious.radix = m_radix;
        gldPrevious.nFE = (int)m_nFE;
        gldPrevious.numwidth = m_numwidth;

        gldPrevious.fIntMath = m_fIntegerMode;
        gldPrevious.bRecord = m_bRecord;
        gldPrevious.bUseSep = true;

        if (m_bRecord)
        {
            // Display the string and return.
            m_numberString = m_input.ToString(m_radix);
        }
        else
        {
            // If we're in Programmer mode, perform integer truncation so e.g. 5 / 2 * 2 results in 4, not 5.
            if (m_fIntegerMode)
            {
                m_currentVal = TruncateNumForIntMath(m_currentVal);
            }
            m_numberString = GetStringForDisplay(m_currentVal, m_radix);
        }

        // Displayed number can go through transformation. So copy it after transformation
        gldPrevious.value = m_currentVal;

        if ((m_radix == 10) && IsNumberInvalid(m_numberString, MAX_EXPONENT, m_precision, m_radix))
        {
            DisplayError(CALC_E_OVERFLOW);
        }
        else
        {
            // Display the string and return.
            SetPrimaryDisplay(GroupDigitsPerRadix(m_numberString, m_radix));
        }
    }
}

int CCalcEngine::IsNumberInvalid(const wstring& numberString, int iMaxExp, int iMaxMantissa, uint32_t radix) const
{
    int iError = 0;

    if (radix == 10)
    {
        // start with an optional + or -
        // followed by zero or more digits
        // followed by an optional decimal point
        // followed by zero or more digits
        // followed by an optional exponent
        // in case there's an exponent:
        //      its optionally followed by a + or -
        //      which is followed by zero or more digits
        wregex rx(wstring{ c_decPreSepStr } + m_decimalSeparator + wstring{ c_decPostSepStr });
        wsmatch matches;
        if (regex_match(numberString, matches, rx))
        {
            // Check that exponent isn't too long
            if (matches.length(3) > iMaxExp)
            {
                iError = IDS_ERR_INPUT_OVERFLOW;
            }
            else
            {
                wstring exp = matches.str(1);
                auto intItr = exp.begin();
                auto intEnd = exp.end();
                while (intItr != intEnd && *intItr == L'0')
                {
                    intItr++;
                }

                auto iMantissa = distance(intItr, intEnd) + matches.length(2);
                if (iMantissa > iMaxMantissa)
                {
                    iError = IDS_ERR_INPUT_OVERFLOW;
                }
            }
        }
        else
        {
            iError = IDS_ERR_UNK_CH;
        }
    }
    else
    {
        for (const wchar_t& c : numberString)
        {
            if (radix == 16)
            {
                if (!(iswdigit(c) || (c >= L'A' && c <= L'F')))
                {
                    iError = IDS_ERR_UNK_CH;
                }
            }
            else if (c < L'0' || c >= L'0' + radix)
            {
                iError = IDS_ERR_UNK_CH;
            }
        }
    }

    return iError;
}

/****************************************************************************\
*
* DigitGroupingStringToGroupingVector
*
* Description:
*   This will take the digit grouping string found in the regional applet and
*   represent this string as a vector.
*
*   groupingString
*   0;0      - no grouping
*   3;0      - group every 3 digits
*   3        - group 1st 3, then no grouping after
*   3;0;0    - group 1st 3, then no grouping after
*   3;2;0    - group 1st 3 and then every 2 digits
*   4;0      - group every 4 digits
*   5;3;2;0  - group 5, then 3, then every 2
*   5;3;2    - group 5, then 3, then 2, then no grouping after
*
* Returns: the groupings as a vector
*
\****************************************************************************/
vector<uint32_t> CCalcEngine::DigitGroupingStringToGroupingVector(wstring_view groupingString)
{
    vector<uint32_t> grouping{};
    uint32_t currentGroup = 0;
    wchar_t* next = nullptr;
    for (const wchar_t* itr = groupingString.data(); *itr != L'\0'; ++itr)
    {
        // Try to parse a grouping number from the string
        currentGroup = wcstoul(itr, &next, 10);

        // If we successfully parsed a group, add it to the grouping.
        if (currentGroup < MAX_GROUPING_SIZE)
        {
            grouping.emplace_back(currentGroup);
        }

        // If we found a grouping and aren't at the end of the string yet,
        // jump to the next position in the string (the ';').
        // The loop will then increment us to the next character, which should be a number.
        if (next && (static_cast<size_t>(next - groupingString.data()) < groupingString.length()))
        {
            itr = next;
        }
    }

    return grouping;
}

wstring CCalcEngine::GroupDigitsPerRadix(wstring_view numberString, uint32_t radix)
{
    if (numberString.empty())
    {
        return wstring{};
    }

    switch (radix)
    {
    case 10:
        return GroupDigits(wstring{ m_groupSeparator }, m_decGrouping, numberString, (L'-' == numberString[0]));
    case 8:
        return GroupDigits(L" ", { 3, 0 }, numberString);
    case 2:
    case 16:
        return GroupDigits(L" ", { 4, 0 }, numberString);
    default:
        return wstring{ numberString };
    }
}

/****************************************************************************\
*
* GroupDigits
*
* Description:
*   This routine will take a grouping vector and the display string and
*   add the separator according to the pattern indicated by the separator.
*
*   Grouping
*   0,0      - no grouping
*   3,0      - group every 3 digits
*   3        - group 1st 3, then no grouping after
*   3,0,0    - group 1st 3, then no grouping after
*   3,2,0    - group 1st 3 and then every 2 digits
*   4,0      - group every 4 digits
*   5,3,2,0  - group 5, then 3, then every 2
*   5,3,2    - group 5, then 3, then 2, then no grouping after
*
\***************************************************************************/
wstring CCalcEngine::GroupDigits(wstring_view delimiter, vector<uint32_t> const& grouping, wstring_view displayString, bool isNumNegative)
{
    // if there's nothing to do, bail
    if (delimiter.empty() || grouping.empty())
    {
        return wstring{ displayString };
    }

    // Find the position of exponential 'e' in the string
    size_t exp = displayString.find(L'e');
    bool hasExponent = (exp != wstring_view::npos);

    // Find the position of decimal point in the string
    size_t dec = displayString.find(m_decimalSeparator);
    bool hasDecimal = (dec != wstring_view::npos);

    // Create an iterator that points to the end of the portion of the number subject to grouping (i.e. left of the decimal)
    auto ritr = displayString.rend();
    if (hasDecimal)
    {
        ritr -= dec;
    }
    else if (hasExponent)
    {
        ritr -= exp;
    }
    else
    {
        ritr = displayString.rbegin();
    }

    wstringstream groupedStream{};
    uint32_t groupingSize = 0;

    auto groupItr = grouping.begin();
    auto currGrouping = *groupItr;
    // Mark the 'end' of the string as either rend() or rend()-1 if there is a negative sign
    // We exclude the sign here because we don't want to end up with e.g. "-,123,456"
    // Then, iterate from back to front, adding group delimiters as needed.
    auto reverse_end = displayString.rend() - (isNumNegative ? 1 : 0);
    while (ritr != reverse_end)
    {
        groupedStream << *ritr++;
        groupingSize++;

        // If a group is complete, add a separator
        // Do not add a separator if:
        // - grouping size is 0
        // - we are at the end of the digit string
        if (currGrouping != 0 && (groupingSize % currGrouping) == 0 && ritr != reverse_end)
        {
            groupedStream << wstring{ delimiter };
            groupingSize = 0; // reset for a new group

            // Shift the grouping to next values if they exist
            if (groupItr != grouping.end())
            {
                ++groupItr;

                // Loop through grouping vector until we find a non-zero value.
                // "0" values may appear in a form of either e.g. "3;0" or "3;0;0".
                // A 0 in the last position means repeat the previous grouping.
                // A 0 in another position is a group. So, "3;0;0" means "group 3, then group 0 repeatedly"
                // This could be expressed as just "3" but GetLocaleInfo is returning 3;0;0 in some cases instead.
                for (currGrouping = 0; groupItr != grouping.end(); ++groupItr)
                {
                    // If it's a non-zero value, that's our new group
                    if (*groupItr != 0)
                    {
                        currGrouping = *groupItr;
                        break;
                    }

                    // Otherwise, save the previous grouping in case we need to repeat it
                    currGrouping = *(groupItr - 1);
                }
            }
        }
    }

    // now copy the negative sign if it is there
    if (isNumNegative)
    {
        groupedStream << displayString[0];
    }

    auto groupedString = groupedStream.str();
    wstring result(groupedString.rbegin(), groupedString.rend());
    // Add the right (fractional or exponential) part of the number to the final string.
    if (hasDecimal)
    {
        result += displayString.substr(dec);
    }
    else if (hasExponent)
    {
        result += displayString.substr(exp);
    }

    return result;
}
