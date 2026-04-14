// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;

namespace CalculatorApp.ViewModel.Common
{
    public struct CopyPasteMaxOperandLengthAndValue
    {
        public uint MaxLength;
        public ulong MaxValue;
    }

    public sealed class CopyPasteManager
    {
        private const string PasteErrorString = "NoOp";
        private const int MaxStandardOperandLengthValue = 16;
        private const int MaxScientificOperandLengthValue = 32;
        private const int MaxConverterInputLengthValue = 16;
        private const int MaxOperandCountValue = 100;
        private const int MaxExponentLengthValue = 4;
        private const int MaxProgrammerBitLengthValue = 64;
        private const int MaxPasteableLengthValue = 512;

        private static readonly string c_validBasicCharacterSet = "0123456789+-.e";
        private static readonly string c_validStandardCharacterSet = c_validBasicCharacterSet + "*/";
        private static readonly string c_validScientificCharacterSet = c_validStandardCharacterSet + "()^%";
        private static readonly string c_validProgrammerCharacterSet = c_validStandardCharacterSet + "()%abcdfABCDEF";

        private static readonly string c_wspc = @"[\s\x85]*";
        private static readonly string c_wspcLParens = c_wspc + @"[(]*" + c_wspc;
        private static readonly string c_wspcLParenSigned = c_wspc + @"([-+]?[(])*" + c_wspc;
        private static readonly string c_wspcRParens = c_wspc + @"[)]*" + c_wspc;
        private static readonly string c_signedDecFloat = @"(?:[-+]?(?:\d+(\.\d*)?|\.\d+))";
        private static readonly string c_optionalENotation = @"(?:e[+-]?\d+)?";

        // Programmer Mode Integer patterns
        private static readonly string c_hexProgrammerChars = @"([a-f]|[A-F]|\d)+((_|'|`)([a-f]|[A-F]|\d)+)*";
        private static readonly string c_decProgrammerChars = @"\d+((_|'|`)\d+)*";
        private static readonly string c_octProgrammerChars = @"[0-7]+((_|'|`)[0-7]+)*";
        private static readonly string c_binProgrammerChars = @"[0-1]+((_|'|`)[0-1]+)*";
        private static readonly string c_uIntSuffixes = @"[uU]?[lL]{0,2}";

        private static readonly Regex[] s_standardModePatterns = new[]
        {
            new Regex("^" + c_wspc + c_signedDecFloat + c_optionalENotation + c_wspc + "$")
        };

        private static readonly Regex[] s_scientificModePatterns = new[]
        {
            new Regex("^(" + c_wspc + @"[-+]?)|(" + c_wspcLParenSigned + ")" + c_signedDecFloat + c_optionalENotation + c_wspcRParens + "$")
        };

        private static readonly Regex[][] s_programmerModePatterns = new[]
        {
            // Hex
            new[]
            {
                new Regex("^" + c_wspcLParens + "(0[xX])?" + c_hexProgrammerChars + c_uIntSuffixes + c_wspcRParens + "$"),
                new Regex("^" + c_wspcLParens + c_hexProgrammerChars + "[hH]?" + c_wspcRParens + "$")
            },
            // Dec
            new[]
            {
                new Regex("^" + c_wspcLParens + @"[-+]?" + c_decProgrammerChars + @"[lL]{0,2}" + c_wspcRParens + "$"),
                new Regex("^" + c_wspcLParens + "(0[nN])?" + c_decProgrammerChars + c_uIntSuffixes + c_wspcRParens + "$")
            },
            // Oct
            new[]
            {
                new Regex("^" + c_wspcLParens + "(0[otOT])?" + c_octProgrammerChars + c_uIntSuffixes + c_wspcRParens + "$")
            },
            // Bin
            new[]
            {
                new Regex("^" + c_wspcLParens + "(0[byBY])?" + c_binProgrammerChars + c_uIntSuffixes + c_wspcRParens + "$"),
                new Regex("^" + c_wspcLParens + c_binProgrammerChars + "[bB]?" + c_wspcRParens + "$")
            }
        };

