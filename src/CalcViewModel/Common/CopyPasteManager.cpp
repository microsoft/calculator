// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CopyPasteManager.h"
#include "Common/TraceLogger.h"
#include "Common/LocalizationSettings.h"

using namespace std;
using namespace concurrency;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Foundation::Collections;

StringReference PasteErrorString(L"NoOp");

static const wstring c_validBasicCharacterSet = L"0123456789+-.e";
static const wstring c_validStandardCharacterSet = c_validBasicCharacterSet + L"*/";
static const wstring c_validScientificCharacterSet = c_validStandardCharacterSet + L"()^%";
static const wstring c_validProgrammerCharacterSet = c_validStandardCharacterSet + L"()%abcdfABCDEF";

// The below values can not be "constexpr"-ed,
// as both wstring_view and wchar[] can not be concatenated
// [\s\x85] means white-space characters
static const wstring c_wspc = L"[\\s\\x85]*";
static const wstring c_wspcLParens = c_wspc + L"[(]*" + c_wspc;
static const wstring c_wspcLParenSigned = c_wspc + L"([-+]?[(])*" + c_wspc;
static const wstring c_wspcRParens = c_wspc + L"[)]*" + c_wspc;
static const wstring c_signedDecFloat = L"(?:[-+]?(?:\\d+(\\.\\d*)?|\\.\\d+))";
static const wstring c_optionalENotation = L"(?:e[+-]?\\d+)?";

// Programmer Mode Integer patterns
// Support digit separators ` (WinDbg/MASM), ' (C++), and _ (C# and other languages)
static const wstring c_hexProgrammerChars = L"([a-f]|[A-F]|\\d)+((_|'|`)([a-f]|[A-F]|\\d)+)*";
static const wstring c_decProgrammerChars = L"\\d+((_|'|`)\\d+)*";
static const wstring c_octProgrammerChars = L"[0-7]+((_|'|`)[0-7]+)*";
static const wstring c_binProgrammerChars = L"[0-1]+((_|'|`)[0-1]+)*";
static const wstring c_uIntSuffixes = L"[uU]?[lL]{0,2}";

// RegEx Patterns used by various modes
static const array<wregex, 1> standardModePatterns = { wregex(c_wspc + c_signedDecFloat + c_optionalENotation + c_wspc) };
static const array<wregex, 1> scientificModePatterns = { wregex(
    L"(" + c_wspc + L"[-+]?)|(" + c_wspcLParenSigned + L")" + c_signedDecFloat + c_optionalENotation + c_wspcRParens) };
static const array<array<wregex, 5>, 4> programmerModePatterns = {
    { // Hex numbers like 5F, 4A0C, 0xa9, 0xFFull, 47CDh
      { wregex(c_wspcLParens + L"(0[xX])?" + c_hexProgrammerChars + c_uIntSuffixes + c_wspcRParens),
        wregex(c_wspcLParens + c_hexProgrammerChars + L"[hH]?" + c_wspcRParens) },
      // Decimal numbers like -145, 145, 0n145, 123ull etc
      { wregex(c_wspcLParens + L"[-+]?" + c_decProgrammerChars + L"[lL]{0,2}" + c_wspcRParens),
        wregex(c_wspcLParens + L"(0[nN])?" + c_decProgrammerChars + c_uIntSuffixes + c_wspcRParens) },
      // Octal numbers like 06, 010, 0t77, 0o77, 077ull etc
      { wregex(c_wspcLParens + L"(0[otOT])?" + c_octProgrammerChars + c_uIntSuffixes + c_wspcRParens) },
      // Binary numbers like 011010110, 0010110, 10101001, 1001b, 0b1001, 0y1001, 0b1001ull
      { wregex(c_wspcLParens + L"(0[byBY])?" + c_binProgrammerChars + c_uIntSuffixes + c_wspcRParens),
        wregex(c_wspcLParens + c_binProgrammerChars + L"[bB]?" + c_wspcRParens) } }
};
static const array<wregex, 1> unitConverterPatterns = { wregex(c_wspc + c_signedDecFloat + c_wspc) };

void CopyPasteManager::CopyToClipboard(String ^ stringToCopy)
{
    // Copy the string to the clipboard
    auto dataPackage = ref new DataPackage();
    dataPackage->SetText(stringToCopy);
    Clipboard::SetContent(dataPackage);
}

