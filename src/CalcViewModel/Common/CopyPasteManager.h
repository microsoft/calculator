// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AppResourceProvider.h"
#include "NavCategory.h"
#include "BitLength.h"
#include "NumberBase.h"

namespace CalculatorUnitTests
{
    class CopyPasteManagerTest;
}

namespace CalculatorApp::ViewModel::Common
{
    public value struct CopyPasteMaxOperandLengthAndValue
    {
        unsigned int maxLength;
        unsigned long long maxValue;
    };

    public ref class CopyPasteManager sealed
    {
    public:
        static void CopyToClipboard(Platform::String ^ stringToCopy);
        static Windows::Foundation::IAsyncOperation<Platform::String ^> ^ GetStringToPaste(
            CalculatorApp::ViewModel::Common::ViewMode mode,
            CalculatorApp::ViewModel::Common::CategoryGroupType modeType,
            CalculatorApp::ViewModel::Common::NumberBase programmerNumberBase,
            CalculatorApp::ViewModel::Common::BitLength bitLengthType);
        static bool HasStringToPaste();
        static bool IsErrorMessage(Platform::String ^ message);
        static property unsigned int MaxPasteableLength
        {
            unsigned int get()
            {
                return MaxPasteableLengthValue;
            }
        }
        static property unsigned int MaxOperandCount
        {
            unsigned int get()
            {
                return MaxOperandCountValue;
            }
        }
        static property unsigned int MaxStandardOperandLength
        {
            unsigned int get()
            {
                return MaxStandardOperandLengthValue;
            }
        }
        static property unsigned int MaxScientificOperandLength
        {
            unsigned int get()
            {
                return MaxScientificOperandLengthValue;
            }
        }

        static property unsigned int MaxConverterInputLength
        {
            unsigned int get()
            {
                return MaxConverterInputLengthValue;
            }
        }

        static property unsigned int MaxExponentLength
        {
            unsigned int get()
            {
                return MaxExponentLengthValue;
            }
        }

        static property unsigned int MaxProgrammerBitLength
        {
            unsigned int get()
            {
                return MaxProgrammerBitLengthValue;
            }
        }

        static Platform::String
            ^ ValidatePasteExpression(
                Platform::String ^ pastedText,
                CalculatorApp::ViewModel::Common::ViewMode mode,
                CalculatorApp::ViewModel::Common::NumberBase programmerNumberBase,
                CalculatorApp::ViewModel::Common::BitLength bitLengthType);
        static Platform::String
            ^ ValidatePasteExpression(
                Platform::String ^ pastedText,
                CalculatorApp::ViewModel::Common::ViewMode mode,
                CalculatorApp::ViewModel::Common::CategoryGroupType modeType,
                CalculatorApp::ViewModel::Common::NumberBase programmerNumberBase,
                CalculatorApp::ViewModel::Common::BitLength bitLengthType);
        static CopyPasteMaxOperandLengthAndValue GetMaxOperandLengthAndValue(
            CalculatorApp::ViewModel::Common::ViewMode mode,
            CalculatorApp::ViewModel::Common::CategoryGroupType modeType,
            CalculatorApp::ViewModel::Common::NumberBase programmerNumberBase,
            CalculatorApp::ViewModel::Common::BitLength bitLengthType);
        static Windows::Foundation::Collections::IVector<
            Platform::String ^> ^ ExtractOperands(Platform::String ^ pasteExpression, CalculatorApp::ViewModel::Common::ViewMode mode);
        static bool ExpressionRegExMatch(
            Windows::Foundation::Collections::IVector<Platform::String ^> ^ operands,
            CalculatorApp::ViewModel::Common::ViewMode mode,
            CalculatorApp::ViewModel::Common::CategoryGroupType modeType,
            CalculatorApp::ViewModel::Common::NumberBase programmerNumberBase,
            CalculatorApp::ViewModel::Common::BitLength bitLengthType);
        static Platform::String ^ SanitizeOperand(Platform::String ^ operand);
        static Platform::String ^ RemoveUnwantedCharsFromString(Platform::String ^ input);
        static Platform::IBox<unsigned long long int> ^ TryOperandToULL(Platform::String ^ operand, CalculatorApp::ViewModel::Common::NumberBase numberBase);
        static ULONG32 StandardScientificOperandLength(Platform::String ^ operand);
        static ULONG32 OperandLength(
            Platform::String ^ operand,
            CalculatorApp::ViewModel::Common::ViewMode mode,
            CalculatorApp::ViewModel::Common::CategoryGroupType modeType,
            CalculatorApp::ViewModel::Common::NumberBase programmerNumberBase);
        static ULONG32 ProgrammerOperandLength(Platform::String ^ operand, CalculatorApp::ViewModel::Common::NumberBase numberBase);

    private:
        static constexpr size_t MaxStandardOperandLengthValue = 16;
        static constexpr size_t MaxScientificOperandLengthValue = 32;
        static constexpr size_t MaxConverterInputLengthValue = 16;
        static constexpr size_t MaxOperandCountValue = 100;
        static constexpr size_t MaxExponentLengthValue = 4;
        static constexpr size_t MaxProgrammerBitLengthValue = 64;
        static constexpr size_t MaxPasteableLengthValue = 512;
    };
}
