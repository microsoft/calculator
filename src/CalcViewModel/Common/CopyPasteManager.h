// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AppResourceProvider.h"
#include "NavCategory.h"
#include "BitLength.h"

namespace CalculatorUnitTests
{
    class CopyPasteManagerTest;
}

namespace CalculatorApp
{
    inline constexpr auto HexBase = 5;
    inline constexpr auto DecBase = 6;
    inline constexpr auto OctBase = 7;
    inline constexpr auto BinBase = 8;

    class CopyPasteManager
    {
    public:
        static void CopyToClipboard(Platform::String ^ stringToCopy);
        static concurrency::task<Platform::String ^> GetStringToPaste(
            CalculatorApp::Common::ViewMode mode,
            CalculatorApp::Common::CategoryGroupType modeType,
            int programmerNumberBase = -1,
            CalculatorApp::Common::BitLength bitLengthType = CalculatorApp::Common::BitLength::BitLengthUnknown);
        static bool HasStringToPaste()
        {
            return ClipboardTextFormat() >= 0;
        }

        static constexpr auto PasteErrorString = L"NoOp";

    private:
        static int ClipboardTextFormat();
        static Platform::String
            ^ ValidatePasteExpression(
                Platform::String ^ pastedText,
                CalculatorApp::Common::ViewMode mode,
                int programmerNumberBase,
                CalculatorApp::Common::BitLength bitLengthType);
        static Platform::String
            ^ ValidatePasteExpression(
                Platform::String ^ pastedText,
                CalculatorApp::Common::ViewMode mode,
                CalculatorApp::Common::CategoryGroupType modeType,
                int programmerNumberBase,
                CalculatorApp::Common::BitLength bitLengthType);

        static std::vector<std::wstring>
        ExtractOperands(const std::wstring& pasteExpression, CalculatorApp::Common::ViewMode mode);
        static bool ExpressionRegExMatch(
            std::vector<std::wstring> operands,
            CalculatorApp::Common::ViewMode mode,
            CalculatorApp::Common::CategoryGroupType modeType,
            int programmerNumberBase = -1,
            CalculatorApp::Common::BitLength bitLengthType = CalculatorApp::Common::BitLength::BitLengthUnknown);

        static std::pair<size_t, uint64_t> GetMaxOperandLengthAndValue(
            CalculatorApp::Common::ViewMode mode,
            CalculatorApp::Common::CategoryGroupType modeType,
            int programmerNumberBase = -1,
            CalculatorApp::Common::BitLength bitLengthType = CalculatorApp::Common::BitLength::BitLengthUnknown);
        static std::wstring SanitizeOperand(const std::wstring& operand);
        static bool TryOperandToULL(const std::wstring& operand, int numberBase, unsigned long long int& result);
        static size_t OperandLength(
            const std::wstring& operand,
            CalculatorApp::Common::ViewMode mode,
            CalculatorApp::Common::CategoryGroupType modeType,
            int programmerNumberBase = -1);
        static size_t StandardScientificOperandLength(const std::wstring& operand);
        static size_t ProgrammerOperandLength(const std::wstring& operand, int numberBase);
        static std::wstring RemoveUnwantedCharsFromWstring(const std::wstring& input);

        static constexpr size_t MaxStandardOperandLength = 16;
        static constexpr size_t MaxScientificOperandLength = 32;
        static constexpr size_t MaxConverterInputLength = 16;
        static constexpr size_t MaxOperandCount = 100;
        static constexpr size_t MaxPasteableLength = 512;
        static constexpr size_t MaxExponentLength = 4;
        static constexpr size_t MaxProgrammerBitLength = 64;

        static Platform::String ^ supportedFormats[];

        friend class CalculatorUnitTests::CopyPasteManagerTest;
    };
}