        private static readonly Regex[] s_unitConverterPatterns = new[]
        {
            new Regex("^" + c_wspc + c_signedDecFloat + c_wspc + "$")
        };

        public static uint MaxPasteableLength => MaxPasteableLengthValue;
        public static uint MaxOperandCount => MaxOperandCountValue;
        public static uint MaxStandardOperandLength => MaxStandardOperandLengthValue;
        public static uint MaxScientificOperandLength => MaxScientificOperandLengthValue;
        public static uint MaxConverterInputLength => MaxConverterInputLengthValue;
        public static uint MaxExponentLength => MaxExponentLengthValue;
        public static uint MaxProgrammerBitLength => MaxProgrammerBitLengthValue;

        public static void CopyToClipboard(string stringToCopy)
        {
            var dataPackage = new DataPackage();
            dataPackage.SetText(stringToCopy);
            Clipboard.SetContentWithOptions(dataPackage, null);
        }

        public static async Task<string> GetStringToPaste(
            ViewMode mode,
            CategoryGroupType modeType,
            NumberBase programmerNumberBase,
            BitLength bitLengthType)
        {
            var dataPackageView = Clipboard.GetContent();
            string pastedText = await dataPackageView.GetTextAsync(StandardDataFormats.Text);
            return ValidatePasteExpression(pastedText, mode, modeType, programmerNumberBase, bitLengthType);
        }

        public static bool HasStringToPaste()
        {
            return Clipboard.GetContent().Contains(StandardDataFormats.Text);
        }

        public static bool IsErrorMessage(string message)
        {
            return message == PasteErrorString;
        }

        public static string ValidatePasteExpression(
            string pastedText,
            ViewMode mode,
            NumberBase programmerNumberBase,
            BitLength bitLengthType)
        {
            return ValidatePasteExpression(pastedText, mode, NavCategoryStates.GetGroupType(mode), programmerNumberBase, bitLengthType);
        }

        public static string ValidatePasteExpression(
            string pastedText,
            ViewMode mode,
            CategoryGroupType modeType,
            NumberBase programmerNumberBase,
            BitLength bitLengthType)
        {
            if (pastedText.Length > MaxPasteableLengthValue)
            {
                TraceLogger.GetInstance().LogError(mode, "CopyPasteManager::ValidatePasteExpression", "PastedExpressionSizeGreaterThanMaxAllowed");
                return PasteErrorString;
            }

            // Get english translated expression
            string englishString = LocalizationSettings.GetInstance().GetEnglishValueFromLocalizedDigits(pastedText);

            // Remove spaces, comma separator from the pasteExpression
            string pasteExpression = RemoveUnwantedCharsFromString(englishString);

            // If the last character is an = sign, remove it
            if (pasteExpression.Length > 0 && pasteExpression[pasteExpression.Length - 1] == '=')
            {
                pasteExpression = pasteExpression.Substring(0, pasteExpression.Length - 1);
            }

            // Extract operands from the expression
            var operands = ExtractOperands(pasteExpression, mode);
            if (operands.Count == 0)
            {
                return PasteErrorString;
            }

            if (modeType == CategoryGroupType.Converter)
            {
                operands.Clear();
                operands.Add(pasteExpression);
            }

            // Validate each operand with patterns for different modes
            if (!ExpressionRegExMatch(operands, mode, modeType, programmerNumberBase, bitLengthType))
            {
                TraceLogger.GetInstance().LogError(mode, "CopyPasteManager::ValidatePasteExpression", "InvalidExpressionForPresentMode");
                return PasteErrorString;
            }

            return pastedText;
        }