IAsyncOperation<String ^> ^ CopyPasteManager::GetStringToPaste(ViewMode mode, CategoryGroupType modeType, NumberBase programmerNumberBase, BitLength bitLengthType)
{
    // Retrieve the text in the clipboard
    auto dataPackageView = Clipboard::GetContent();

    // TODO: Support all formats supported by ClipboardHasText
    //-- add support to avoid pasting of expressions like 12 34(as of now we allow 1234)
    //-- add support to allow pasting for expressions like .2 , -.2
    //-- add support to allow pasting for expressions like 1.3e12(as of now we allow 1.3e+12)

    return create_async([dataPackageView, mode, modeType, programmerNumberBase, bitLengthType] {
        return create_task(dataPackageView->GetTextAsync(::StandardDataFormats::Text))
            .then(
                [mode, modeType, programmerNumberBase, bitLengthType](String ^ pastedText) {
                    return ValidatePasteExpression(pastedText, mode, modeType, programmerNumberBase, bitLengthType);
                },
                task_continuation_context::use_arbitrary());
    });
}

bool CopyPasteManager::HasStringToPaste()
{
    return Clipboard::GetContent()->Contains(StandardDataFormats::Text);
}

String ^ CopyPasteManager::ValidatePasteExpression(String ^ pastedText, ViewMode mode, NumberBase programmerNumberBase, BitLength bitLengthType)
{
    return ValidatePasteExpression(pastedText, mode, NavCategory::GetGroupType(mode), programmerNumberBase, bitLengthType);
}

// return "NoOp" if pastedText is invalid else return pastedText
String
    ^ CopyPasteManager::ValidatePasteExpression(
        String ^ pastedText,
        ViewMode mode,
        CategoryGroupType modeType,
        NumberBase programmerNumberBase,
        BitLength bitLengthType)
{
    if (pastedText->Length() > MaxPasteableLength)
    {
        // return NoOp to indicate don't paste anything.
        TraceLogger::GetInstance()->LogError(mode, L"CopyPasteManager::ValidatePasteExpression", L"PastedExpressionSizeGreaterThanMaxAllowed");
        return PasteErrorString;
    }

    // Get english translated expression
    String ^ englishString = LocalizationSettings::GetInstance().GetEnglishValueFromLocalizedDigits(pastedText);

    // Removing the spaces, comma separator from the pasteExpression to allow pasting of expressions like 1  +     2+1,333
    auto pasteExpression = wstring(RemoveUnwantedCharsFromString(englishString)->Data());

    // If the last character is an = sign, remove it from the pasteExpression to allow evaluating the result on paste.
    if (!pasteExpression.empty() && pasteExpression.back() == L'=')
    {
        pasteExpression = pasteExpression.substr(0, pasteExpression.length() - 1);
    }

    // Extract operands from the expression to make regex comparison easy and quick. For whole expression it was taking too much of time.
    // Operands vector will have the list of operands in the pasteExpression
    auto operands = ExtractOperands(StringReference(pasteExpression.c_str()), mode);
    if (operands->Size == 0)
    {
        // return NoOp to indicate don't paste anything.
        return PasteErrorString;
    }

    if (modeType == CategoryGroupType::Converter)
    {
        operands->Clear();
        operands->Append(ref new String(pasteExpression.c_str()));
    }

    // validate each operand with patterns for different modes
    if (!ExpressionRegExMatch(operands, mode, modeType, programmerNumberBase, bitLengthType))
    {
        TraceLogger::GetInstance()->LogError(mode, L"CopyPasteManager::ValidatePasteExpression", L"InvalidExpressionForPresentMode");
        return PasteErrorString;
    }

    return pastedText;
}

