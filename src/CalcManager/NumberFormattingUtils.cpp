#include "pch.h"
#include "NumberFormattingUtils.h"

using namespace std;

namespace UnitConversionManager::NumberFormattingUtils
{
    /// <summary>
    /// Trims out any trailing zeros or decimals in the given input string
    /// </summary>
    /// <param name="number">number to trim</param>
    void TrimTrailingZeros(_Inout_ wstring& number)
    {
        //check if the number contains a decimal point.
        if (number.find(L'.') == wstring::npos)
        {
            return; //no trailing zeros or decimal to trim.
        }

        //find the last non-zero digit and erase and triling zeros.
        if (auto i = number.find_last_not_of(L'0'); i != wstring::npos)
        {
            number.erase(number.cbegin() + i + 1, number.cend());
        }
        //remove the trailing decimal point if it exists.
        if (number.back() == L'.')
        {
            number.pop_back();
        }
    }

    /// <summary>
    /// Get number of digits (whole number part + decimal part)</summary>
    /// <param name="value">the number</param>
    unsigned int GetNumberDigits(wstring value)
    {
        //trim trailing zeros and decimal point.
        TrimTrailingZeros(value);
        unsigned int numberSignificantDigits = static_cast<unsigned int>(value.size());
        //if the number contains a decimal point, reduce the count by one.
        if (value.find(L'.') != wstring::npos)
        {
            --numberSignificantDigits;
        }
        //if the number is negative, reduce the count by one
        if (value.find(L'-') != wstring::npos)
        {
            --numberSignificantDigits;
        }
        return numberSignificantDigits;
    }

    /// <summary>
    /// Get number of digits (whole number part only)</summary>
    /// <param name="value">the number</param>
    unsigned int GetNumberDigitsWholeNumberPart(double value)
    {
        return value == 0 ? 1u : static_cast<unsigned int>(1 + max(0.0, log10(abs(value))));
    }

    /// <summary>
    /// Rounds the given double to the given number of significant digits
    /// </summary>
    /// <param name="num">input double</param>
    /// <param name="numSignificant">unsigned int number of significant digits to round to</param>
    wstring RoundSignificantDigits(double num, unsigned int numSignificant)
    {
        wstringstream out(wstringstream::out);
        out << fixed;
        out.precision(numSignificant);
        out << num;
        return out.str();
    }

    /// <summary>
    ///  Convert a Number to Scientific Notation
    /// </summary>
    /// <param name="number">number to convert</param>
    wstring ToScientificNumber(double number)
    {
        wstringstream out(wstringstream::out);
        out << scientific << number;
        return out.str();
    }
}