        public static IList<string> ExtractOperands(string pasteExpression, ViewMode mode)
        {
            var operands = new List<string>();
            int lastIndex = 0;
            bool haveOperator = false;
            bool startExpCounting = false;
            bool startOfExpression = true;
            bool isPreviousOpenParen = false;
            bool isPreviousOperator = false;

            string validCharacterSet;
            switch (mode)
            {
                case ViewMode.Standard:
                    validCharacterSet = c_validStandardCharacterSet;
                    break;
                case ViewMode.Scientific:
                    validCharacterSet = c_validScientificCharacterSet;
                    break;
                case ViewMode.Programmer:
                    validCharacterSet = c_validProgrammerCharacterSet;
                    break;
                default:
                    validCharacterSet = c_validBasicCharacterSet;
                    break;
            }

            int expLength = 0;

            for (int i = 0; i < pasteExpression.Length; i++)
            {
                char currentChar = pasteExpression[i];

                // If the current character is not a valid one, don't process it
                if (validCharacterSet.IndexOf(currentChar) < 0)
                {
                    continue;
                }

                if (operands.Count >= MaxOperandCountValue)
                {
                    TraceLogger.GetInstance().LogError(mode, "CopyPasteManager::ExtractOperands", "OperandCountGreaterThanMaxCount");
                    operands.Clear();
                    return operands;
                }

                if (currentChar >= '0' && currentChar <= '9')
                {
                    if (startExpCounting)
                    {
                        expLength++;
                        if (expLength > MaxExponentLengthValue)
                        {
                            TraceLogger.GetInstance().LogError(mode, "CopyPasteManager::ExtractOperands", "ExponentLengthGreaterThanMaxLength");
                            operands.Clear();
                            return operands;
                        }
                    }
                    isPreviousOperator = false;
                }
                else if (currentChar == 'e')
                {
                    if (mode != ViewMode.Programmer)
                    {
                        startExpCounting = true;
                    }
                    isPreviousOperator = false;
                }
                else if (currentChar == '+' || currentChar == '-' || currentChar == '*' || currentChar == '/' || currentChar == '^' || currentChar == '%')
                {
                    if (currentChar == '+' || currentChar == '-')
                    {
                        if (isPreviousOpenParen || startOfExpression || isPreviousOperator
                            || ((mode != ViewMode.Programmer) && !((i != 0) && pasteExpression[i - 1] != 'e')))
                        {
                            isPreviousOperator = false;
                            continue;
                        }
                    }

                    startExpCounting = false;
                    expLength = 0;
                    haveOperator = true;
                    isPreviousOperator = true;
                    operands.Add(pasteExpression.Substring(lastIndex, i - lastIndex));
                    lastIndex = i + 1;
                }
                else
                {
                    isPreviousOperator = false;
                }

                isPreviousOpenParen = (currentChar == '(');
                startOfExpression = false;
            }

            if (!haveOperator)
            {
                operands.Clear();
                operands.Add(pasteExpression);
            }
            else
            {
                operands.Add(pasteExpression.Substring(lastIndex, pasteExpression.Length - lastIndex));
            }

            return operands;
        }