IVector<Platform::String ^> ^ CopyPasteManager::ExtractOperands(Platform::String ^ pasteExpression, ViewMode mode)
{
    auto operands = ref new Vector<Platform::String ^>();
    size_t lastIndex = 0;
    bool haveOperator = false;
    bool startExpCounting = false;
    bool startOfExpression = true;
    bool isPreviousOpenParen = false;
    bool isPreviousOperator = false;

    wstring validCharacterSet;
    switch (mode)
    {
    case ViewMode::Standard:
        validCharacterSet = c_validStandardCharacterSet;
        break;
    case ViewMode::Scientific:
        validCharacterSet = c_validScientificCharacterSet;
        break;
    case ViewMode::Programmer:
        validCharacterSet = c_validProgrammerCharacterSet;
        break;
    default:
        validCharacterSet = c_validBasicCharacterSet;
    }

    // This will have the exponent length
    size_t expLength = 0;
    int i = -1;
    for (auto currentChar : pasteExpression)
    {
        ++i;
        // if the current character is not a valid one don't process it
        if (validCharacterSet.find(currentChar) == wstring_view::npos)
        {
            continue;
        }

        if (operands->Size >= MaxOperandCount)
        {
            TraceLogger::GetInstance()->LogError(mode, L"CopyPasteManager::ExtractOperands", L"OperandCountGreaterThanMaxCount");
            operands->Clear();
            return operands;
        }

        if (currentChar >= L'0' && currentChar <= L'9')
        {
            if (startExpCounting)
            {
                expLength++;

                // to disallow pasting of 1e+12345 as 1e+1234, max exponent that can be pasted is 9999.
                if (expLength > MaxExponentLength)
                {
                    TraceLogger::GetInstance()->LogError(mode, L"CopyPasteManager::ExtractOperands", L"ExponentLengthGreaterThanMaxLength");
                    operands->Clear();
                    return operands;
                }
            }
            isPreviousOperator = false;
        }
        else if (currentChar == L'e')
        {
            if (mode != ViewMode::Programmer)
            {
                startExpCounting = true;
            }
            isPreviousOperator = false;
        }
        else if (currentChar == L'+' || currentChar == L'-' || currentChar == L'*' || currentChar == L'/' || currentChar == L'^' || currentChar == L'%')
        {
            if (currentChar == L'+' || currentChar == L'-')
            {
                // don't break the expression into operands if the encountered character corresponds to sign command(+-)
                if (isPreviousOpenParen || startOfExpression || isPreviousOperator
                    || ((mode != ViewMode::Programmer) && !((i != 0) && pasteExpression->Data()[i - 1] != L'e')))
                {
                    isPreviousOperator = false;
                    continue;
                }
            }

            startExpCounting = false;
            expLength = 0;
            haveOperator = true;
            isPreviousOperator = true;
            operands->Append(ref new String(wstring(pasteExpression->Data()).substr(lastIndex, i - lastIndex).c_str()));
            lastIndex = i + 1;
        }
        else
        {
            isPreviousOperator = false;
        }

        isPreviousOpenParen = (currentChar == L'(');
        startOfExpression = false;
    }

    if (!haveOperator)
    {
        operands->Clear();
        operands->Append(pasteExpression);
    }
    else
    {
        operands->Append(ref new String(wstring(pasteExpression->Data()).substr(lastIndex, pasteExpression->Length() - 1).c_str()));
    }

    return operands;
}

bool CopyPasteManager::ExpressionRegExMatch(
    IVector<String ^> ^ operands,
    ViewMode mode,
    CategoryGroupType modeType,
    NumberBase programmerNumberBase,
    BitLength bitLengthType)
{
    if (operands->Size == 0)
    {
        return false;
    }

    vector<wregex> patterns{};

    if (mode == ViewMode::Standard)
    {
        patterns.assign(standardModePatterns.begin(), standardModePatterns.end());
    }
    else if (mode == ViewMode::Scientific)
    {
        patterns.assign(scientificModePatterns.begin(), scientificModePatterns.end());
    }
    else if (mode == ViewMode::Programmer)
    {
        patterns.assign(
            programmerModePatterns[(int)programmerNumberBase - (int)NumberBase::HexBase].begin(),
            programmerModePatterns[(int)programmerNumberBase - (int)NumberBase::HexBase].end());
    }
    else if (modeType == CategoryGroupType::Converter)
    {
        patterns.assign(unitConverterPatterns.begin(), unitConverterPatterns.end());
    }

    auto maxOperandLengthAndValue = GetMaxOperandLengthAndValue(mode, modeType, programmerNumberBase, bitLengthType);
    bool expMatched = true;

    for (const auto& operand : operands)
    {
        // Each operand only needs to match one of the available patterns.
        bool operandMatched = false;
        for (const auto& pattern : patterns)
        {
            operandMatched = operandMatched || regex_match(operand->Data(), pattern);
        }

        if (operandMatched)
        {
            // Remember the sign of the operand
            bool isNegativeValue = operand->Data()[0] == L'-';

            // Remove characters that are valid in the expression but we do not want to include in length calculations
            // or which will break conversion from string-to-ULL.
            auto operandValue = SanitizeOperand(operand);

            // If an operand exceeds the maximum length allowed, break and return.
            if (OperandLength(operandValue, mode, modeType, programmerNumberBase) > maxOperandLengthAndValue.maxLength)
            {
                expMatched = false;
                break;
            }

            // If maxOperandValue is set and the operandValue exceeds it, break and return.
            if (maxOperandLengthAndValue.maxValue != 0)
            {
                auto operandAsULL = TryOperandToULL(operandValue, programmerNumberBase);
                if (operandAsULL == nullptr)
                {
                    // Operand was empty, received invalid_argument, or received out_of_range. Input is invalid.
                    expMatched = false;
                    break;
                }

                // Calculate how much we exceed the maxValue.
                // In case we exceed it for 1 only, and working with negative number - that's a corner case for max signed values (e.g. -32768)
                bool isOverflow = operandAsULL->Value > maxOperandLengthAndValue.maxValue;
                bool isMaxNegativeValue = operandAsULL->Value - 1 == maxOperandLengthAndValue.maxValue;
                if (isOverflow && !(isNegativeValue && isMaxNegativeValue))
                {
                    expMatched = false;
                    break;
                }
            }
        }

        expMatched = expMatched && operandMatched;
    }

    return expMatched;
}

CopyPasteMaxOperandLengthAndValue
CopyPasteManager::GetMaxOperandLengthAndValue(ViewMode mode, CategoryGroupType modeType, NumberBase programmerNumberBase, BitLength bitLengthType)
{
    constexpr size_t defaultMaxOperandLength = 0;
    constexpr uint64_t defaultMaxValue = 0;
    CopyPasteMaxOperandLengthAndValue res;
    if (mode == ViewMode::Standard)
    {
        res.maxLength = MaxStandardOperandLength;
        res.maxValue = defaultMaxValue;
        return res;
    }
    else if (mode == ViewMode::Scientific)
    {
        res.maxLength = MaxScientificOperandLength;
        res.maxValue = defaultMaxValue;
        return res;
    }
    else if (mode == ViewMode::Programmer)
    {
        unsigned int bitLength = 0;
        switch (bitLengthType)
        {
        case BitLength::BitLengthQWord:
            bitLength = 64;
            break;
        case BitLength::BitLengthDWord:
            bitLength = 32;
            break;
        case BitLength::BitLengthWord:
            bitLength = 16;
            break;
        case BitLength::BitLengthByte:
            bitLength = 8;
            break;
        }

        double bitsPerDigit = 0;
        switch (programmerNumberBase)
        {
        case NumberBase::BinBase:
            bitsPerDigit = log2(2);
            break;
        case NumberBase::OctBase:
            bitsPerDigit = log2(8);
            break;
        case NumberBase::DecBase:
            bitsPerDigit = log2(10);
            break;
        case NumberBase::HexBase:
            bitsPerDigit = log2(16);
            break;
        }

        unsigned int signBit = (programmerNumberBase == NumberBase::DecBase) ? 1 : 0;

        const auto maxLength = static_cast<unsigned int>(ceil((bitLength - signBit) / bitsPerDigit));
        const uint64_t maxValue = UINT64_MAX >> (MaxProgrammerBitLength - (bitLength - signBit));

        res.maxLength = maxLength;
        res.maxValue = maxValue;
        return res;
    }
    else if (modeType == CategoryGroupType::Converter)
    {
        res.maxLength = MaxConverterInputLength;
        res.maxValue = defaultMaxValue;
        return res;
    }

    res.maxLength = defaultMaxOperandLength;
    res.maxValue = defaultMaxValue;
    return res;
}

Platform::String ^ CopyPasteManager::SanitizeOperand(Platform::String ^ operand)
{
    constexpr wchar_t unWantedChars[] = { L'\'', L'_', L'`', L'(', L')', L'-', L'+' };

    return ref new String(Utils::RemoveUnwantedCharsFromString(operand->Data(), unWantedChars).c_str());
}

IBox<unsigned long long int> ^ CopyPasteManager::TryOperandToULL(String ^ operand, NumberBase numberBase)
{
    if (operand->Length() == 0 || operand->Data()[0] == L'-')
    {
        return nullptr;
    }

    int intBase;
    switch (numberBase)
    {
    case NumberBase::HexBase:
        intBase = 16;
        break;
    case NumberBase::OctBase:
        intBase = 8;
        break;
    case NumberBase::BinBase:
        intBase = 2;
        break;
    default:
    case NumberBase::DecBase:
        intBase = 10;
        break;
    }

    wstring::size_type size = 0;
    try
    {
        return stoull(operand->Data(), &size, intBase);
    }
    catch (const invalid_argument&)
    {
        // Do nothing
    }
    catch (const out_of_range&)
    {
        // Do nothing
    }

    return nullptr;
}