        public static bool ExpressionRegExMatch(
            IList<string> operands,
            ViewMode mode,
            CategoryGroupType modeType,
            NumberBase programmerNumberBase,
            BitLength bitLengthType)
        {
            if (operands.Count == 0)
            {
                return false;
            }

            Regex[] patterns;

            if (mode == ViewMode.Standard)
            {
                patterns = s_standardModePatterns;
            }
            else if (mode == ViewMode.Scientific)
            {
                patterns = s_scientificModePatterns;
            }
            else if (mode == ViewMode.Programmer)
            {
                int index = (int)programmerNumberBase - (int)NumberBase.HexBase;
                patterns = s_programmerModePatterns[index];
            }
            else if (modeType == CategoryGroupType.Converter)
            {
                patterns = s_unitConverterPatterns;
            }
            else
            {
                patterns = Array.Empty<Regex>();
            }

            var maxOperandLengthAndValue = GetMaxOperandLengthAndValue(mode, modeType, programmerNumberBase, bitLengthType);
            bool expMatched = true;

            foreach (var operand in operands)
            {
                bool operandMatched = patterns.Any(p => p.IsMatch(operand));

                if (operandMatched)
                {
                    bool isNegativeValue = operand.Length > 0 && operand[0] == '-';
                    var operandValue = SanitizeOperand(operand);

                    if (OperandLength(operandValue, mode, modeType, programmerNumberBase) > maxOperandLengthAndValue.MaxLength)
                    {
                        expMatched = false;
                        break;
                    }

                    if (maxOperandLengthAndValue.MaxValue != 0)
                    {
                        ulong? operandAsULL = TryOperandToULL(operandValue, programmerNumberBase);
                        if (operandAsULL == null)
                        {
                            expMatched = false;
                            break;
                        }

                        bool isOverflow = operandAsULL.Value > maxOperandLengthAndValue.MaxValue;
                        bool isMaxNegativeValue = operandAsULL.Value - 1 == maxOperandLengthAndValue.MaxValue;
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

        public static CopyPasteMaxOperandLengthAndValue GetMaxOperandLengthAndValue(
            ViewMode mode,
            CategoryGroupType modeType,
            NumberBase programmerNumberBase,
            BitLength bitLengthType)
        {
            CopyPasteMaxOperandLengthAndValue res;

            if (mode == ViewMode.Standard)
            {
                res.MaxLength = MaxStandardOperandLengthValue;
                res.MaxValue = 0;
                return res;
            }
            else if (mode == ViewMode.Scientific)
            {
                res.MaxLength = MaxScientificOperandLengthValue;
                res.MaxValue = 0;
                return res;
            }
            else if (mode == ViewMode.Programmer)
            {
                uint bitLength;
                switch (bitLengthType)
                {
                    case BitLength.BitLengthQWord: bitLength = 64; break;
                    case BitLength.BitLengthDWord: bitLength = 32; break;
                    case BitLength.BitLengthWord: bitLength = 16; break;
                    case BitLength.BitLengthByte: bitLength = 8; break;
                    default: bitLength = 0; break;
                }

                double bitsPerDigit;
                switch (programmerNumberBase)
                {
                    case NumberBase.BinBase: bitsPerDigit = Math.Log(2) / Math.Log(2); break;
                    case NumberBase.OctBase: bitsPerDigit = Math.Log(8) / Math.Log(2); break;
                    case NumberBase.DecBase: bitsPerDigit = Math.Log(10) / Math.Log(2); break;
                    case NumberBase.HexBase: bitsPerDigit = Math.Log(16) / Math.Log(2); break;
                    default: bitsPerDigit = 0; break;
                }

                uint signBit = (programmerNumberBase == NumberBase.DecBase) ? 1u : 0u;

                uint maxLength = (uint)Math.Ceiling((bitLength - signBit) / bitsPerDigit);
                ulong maxValue = ulong.MaxValue >> (MaxProgrammerBitLengthValue - (int)(bitLength - signBit));

                res.MaxLength = maxLength;
                res.MaxValue = maxValue;
                return res;
            }
            else if (modeType == CategoryGroupType.Converter)
            {
                res.MaxLength = MaxConverterInputLengthValue;
                res.MaxValue = 0;
                return res;
            }

            res.MaxLength = 0;
            res.MaxValue = 0;
            return res;
        }

        public static string SanitizeOperand(string operand)
        {
            char[] unwantedChars = { '\'', '_', '`', '(', ')', '-', '+' };
            return RemoveCharsFromString(operand, unwantedChars);
        }

        public static ulong? TryOperandToULL(string operand, NumberBase numberBase)
        {
            if (operand.Length == 0 || operand[0] == '-')
            {
                return null;
            }

            int intBase;
            switch (numberBase)
            {
                case NumberBase.HexBase: intBase = 16; break;
                case NumberBase.OctBase: intBase = 8; break;
                case NumberBase.BinBase: intBase = 2; break;
                default: intBase = 10; break;
            }

            try
            {
                return Convert.ToUInt64(operand, intBase);
            }
            catch (FormatException)
            {
                return null;
            }
            catch (OverflowException)
            {
                return null;
            }
        }

        public static uint OperandLength(string operand, ViewMode mode, CategoryGroupType modeType, NumberBase programmerNumberBase)
        {
            if (modeType == CategoryGroupType.Converter)
            {
                return (uint)operand.Length;
            }

            switch (mode)
            {
                case ViewMode.Standard:
                case ViewMode.Scientific:
                    return StandardScientificOperandLength(operand);
                case ViewMode.Programmer:
                    return ProgrammerOperandLength(operand, programmerNumberBase);
                default:
                    return 0;
            }
        }

        public static uint StandardScientificOperandLength(string operand)
        {
            bool hasDecimal = operand.Contains('.');
            int length = operand.Length;

            if (hasDecimal && length >= 2)
            {
                if (operand[0] == '0' && operand[1] == '.')
                {
                    length -= 2;
                }
                else
                {
                    length -= 1;
                }
            }

            int exponentPos = operand.IndexOf('e');
            if (exponentPos >= 0)
            {
                int expLength = operand.Length - exponentPos;
                length -= expLength;
            }

            return (uint)length;
        }

        public static uint ProgrammerOperandLength(string operand, NumberBase numberBase)
        {
            var prefixes = new List<string>();
            var suffixes = new List<string>();

            switch (numberBase)
            {
                case NumberBase.BinBase:
                    prefixes.AddRange(new[] { "0B", "0Y" });
                    suffixes.Add("B");
                    break;
                case NumberBase.DecBase:
                    prefixes.AddRange(new[] { "-", "0N" });
                    break;
                case NumberBase.OctBase:
                    prefixes.AddRange(new[] { "0T", "0O" });
                    break;
                case NumberBase.HexBase:
                    prefixes.Add("0X");
                    suffixes.Add("H");
                    break;
                default:
                    return 0;
            }

            // UInt suffixes are common across all modes
            suffixes.AddRange(new[] { "ULL", "UL", "LL", "U", "L" });

            string operandUpper = operand.ToUpperInvariant();
            int len = operand.Length;

            // Detect suffix and subtract its length
            foreach (var suffix in suffixes)
            {
                if (len < suffix.Length)
                    continue;

                if (operandUpper.EndsWith(suffix, StringComparison.Ordinal))
                {
                    len -= suffix.Length;
                    break;
                }
            }

            // Detect prefix and subtract its length
            foreach (var prefix in prefixes)
            {
                if (len < prefix.Length)
                    continue;

                if (operandUpper.StartsWith(prefix, StringComparison.Ordinal))
                {
                    len -= prefix.Length;
                    break;
                }
            }

            return (uint)len;
        }

        public static string RemoveUnwantedCharsFromString(string input)
        {
            char[] unwantedChars = { ' ', ',', '"', (char)165, (char)164, (char)8373, '$', (char)8353, (char)8361, (char)8362, (char)8358, (char)8377, (char)163, (char)8364, (char)8234, (char)8235, (char)8236, (char)8237, (char)160 };
            input = LocalizationSettings.GetInstance().RemoveGroupSeparators(input);
            return RemoveCharsFromString(input, unwantedChars);
        }

        private static string RemoveCharsFromString(string input, char[] charsToRemove)
        {
            var sb = new StringBuilder(input.Length);
            foreach (char ch in input)
            {
                if (Array.IndexOf(charsToRemove, ch) < 0)
                {
                    sb.Append(ch);
                }
            }
            return sb.ToString();
        }
    }
}