ULONG32 CopyPasteManager::OperandLength(Platform::String ^ operand, ViewMode mode, CategoryGroupType modeType, NumberBase programmerNumberBase)
{
    if (modeType == CategoryGroupType::Converter)
    {
        return operand->Length();
    }

    switch (mode)
    {
    case ViewMode::Standard:
    case ViewMode::Scientific:
        return StandardScientificOperandLength(operand);

    case ViewMode::Programmer:
        return ProgrammerOperandLength(operand, programmerNumberBase);

    default:
        return 0;
    }
}

ULONG32 CopyPasteManager::StandardScientificOperandLength(Platform::String ^ operand)
{
    auto operandWstring = wstring(operand->Data());
    const bool hasDecimal = operandWstring.find('.') != wstring::npos;
    auto length = operandWstring.length();

    if (hasDecimal)
    {
        if (length >= 2)
        {
            if ((operandWstring[0] == L'0') && (operandWstring[1] == L'.'))
            {
                length -= 2;
            }
            else
            {
                length -= 1;
            }
        }
    }

    auto exponentPos = operandWstring.find('e');
    const bool hasExponent = exponentPos != wstring::npos;
    if (hasExponent)
    {
        auto expLength = operandWstring.substr(exponentPos).length();
        length -= expLength;
    }

    return static_cast<ULONG32>(length);
}

ULONG32 CopyPasteManager::ProgrammerOperandLength(Platform::String ^ operand, NumberBase numberBase)
{
    vector<wstring> prefixes{};
    vector<wstring> suffixes{};
    switch (numberBase)
    {
    case NumberBase::BinBase:
        prefixes = { L"0B", L"0Y" };
        suffixes = { L"B" };
        break;
    case NumberBase::DecBase:
        prefixes = { L"-", L"0N" };
        break;
    case NumberBase::OctBase:
        prefixes = { L"0T", L"0O" };
        break;
    case NumberBase::HexBase:
        prefixes = { L"0X" };
        suffixes = { L"H" };
        break;
    default:
        // No defined prefixes/suffixes
        return 0;
    }

    // UInt suffixes are common across all modes
    const array<wstring, 5> uintSuffixes = { L"ULL", L"UL", L"LL", L"U", L"L" };
    suffixes.insert(suffixes.end(), uintSuffixes.begin(), uintSuffixes.end());

    wstring operandUpper = wstring(operand->Data());
    transform(operandUpper.begin(), operandUpper.end(), operandUpper.begin(), towupper);

    size_t len = operand->Length();

    // Detect if there is a suffix and subtract its length
    // Check suffixes first to allow e.g. "0b" to result in length 1 (value 0), rather than length 0 (no value).
    for (const auto& suffix : suffixes)
    {
        if (len < suffix.length())
        {
            continue;
        }

        if (operandUpper.compare(operandUpper.length() - suffix.length(), suffix.length(), suffix) == 0)
        {
            len -= suffix.length();
            break;
        }
    }

    // Detect if there is a prefix and subtract its length
    for (const auto& prefix : prefixes)
    {
        if (len < prefix.length())
        {
            continue;
        }

        if (operandUpper.compare(0, prefix.length(), prefix) == 0)
        {
            len -= prefix.length();
            break;
        }
    }

    return static_cast<ULONG32>(len);
}

// return wstring after removing characters like space, comma, double quotes, and monetary prefix currency symbols supported by the Windows keyboard:
// yen or yuan(¥) - 165
// unspecified currency sign(¤) - 164
// Ghanaian cedi(₵) - 8373
// dollar or peso($) - 36
// colón(₡) - 8353
// won(₩) - 8361
// shekel(₪) - 8362
// naira(₦) - 8358
// Indian rupee(₹) - 8377
// pound(£) - 163
// euro(€) - 8364
// non-breaking whitespace - 160
Platform::String ^ CopyPasteManager::RemoveUnwantedCharsFromString(Platform::String ^ input)
{
    constexpr wchar_t unWantedChars[] = { L' ', L',', L'"', 165, 164, 8373, 36, 8353, 8361, 8362, 8358, 8377, 163, 8364, 8234, 8235, 8236, 8237, 160 };
    input = CalculatorApp::Common::LocalizationSettings::GetInstance().RemoveGroupSeparators(input);
    return ref new String(Utils::RemoveUnwantedCharsFromString(input->Data(), unWantedChars).c_str());
}

bool CopyPasteManager::IsErrorMessage(Platform::String ^ message)
{
    return message == PasteErrorString;
}
