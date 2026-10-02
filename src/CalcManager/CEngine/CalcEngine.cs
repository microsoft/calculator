// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;
using System.Text.RegularExpressions;
using CalcManager.CalculationManager;
using CalcManager.Ratpack;

namespace CalcManager.CEngine
{
    public enum NUM_WIDTH
    {
        QWORD_WIDTH, // 64 bits mode (default)
        DWORD_WIDTH, // 32 bits mode
        WORD_WIDTH,  // 16 bits mode
        BYTE_WIDTH   // 8 bits mode
    }

    public class CCalcEngine
    {
        public const int DEFAULT_MAX_DIGITS = 32;
        public const int DEFAULT_PRECISION = 32;
        public const int DEFAULT_RADIX = 10;
        public const char DEFAULT_DEC_SEPARATOR = '.';
        public const char DEFAULT_GRP_SEPARATOR = ',';
        public const string DEFAULT_GRP_STR = "3;0";
        public const string DEFAULT_NUMBER_STR = "0";
        public const int NUM_WIDTH_LENGTH = 4;
        public const int MAX_EXPONENT = 4;
        public const uint MAX_GROUPING_SIZE = 16;

        private struct FunctionNameElement
        {
            public string DegreeString;
            public string InverseDegreeString;
            public string RadString;
            public string InverseRadString;
            public string GradString;
            public string InverseGradString;
            public string ProgrammerModeString;

            public bool HasAngleStrings => !string.IsNullOrEmpty(RadString) ||
                                           !string.IsNullOrEmpty(InverseRadString) ||
                                           !string.IsNullOrEmpty(GradString) ||
                                           !string.IsNullOrEmpty(InverseGradString);
        }

        private static readonly Dictionary<int, FunctionNameElement> s_operatorStringTable = new Dictionary<int, FunctionNameElement>
        {
            { CCommand.IDC_CHOP, new FunctionNameElement { DegreeString = "", InverseDegreeString = EngineStrings.SIDS_FRAC } },
            { CCommand.IDC_SIN, new FunctionNameElement { DegreeString = EngineStrings.SIDS_SIND, InverseDegreeString = EngineStrings.SIDS_ASIND, RadString = EngineStrings.SIDS_SINR, InverseRadString = EngineStrings.SIDS_ASINR, GradString = EngineStrings.SIDS_SING, InverseGradString = EngineStrings.SIDS_ASING } },
            { CCommand.IDC_COS, new FunctionNameElement { DegreeString = EngineStrings.SIDS_COSD, InverseDegreeString = EngineStrings.SIDS_ACOSD, RadString = EngineStrings.SIDS_COSR, InverseRadString = EngineStrings.SIDS_ACOSR, GradString = EngineStrings.SIDS_COSG, InverseGradString = EngineStrings.SIDS_ACOSG } },
            { CCommand.IDC_TAN, new FunctionNameElement { DegreeString = EngineStrings.SIDS_TAND, InverseDegreeString = EngineStrings.SIDS_ATAND, RadString = EngineStrings.SIDS_TANR, InverseRadString = EngineStrings.SIDS_ATANR, GradString = EngineStrings.SIDS_TANG, InverseGradString = EngineStrings.SIDS_ATANG } },
            { CCommand.IDC_SINH, new FunctionNameElement { DegreeString = "", InverseDegreeString = EngineStrings.SIDS_ASINH } },
            { CCommand.IDC_COSH, new FunctionNameElement { DegreeString = "", InverseDegreeString = EngineStrings.SIDS_ACOSH } },
            { CCommand.IDC_TANH, new FunctionNameElement { DegreeString = "", InverseDegreeString = EngineStrings.SIDS_ATANH } },
            { CCommand.IDC_SEC, new FunctionNameElement { DegreeString = EngineStrings.SIDS_SECD, InverseDegreeString = EngineStrings.SIDS_ASECD, RadString = EngineStrings.SIDS_SECR, InverseRadString = EngineStrings.SIDS_ASECR, GradString = EngineStrings.SIDS_SECG, InverseGradString = EngineStrings.SIDS_ASECG } },
            { CCommand.IDC_CSC, new FunctionNameElement { DegreeString = EngineStrings.SIDS_CSCD, InverseDegreeString = EngineStrings.SIDS_ACSCD, RadString = EngineStrings.SIDS_CSCR, InverseRadString = EngineStrings.SIDS_ACSCR, GradString = EngineStrings.SIDS_CSCG, InverseGradString = EngineStrings.SIDS_ACSCG } },
            { CCommand.IDC_COT, new FunctionNameElement { DegreeString = EngineStrings.SIDS_COTD, InverseDegreeString = EngineStrings.SIDS_ACOTD, RadString = EngineStrings.SIDS_COTR, InverseRadString = EngineStrings.SIDS_ACOTR, GradString = EngineStrings.SIDS_COTG, InverseGradString = EngineStrings.SIDS_ACOTG } },
            { CCommand.IDC_SECH, new FunctionNameElement { DegreeString = EngineStrings.SIDS_SECH, InverseDegreeString = EngineStrings.SIDS_ASECH } },
            { CCommand.IDC_CSCH, new FunctionNameElement { DegreeString = EngineStrings.SIDS_CSCH, InverseDegreeString = EngineStrings.SIDS_ACSCH } },
            { CCommand.IDC_COTH, new FunctionNameElement { DegreeString = EngineStrings.SIDS_COTH, InverseDegreeString = EngineStrings.SIDS_ACOTH } },
            { CCommand.IDC_LN, new FunctionNameElement { DegreeString = "", InverseDegreeString = EngineStrings.SIDS_POWE } },
            { CCommand.IDC_SQR, new FunctionNameElement { DegreeString = EngineStrings.SIDS_SQR } },
            { CCommand.IDC_CUB, new FunctionNameElement { DegreeString = EngineStrings.SIDS_CUBE } },
            { CCommand.IDC_FAC, new FunctionNameElement { DegreeString = EngineStrings.SIDS_FACT } },
            { CCommand.IDC_REC, new FunctionNameElement { DegreeString = EngineStrings.SIDS_RECIPROC } },
            { CCommand.IDC_DMS, new FunctionNameElement { DegreeString = "", InverseDegreeString = EngineStrings.SIDS_DEGREES } },
            { CCommand.IDC_SIGN, new FunctionNameElement { DegreeString = EngineStrings.SIDS_NEGATE } },
            { CCommand.IDC_DEGREES, new FunctionNameElement { DegreeString = EngineStrings.SIDS_DEGREES } },
            { CCommand.IDC_POW2, new FunctionNameElement { DegreeString = EngineStrings.SIDS_TWOPOWX } },
            { CCommand.IDC_LOGBASEY, new FunctionNameElement { DegreeString = EngineStrings.SIDS_LOGBASEY } },
            { CCommand.IDC_ABS, new FunctionNameElement { DegreeString = EngineStrings.SIDS_ABS } },
            { CCommand.IDC_CEIL, new FunctionNameElement { DegreeString = EngineStrings.SIDS_CEIL } },
            { CCommand.IDC_FLOOR, new FunctionNameElement { DegreeString = EngineStrings.SIDS_FLOOR } },
            { CCommand.IDC_NAND, new FunctionNameElement { DegreeString = EngineStrings.SIDS_NAND } },
            { CCommand.IDC_NOR, new FunctionNameElement { DegreeString = EngineStrings.SIDS_NOR } },
            { CCommand.IDC_RSHFL, new FunctionNameElement { DegreeString = EngineStrings.SIDS_RSH } },
            { CCommand.IDC_RORC, new FunctionNameElement { DegreeString = EngineStrings.SIDS_ROR } },
            { CCommand.IDC_ROLC, new FunctionNameElement { DegreeString = EngineStrings.SIDS_ROL } },
            { CCommand.IDC_CUBEROOT, new FunctionNameElement { DegreeString = EngineStrings.SIDS_CUBEROOT } },
            { CCommand.IDC_MOD, new FunctionNameElement { DegreeString = EngineStrings.SIDS_MOD, ProgrammerModeString = EngineStrings.SIDS_PROGRAMMER_MOD } }
        };

        private static readonly Dictionary<string, string> s_engineStrings = new Dictionary<string, string>();

        private readonly bool m_fPrecedence;
        private readonly bool m_fIntegerMode;
        private readonly ICalcDisplay m_pCalcDisplay;
        private readonly IResourceProvider m_resourceProvider;

        private int m_nOpCode;
        private int m_nPrevOpCode;
        private bool m_bChangeOp;
        private bool m_bRecord;
        private bool m_bSetCalcState;
        private CalcInput m_input;
        private NumberFormat m_nFE;
        private Rational m_maxTrigonometricNum;
        private Rational m_memoryValue;
        private Rational m_holdVal;
        private Rational m_currentVal;
        private Rational m_lastVal;
        private readonly Rational[] m_parenVals = new Rational[HistoryCollector.MAXPRECDEPTH];
        private readonly Rational[] m_precedenceVals = new Rational[HistoryCollector.MAXPRECDEPTH];
        private bool m_bError;
        private bool m_bInv;
        private bool m_bNoPrevEqu;

        private uint m_radix;
        private int m_precision;
        private int m_cIntDigitsSav;
        private List<uint> m_decGrouping = new List<uint>();

        private string m_numberString;
        private int m_nTempCom;
        private int m_openParenCount;
        private readonly int[] m_nOp = new int[HistoryCollector.MAXPRECDEPTH];
        private readonly int[] m_nPrecOp = new int[HistoryCollector.MAXPRECDEPTH];
        private int m_precedenceOpCount;
        private int m_nLastCom;
        private AngleType m_angletype;
        private NUM_WIDTH m_numwidth;
        private int m_dwWordBitWidth;

        private Random m_randomGenerator;
        private ulong m_carryBit;

        private readonly HistoryCollector m_HistoryCollector;
        private readonly Rational[] m_chopNumbers = new Rational[NUM_WIDTH_LENGTH];
        private readonly string[] m_maxDecimalValueStrings = new string[NUM_WIDTH_LENGTH];
        private char m_decimalSeparator;
        private char m_groupSeparator;

        public static void LoadEngineStrings(IResourceProvider resourceProvider)
        {
            if (resourceProvider == null) return;
            foreach (var sid in EngineStrings.g_sids)
            {
                var locString = resourceProvider.GetCEngineString(sid);
                if (!string.IsNullOrEmpty(locString))
                {
                    s_engineStrings[sid] = locString;
                }
            }
        }

        public static void InitialOneTimeOnlySetup(IResourceProvider resourceProvider)
        {
            LoadEngineStrings(resourceProvider);
            ChangeBaseConstants(DEFAULT_RADIX, DEFAULT_MAX_DIGITS, DEFAULT_PRECISION);
        }

        public CCalcEngine(
            bool fPrecedence,
            bool fIntegerMode,
            IResourceProvider pResourceProvider,
            ICalcDisplay pCalcDisplay,
            IHistoryDisplay pHistoryDisplay)
        {
            m_fPrecedence = fPrecedence;
            m_fIntegerMode = fIntegerMode;
            m_pCalcDisplay = pCalcDisplay;
            m_resourceProvider = pResourceProvider;
            m_nOpCode = 0;
            m_nPrevOpCode = 0;
            m_bChangeOp = false;
            m_bRecord = false;
            m_bSetCalcState = false;
            m_input = new CalcInput(DEFAULT_DEC_SEPARATOR);
            m_nFE = NumberFormat.Float;
            m_memoryValue = 0;
            m_holdVal = 0;
            m_currentVal = 0;
            m_lastVal = 0;
            m_bError = false;
            m_bInv = false;
            m_bNoPrevEqu = true;
            m_radix = DEFAULT_RADIX;
            m_precision = DEFAULT_PRECISION;
            m_cIntDigitsSav = DEFAULT_MAX_DIGITS;
            m_numberString = DEFAULT_NUMBER_STR;
            m_nTempCom = 0;
            m_openParenCount = 0;
            m_precedenceOpCount = 0;
            m_nLastCom = 0;
            m_angletype = AngleType.Degrees;
            m_numwidth = NUM_WIDTH.QWORD_WIDTH;
            m_HistoryCollector = new HistoryCollector(pCalcDisplay, pHistoryDisplay, DEFAULT_DEC_SEPARATOR);
            m_groupSeparator = DEFAULT_GRP_SEPARATOR;
            m_decimalSeparator = DEFAULT_DEC_SEPARATOR;

            InitChopNumbers();
            m_dwWordBitWidth = DwWordBitWidthFromNumWidth(m_numwidth);
            m_maxTrigonometricNum = RationalMath.Pow(10, 100);

            SetRadixTypeAndNumWidth(RadixType.Decimal, m_numwidth);
            SettingsChanged();
            DisplayNum();
        }

        private void InitChopNumbers()
        {
            m_chopNumbers[0] = new Rational(Ratpak.rat_qword);
            m_chopNumbers[1] = new Rational(Ratpak.rat_dword);
            m_chopNumbers[2] = new Rational(Ratpak.rat_word);
            m_chopNumbers[3] = new Rational(Ratpak.rat_byte);

            for (int i = 0; i < m_chopNumbers.Length; i++)
            {
                var maxVal = m_chopNumbers[i] / 2;
                maxVal = RationalMath.Integer(maxVal);
                m_maxDecimalValueStrings[i] = maxVal.ToString(10, NumberFormat.Float, m_precision);
            }
        }

        public Rational GetChopNumber() => m_chopNumbers[(int)m_numwidth];
        public string GetMaxDecimalValueString() => m_maxDecimalValueStrings[(int)m_numwidth];

        public Rational PersistedMemObject() => m_memoryValue;
        public void PersistedMemObject(Rational memObject)
        {
            m_memoryValue = memObject;
        }

        public bool FInErrorState() => m_bError;
        public bool IsInputEmpty() => m_input.IsEmpty() && (string.IsNullOrEmpty(m_numberString) || m_numberString == "0");
        public bool FInRecordingState() => m_bRecord;

        public void SettingsChanged()
        {
            char lastDec = m_decimalSeparator;
            string decStr = m_resourceProvider?.GetCEngineString("sDecimal");
            m_decimalSeparator = string.IsNullOrEmpty(decStr) ? DEFAULT_DEC_SEPARATOR : decStr[0];
            Ratpak.SetDecimalSeparator(m_decimalSeparator);

            char lastSep = m_groupSeparator;
            string sepStr = m_resourceProvider?.GetCEngineString("sThousand");
            m_groupSeparator = string.IsNullOrEmpty(sepStr) ? DEFAULT_GRP_SEPARATOR : sepStr[0];

            var lastDecGrouping = m_decGrouping;
            string grpStr = m_resourceProvider?.GetCEngineString("sGrouping");
            m_decGrouping = DigitGroupingStringToGroupingVector(string.IsNullOrEmpty(grpStr) ? DEFAULT_GRP_STR : grpStr);

            bool numChanged = false;
            if (!GroupingEquals(m_decGrouping, lastDecGrouping) || m_groupSeparator != lastSep)
            {
                numChanged = true;
            }

            if (m_decimalSeparator != lastDec)
            {
                m_input.SetDecimalSymbol(m_decimalSeparator);
                m_HistoryCollector.SetDecimalSymbol(m_decimalSeparator);
                s_engineStrings[EngineStrings.SIDS_DECIMAL_SEPARATOR] = m_decimalSeparator.ToString();
                numChanged = true;
            }

            if (numChanged)
            {
                DisplayNum();
            }
        }

        private static bool GroupingEquals(List<uint> a, List<uint> b)
        {
            if (a == null && b == null) return true;
            if (a == null || b == null) return false;
            if (a.Count != b.Count) return false;
            for (int i = 0; i < a.Count; i++)
            {
                if (a[i] != b[i]) return false;
            }
            return true;
        }

        public char DecimalSeparator() => m_decimalSeparator;

        public List<IExpressionCommand> GetHistoryCollectorCommandsSnapshot()
        {
            var commands = m_HistoryCollector.GetCommands();
            if (!m_HistoryCollector.FOpndAddedToHistory() && m_bRecord)
            {
                commands.Add(m_HistoryCollector.GetOperandCommandsFromString(m_numberString, m_currentVal));
            }
            return commands;
        }

        public static string GetString(int ids)
        {
            string key = ids.ToString();
            return s_engineStrings.TryGetValue(key, out var val) ? val : string.Empty;
        }

        public static string GetString(string ids)
        {
            if (string.IsNullOrEmpty(ids)) return string.Empty;
            return s_engineStrings.TryGetValue(ids, out var val) ? val : string.Empty;
        }

        public static string OpCodeToString(int nOpCode)
        {
            return GetString(IdStrFromCmdId(nOpCode));
        }

        private static int IdStrFromCmdId(int id)
        {
            return id - CCommand.IDC_FIRSTCONTROL + CCommand.IDS_ENGINESTR_FIRST;
        }

        public static string OpCodeToUnaryString(int nOpCode, bool fInv, AngleType angletype)
        {
            string ids = string.Empty;
            if (s_operatorStringTable.TryGetValue(nOpCode, out var element))
            {
                if (!element.HasAngleStrings || AngleType.Degrees == angletype)
                {
                    if (fInv)
                    {
                        ids = element.InverseDegreeString;
                    }
                    if (string.IsNullOrEmpty(ids))
                    {
                        ids = element.DegreeString;
                    }
                }
                else if (AngleType.Radians == angletype)
                {
                    if (fInv)
                    {
                        ids = element.InverseRadString;
                    }
                    if (string.IsNullOrEmpty(ids))
                    {
                        ids = element.RadString;
                    }
                }
                else if (AngleType.Gradians == angletype)
                {
                    if (fInv)
                    {
                        ids = element.InverseGradString;
                    }
                    if (string.IsNullOrEmpty(ids))
                    {
                        ids = element.GradString;
                    }
                }
            }

            if (!string.IsNullOrEmpty(ids))
            {
                return GetString(ids);
            }

            return OpCodeToString(nOpCode);
        }

        public static string OpCodeToBinaryString(int nOpCode, bool isIntegerMode)
        {
            string ids = string.Empty;
            if (s_operatorStringTable.TryGetValue(nOpCode, out var element))
            {
                if (isIntegerMode && !string.IsNullOrEmpty(element.ProgrammerModeString))
                {
                    ids = element.ProgrammerModeString;
                }
                else
                {
                    ids = element.DegreeString;
                }
            }

            if (!string.IsNullOrEmpty(ids))
            {
                return GetString(ids);
            }

            return OpCodeToString(nOpCode);
        }

        private static int NPrecedenceOfOp(int nopCode)
        {
            switch (nopCode)
            {
                default:
                case CCommand.IDC_OR:
                case CCommand.IDC_XOR:
                    return 0;
                case CCommand.IDC_AND:
                case CCommand.IDC_NAND:
                case CCommand.IDC_NOR:
                    return 1;
                case CCommand.IDC_ADD:
                case CCommand.IDC_SUB:
                    return 2;
                case CCommand.IDC_LSHF:
                case CCommand.IDC_RSHF:
                case CCommand.IDC_RSHFL:
                case CCommand.IDC_MOD:
                case CCommand.IDC_DIV:
                case CCommand.IDC_MUL:
                    return 3;
                case CCommand.IDC_PWR:
                case CCommand.IDC_ROOT:
                case CCommand.IDC_LOGBASEY:
                    return 4;
            }
        }

        private void HandleErrorCommand(uint idc)
        {
            if (!CalcUtils.IsGuiSettingOpCode(idc))
            {
                m_nTempCom = m_nLastCom;
            }
        }

        private void HandleMaxDigitsReached()
        {
            m_pCalcDisplay?.MaxDigitsReached();
        }

        public void ClearTemporaryValues()
        {
            m_bInv = false;
            m_input.Clear();
            m_bRecord = true;
            CheckAndAddLastBinOpToHistory();
            DisplayNum();
            m_bError = false;
        }

        public void ClearDisplay()
        {
            m_pCalcDisplay?.SetExpressionDisplay(new List<Tuple<string, int>>(), new List<IExpressionCommand>());
        }

        public void ProcessCommand(uint wParam)
        {
            if (wParam == CCommand.IDC_SET_RESULT)
            {
                wParam = CCommand.IDC_RECALL;
                m_bSetCalcState = true;
            }

            ProcessCommandWorker(wParam);
        }

        private void ProcessCommandWorker(uint wParam)
        {
            if (!CalcUtils.IsGuiSettingOpCode(wParam))
            {
                m_nLastCom = m_nTempCom;
                m_nTempCom = (int)wParam;
            }

            if (!m_bNoPrevEqu)
            {
                ClearDisplay();
            }

            if (m_bError)
            {
                if (wParam == CCommand.IDC_CLEAR)
                {
                    // handle C
                }
                else if (wParam == CCommand.IDC_CENTR)
                {
                    wParam = CCommand.IDC_CLEAR;
                }
                else
                {
                    HandleErrorCommand(wParam);
                    return;
                }
            }

            if (m_bRecord)
            {
                if (CalcUtils.IsBinOpCode(wParam) || CalcUtils.IsUnaryOpCode(wParam) || CalcUtils.IsOpInRange(wParam, CCommand.IDC_FE, CCommand.IDC_MMINUS) ||
                    CalcUtils.IsOpInRange(wParam, CCommand.IDC_OPENP, CCommand.IDC_CLOSEP) || CalcUtils.IsOpInRange(wParam, CCommand.IDM_HEX, CCommand.IDM_BIN) ||
                    CalcUtils.IsOpInRange(wParam, CCommand.IDM_QWORD, CCommand.IDM_BYTE) || CalcUtils.IsOpInRange(wParam, CCommand.IDM_DEG, CCommand.IDM_GRAD) ||
                    CalcUtils.IsOpInRange(wParam, CCommand.IDC_BINEDITSTART, CCommand.IDC_BINEDITEND) || (CCommand.IDC_INV == wParam) ||
                    (CCommand.IDC_SIGN == wParam && 10 != m_radix) || (CCommand.IDC_RAND == wParam) || (CCommand.IDC_EULER == wParam))
                {
                    m_bRecord = false;
                    m_currentVal = m_input.ToRational(m_radix, m_precision);
                    DisplayNum();
                }
            }
            else if (CalcUtils.IsDigitOpCode(wParam) || wParam == CCommand.IDC_PNT)
            {
                m_bRecord = true;
                m_input.Clear();

                if (m_nLastCom != CCommand.IDC_CLOSEP)
                {
                    CheckAndAddLastBinOpToHistory();
                }
            }

            if (CalcUtils.IsDigitOpCode(wParam))
            {
                uint iValue = wParam - CCommand.IDC_0;
                if (iValue >= m_radix)
                {
                    HandleErrorCommand(wParam);
                    return;
                }

                if (!m_input.TryAddDigit(iValue, m_radix, m_fIntegerMode, GetMaxDecimalValueString(), m_dwWordBitWidth, m_cIntDigitsSav))
                {
                    HandleErrorCommand(wParam);
                    HandleMaxDigitsReached();
                    return;
                }

                if (m_nLastCom == CCommand.IDC_CLOSEP)
                {
                    m_nOpCode = CCommand.IDC_MUL;
                    m_lastVal = m_currentVal;
                    m_holdVal = 0;
                    m_bNoPrevEqu = true;

                    if (!m_HistoryCollector.FOpndAddedToHistory())
                    {
                        m_HistoryCollector.AddOpenBraceToHistory();
                        m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                        m_HistoryCollector.AddCloseBraceToHistory();
                    }

                    m_HistoryCollector.AddBinOpToHistory(m_nOpCode, m_fIntegerMode);
                    m_bChangeOp = true;
                    m_nPrevOpCode = 0;

                    while (m_precedenceOpCount > 0)
                    {
                        m_precedenceOpCount--;
                        m_nPrecOp[m_precedenceOpCount] = 0;
                    }
                }
                DisplayNum();
                return;
            }

            if (CalcUtils.IsBinOpCode(wParam))
            {
                if (CalcUtils.IsBinOpCode((uint)m_nLastCom))
                {
                    bool fPrecInvToHigher = false;
                    m_nOpCode = (int)wParam;

                    if (m_fPrecedence && 0 != m_nPrevOpCode)
                    {
                        int nPrev = NPrecedenceOfOp(m_nPrevOpCode);
                        int nx = NPrecedenceOfOp(m_nLastCom);
                        int ni = NPrecedenceOfOp(m_nOpCode);
                        if (nx <= nPrev && ni > nPrev)
                        {
                            fPrecInvToHigher = true;
                            m_nPrevOpCode = 0;
                        }
                    }
                    m_HistoryCollector.ChangeLastBinOp(m_nOpCode, fPrecInvToHigher, m_fIntegerMode);
                    DisplayAnnounceBinaryOperator();
                    return;
                }

                if (!m_HistoryCollector.FOpndAddedToHistory())
                {
                    m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                }

                if (m_bChangeOp)
                {
                    while (true)
                    {
                        int nx = NPrecedenceOfOp((int)wParam);
                        int ni = NPrecedenceOfOp(m_nOpCode);

                        if ((nx > ni) && m_fPrecedence)
                        {
                            if (m_precedenceOpCount < HistoryCollector.MAXPRECDEPTH)
                            {
                                m_precedenceVals[m_precedenceOpCount] = m_lastVal;
                                m_nPrecOp[m_precedenceOpCount] = m_nOpCode;
                                m_HistoryCollector.PushLastOpndStart();
                            }
                            else
                            {
                                m_precedenceOpCount = HistoryCollector.MAXPRECDEPTH - 1;
                                HandleErrorCommand(wParam);
                            }
                            m_precedenceOpCount++;
                            break;
                        }
                        else
                        {
                            m_currentVal = DoOperation(m_nOpCode, m_currentVal, m_lastVal);
                            m_nPrevOpCode = m_nOpCode;

                            if (!m_bError)
                            {
                                DisplayNum();
                                if (!m_fPrecedence)
                                {
                                    string groupedString = GroupDigitsPerRadix(m_numberString, m_radix);
                                    m_HistoryCollector.CompleteEquation(groupedString);
                                    m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                                }
                            }

                            if ((m_precedenceOpCount != 0) && (m_nPrecOp[m_precedenceOpCount - 1] != 0))
                            {
                                m_precedenceOpCount--;
                                m_nOpCode = m_nPrecOp[m_precedenceOpCount];
                                m_lastVal = m_precedenceVals[m_precedenceOpCount];

                                nx = NPrecedenceOfOp(m_nOpCode);
                                if (ni <= nx)
                                {
                                    m_HistoryCollector.EnclosePrecInversionBrackets();
                                }
                                m_HistoryCollector.PopLastOpndStart();
                                continue;
                            }
                            break;
                        }
                    }
                }

                DisplayAnnounceBinaryOperator();
                m_lastVal = m_currentVal;
                m_nOpCode = (int)wParam;
                m_HistoryCollector.AddBinOpToHistory(m_nOpCode, m_fIntegerMode);
                m_bNoPrevEqu = m_bChangeOp = true;
                return;
            }

            if (CalcUtils.IsUnaryOpCode(wParam) || (wParam == CCommand.IDC_DEGREES))
            {
                if (CalcUtils.IsBinOpCode((uint)m_nLastCom))
                {
                    m_currentVal = m_lastVal;
                }

                if (wParam != CCommand.IDC_PERCENT)
                {
                    if (!m_HistoryCollector.FOpndAddedToHistory())
                    {
                        m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                    }
                    m_HistoryCollector.AddUnaryOpToHistory((int)wParam, m_bInv, m_angletype);
                }

                if ((wParam == CCommand.IDC_SIN) || (wParam == CCommand.IDC_COS) || (wParam == CCommand.IDC_TAN) || (wParam == CCommand.IDC_SINH) ||
                    (wParam == CCommand.IDC_COSH) || (wParam == CCommand.IDC_TANH) || (wParam == CCommand.IDC_SEC) || (wParam == CCommand.IDC_CSC) ||
                    (wParam == CCommand.IDC_COT) || (wParam == CCommand.IDC_SECH) || (wParam == CCommand.IDC_CSCH) || (wParam == CCommand.IDC_COTH))
                {
                    if (IsCurrentTooBigForTrig())
                    {
                        m_currentVal = 0;
                        DisplayError(CalcErr.CALC_E_DOMAIN);
                        return;
                    }
                }

                m_currentVal = SciCalcFunctions(m_currentVal, wParam);
                if (m_bError) return;

                DisplayNum();

                if (wParam == CCommand.IDC_PERCENT)
                {
                    CheckAndAddLastBinOpToHistory();
                    m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal, true);
                }

                if (m_bInv && ((wParam == CCommand.IDC_CHOP) || (wParam == CCommand.IDC_SIN) || (wParam == CCommand.IDC_COS) || (wParam == CCommand.IDC_TAN) ||
                    (wParam == CCommand.IDC_LN) || (wParam == CCommand.IDC_DMS) || (wParam == CCommand.IDC_DEGREES) || (wParam == CCommand.IDC_SINH) ||
                    (wParam == CCommand.IDC_COSH) || (wParam == CCommand.IDC_TANH) || (wParam == CCommand.IDC_SEC) || (wParam == CCommand.IDC_CSC) ||
                    (wParam == CCommand.IDC_COT) || (wParam == CCommand.IDC_SECH) || (wParam == CCommand.IDC_CSCH) || (wParam == CCommand.IDC_COTH)))
                {
                    m_bInv = false;
                }

                return;
            }

            if (CalcUtils.IsOpInRange(wParam, CCommand.IDC_BINEDITSTART, CCommand.IDC_BINEDITEND))
            {
                if (CalcUtils.IsBinOpCode((uint)m_nLastCom))
                {
                    m_currentVal = m_lastVal;
                }

                CheckAndAddLastBinOpToHistory();
                if (TryToggleBit(ref m_currentVal, wParam - CCommand.IDC_BINEDITSTART))
                {
                    DisplayNum();
                }
                return;
            }

            switch (wParam)
            {
                case CCommand.IDC_CLEAR:
                    if (!m_bChangeOp)
                    {
                        CheckAndAddLastBinOpToHistory(false);
                    }
                    m_lastVal = 0;
                    m_bChangeOp = false;
                    m_openParenCount = 0;
                    m_precedenceOpCount = m_nTempCom = m_nLastCom = m_nOpCode = 0;
                    m_nPrevOpCode = 0;
                    m_bNoPrevEqu = true;
                    m_carryBit = 0;

                    m_pCalcDisplay?.SetParenthesisNumber(0);
                    ClearDisplay();

                    m_HistoryCollector.ClearHistoryLine(string.Empty);
                    ClearTemporaryValues();
                    break;

                case CCommand.IDC_CENTR:
                    ClearTemporaryValues();
                    break;

                case CCommand.IDC_BACK:
                    if (m_bRecord)
                    {
                        m_input.Backspace();
                        DisplayNum();
                    }
                    else
                    {
                        HandleErrorCommand(wParam);
                    }
                    break;

                case CCommand.IDC_EQU:
                    while (m_openParenCount > 0)
                    {
                        if (m_bError) break;
                        m_nTempCom = m_nLastCom;
                        ProcessCommand(CCommand.IDC_CLOSEP);
                        m_nLastCom = m_nTempCom;
                        m_nTempCom = (int)wParam;
                    }

                    if (!m_bNoPrevEqu)
                    {
                        m_lastVal = m_currentVal;
                    }

                    if (CalcUtils.IsBinOpCode((uint)m_nLastCom))
                    {
                        m_currentVal = m_lastVal;
                    }

                    if (!m_HistoryCollector.FOpndAddedToHistory())
                    {
                        m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                    }

                    ResolveHighestPrecedenceOperation();
                    while (m_fPrecedence && m_precedenceOpCount > 0)
                    {
                        m_precedenceOpCount--;
                        m_nOpCode = m_nPrecOp[m_precedenceOpCount];
                        m_lastVal = m_precedenceVals[m_precedenceOpCount];

                        int ni = NPrecedenceOfOp(m_nPrevOpCode);
                        int nx = NPrecedenceOfOp(m_nOpCode);
                        if (ni <= nx)
                        {
                            m_HistoryCollector.EnclosePrecInversionBrackets();
                        }
                        m_HistoryCollector.PopLastOpndStart();

                        m_bNoPrevEqu = true;
                        ResolveHighestPrecedenceOperation();
                    }

                    if (!m_bError)
                    {
                        string groupedString = GroupDigitsPerRadix(m_numberString, m_radix);
                        m_HistoryCollector.CompleteEquation(groupedString);
                        m_lastVal = m_currentVal;
                        m_nPrevOpCode = 0;
                        m_precedenceOpCount = 0;
                    }

                    m_bChangeOp = false;
                    break;

                case CCommand.IDC_OPENP:
                case CCommand.IDC_CLOSEP:
                    if ((m_openParenCount >= HistoryCollector.MAXPRECDEPTH && (wParam == CCommand.IDC_OPENP)) ||
                        (m_openParenCount == 0 && (wParam != CCommand.IDC_OPENP)) ||
                        (m_precedenceOpCount >= HistoryCollector.MAXPRECDEPTH && m_nPrecOp[m_precedenceOpCount - 1] != 0))
                    {
                        if (m_openParenCount == 0 && (wParam != CCommand.IDC_OPENP))
                        {
                            m_pCalcDisplay?.OnNoRightParenAdded();
                        }
                        HandleErrorCommand(wParam);
                        break;
                    }

                    if (wParam == CCommand.IDC_OPENP)
                    {
                        if (CalcUtils.IsDigitOpCode((uint)m_nLastCom) || CalcUtils.IsUnaryOpCode((uint)m_nLastCom) ||
                            m_nLastCom == CCommand.IDC_PNT || m_nLastCom == CCommand.IDC_CLOSEP)
                        {
                            ProcessCommand(CCommand.IDC_MUL);
                        }

                        CheckAndAddLastBinOpToHistory();
                        m_HistoryCollector.AddOpenBraceToHistory();

                        m_parenVals[m_openParenCount] = m_lastVal;
                        m_nOp[m_openParenCount++] = (m_bChangeOp ? m_nOpCode : 0);

                        if (m_precedenceOpCount < m_nPrecOp.Length)
                        {
                            m_nPrecOp[m_precedenceOpCount++] = 0;
                        }

                        m_lastVal = 0;
                        if (CalcUtils.IsBinOpCode((uint)m_nLastCom))
                        {
                            m_currentVal = 0;
                        }
                        m_nTempCom = 0;
                        m_nOpCode = 0;
                        m_bChangeOp = false;
                    }
                    else
                    {
                        if (CalcUtils.IsBinOpCode((uint)m_nLastCom))
                        {
                            m_currentVal = m_lastVal;
                        }

                        if (!m_HistoryCollector.FOpndAddedToHistory())
                        {
                            m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                        }

                        m_currentVal = DoOperation(m_nOpCode, m_currentVal, m_lastVal);
                        m_nPrevOpCode = m_nOpCode;

                        for (m_nOpCode = m_nPrecOp[--m_precedenceOpCount]; m_nOpCode != 0; m_nOpCode = m_nPrecOp[--m_precedenceOpCount])
                        {
                            int ni = NPrecedenceOfOp(m_nPrevOpCode);
                            int nx = NPrecedenceOfOp(m_nOpCode);
                            if (ni <= nx)
                            {
                                m_HistoryCollector.EnclosePrecInversionBrackets();
                            }
                            m_HistoryCollector.PopLastOpndStart();

                            m_lastVal = m_precedenceVals[m_precedenceOpCount];
                            m_currentVal = DoOperation(m_nOpCode, m_currentVal, m_lastVal);
                            m_nPrevOpCode = m_nOpCode;
                        }

                        m_HistoryCollector.AddCloseBraceToHistory();

                        m_openParenCount -= 1;
                        m_lastVal = m_parenVals[m_openParenCount];
                        m_nOpCode = m_nOp[m_openParenCount];
                        m_bChangeOp = (m_nOpCode != 0);
                    }

                    m_pCalcDisplay?.SetParenthesisNumber((uint)m_openParenCount);
                    if (!m_bError)
                    {
                        DisplayNum();
                    }
                    break;

                case CCommand.IDM_HEX:
                case CCommand.IDM_DEC:
                case CCommand.IDM_OCT:
                case CCommand.IDM_BIN:
                    SetRadixTypeAndNumWidth((RadixType)(wParam - CCommand.IDM_HEX), (NUM_WIDTH)(-1));
                    m_HistoryCollector.UpdateHistoryExpression(m_radix, m_precision);
                    break;

                case CCommand.IDM_QWORD:
                case CCommand.IDM_DWORD:
                case CCommand.IDM_WORD:
                case CCommand.IDM_BYTE:
                    if (m_bRecord)
                    {
                        m_currentVal = m_input.ToRational(m_radix, m_precision);
                        m_bRecord = false;
                    }
                    SetRadixTypeAndNumWidth((RadixType)(-1), (NUM_WIDTH)(wParam - CCommand.IDM_QWORD));
                    break;

                case CCommand.IDM_DEG:
                case CCommand.IDM_RAD:
                case CCommand.IDM_GRAD:
                    m_angletype = (AngleType)(wParam - CCommand.IDM_DEG);
                    break;

                case CCommand.IDC_SIGN:
                    if (m_bRecord)
                    {
                        if (m_input.TryToggleSign(m_fIntegerMode, GetMaxDecimalValueString()))
                        {
                            DisplayNum();
                        }
                        else
                        {
                            HandleErrorCommand(wParam);
                        }
                        break;
                    }

                    if (CalcUtils.IsBinOpCode((uint)m_nLastCom))
                    {
                        m_currentVal = m_lastVal;
                    }

                    if (!m_HistoryCollector.FOpndAddedToHistory())
                    {
                        m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                    }

                    m_currentVal = -m_currentVal;
                    DisplayNum();
                    m_HistoryCollector.AddUnaryOpToHistory(CCommand.IDC_SIGN, m_bInv, m_angletype);
                    break;

                case CCommand.IDC_RECALL:
                    if (m_bSetCalcState)
                    {
                        m_bSetCalcState = false;
                    }
                    else
                    {
                        m_currentVal = m_memoryValue;
                    }
                    CheckAndAddLastBinOpToHistory();
                    DisplayNum();
                    break;

                case CCommand.IDC_MPLUS:
                    Rational plusResult = m_memoryValue + m_currentVal;
                    m_memoryValue = TruncateNumForIntMath(plusResult);
                    break;

                case CCommand.IDC_MMINUS:
                    Rational minusResult = m_memoryValue - m_currentVal;
                    m_memoryValue = TruncateNumForIntMath(minusResult);
                    break;

                case CCommand.IDC_STORE:
                case CCommand.IDC_MCLEAR:
                    m_memoryValue = (wParam == CCommand.IDC_STORE ? TruncateNumForIntMath(m_currentVal) : 0);
                    break;

                case CCommand.IDC_PI:
                    if (!m_fIntegerMode)
                    {
                        CheckAndAddLastBinOpToHistory();
                        m_currentVal = new Rational(m_bInv ? Ratpak.two_pi : Ratpak.pi);
                        DisplayNum();
                        m_bInv = false;
                        break;
                    }
                    HandleErrorCommand(wParam);
                    break;

                case CCommand.IDC_RAND:
                    if (!m_fIntegerMode)
                    {
                        CheckAndAddLastBinOpToHistory();
                        string randStr = GenerateRandomNumber().ToString($"F{m_precision}", CultureInfo.InvariantCulture);
                        Rat rat = Ratpak.StringToRat(false, randStr, false, "", m_radix, m_precision);
                        m_currentVal = rat != null ? new Rational(rat) : 0;
                        DisplayNum();
                        m_bInv = false;
                        break;
                    }
                    HandleErrorCommand(wParam);
                    break;

                case CCommand.IDC_EULER:
                    if (!m_fIntegerMode)
                    {
                        CheckAndAddLastBinOpToHistory();
                        m_currentVal = new Rational(Ratpak.rat_exp);
                        DisplayNum();
                        m_bInv = false;
                        break;
                    }
                    HandleErrorCommand(wParam);
                    break;

                case CCommand.IDC_FE:
                    m_nFE = (m_nFE == NumberFormat.Float) ? NumberFormat.Scientific : NumberFormat.Float;
                    DisplayNum();
                    break;

                case CCommand.IDC_EXP:
                    if (m_bRecord && !m_fIntegerMode && m_input.TryBeginExponent())
                    {
                        DisplayNum();
                        break;
                    }
                    HandleErrorCommand(wParam);
                    break;

                case CCommand.IDC_PNT:
                    if (m_nLastCom == CCommand.IDC_CLOSEP)
                    {
                        m_nOpCode = CCommand.IDC_MUL;
                        m_lastVal = m_currentVal;
                        m_holdVal = 0;
                        m_bNoPrevEqu = true;

                        if (!m_HistoryCollector.FOpndAddedToHistory())
                        {
                            m_HistoryCollector.AddOpenBraceToHistory();
                            m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                            m_HistoryCollector.AddCloseBraceToHistory();
                        }

                        m_HistoryCollector.AddBinOpToHistory(m_nOpCode, m_fIntegerMode);
                        m_bChangeOp = true;
                        m_nPrevOpCode = 0;

                        while (m_precedenceOpCount > 0)
                        {
                            m_precedenceOpCount--;
                            m_nPrecOp[m_precedenceOpCount] = 0;
                        }
                    }

                    if (m_bRecord && !m_fIntegerMode && m_input.TryAddDecimalPt())
                    {
                        DisplayNum();
                        break;
                    }
                    HandleErrorCommand(wParam);
                    break;

                case CCommand.IDC_INV:
                    m_bInv = !m_bInv;
                    break;
            }
        }

        private void ResolveHighestPrecedenceOperation()
        {
            if (m_nOpCode != 0)
            {
                if (m_bNoPrevEqu)
                {
                    m_holdVal = m_currentVal;
                }
                else
                {
                    m_currentVal = m_holdVal;
                    DisplayNum();
                    m_HistoryCollector.AddBinOpToHistory(m_nOpCode, m_fIntegerMode, false);
                    m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                }

                m_currentVal = DoOperation(m_nOpCode, m_currentVal, m_lastVal);
                m_nPrevOpCode = m_nOpCode;
                m_lastVal = m_currentVal;

                if (!m_bError)
                {
                    DisplayNum();
                }

                m_bNoPrevEqu = false;
            }
            else if (!m_bError)
            {
                DisplayNum();
            }
        }

        public void CheckAndAddLastBinOpToHistory(bool addToHistory = true)
        {
            if (m_bChangeOp)
            {
                if (m_HistoryCollector.FOpndAddedToHistory())
                {
                    m_HistoryCollector.RemoveLastOpndFromHistory();
                }
            }
            else if (m_HistoryCollector.FOpndAddedToHistory() && !m_bError)
            {
                if ((CalcUtils.IsUnaryOpCode((uint)m_nLastCom) || CCommand.IDC_SIGN == m_nLastCom || CCommand.IDC_CLOSEP == m_nLastCom) && 0 == m_openParenCount)
                {
                    if (addToHistory)
                    {
                        m_HistoryCollector.CompleteHistoryLine(GroupDigitsPerRadix(m_numberString, m_radix));
                    }
                }
                else
                {
                    m_HistoryCollector.RemoveLastOpndFromHistory();
                }
            }
        }

        private void SetPrimaryDisplay(string szText, bool isError = false)
        {
            m_pCalcDisplay?.SetPrimaryDisplay(szText, isError);
            m_pCalcDisplay?.SetIsInError(isError);
        }

        private void DisplayAnnounceBinaryOperator()
        {
            m_pCalcDisplay?.BinaryOperatorReceived();
        }

        public bool IsCurrentTooBigForTrig()
        {
            return m_currentVal >= m_maxTrigonometricNum;
        }

        public uint GetCurrentRadix() => m_radix;

        public string GetCurrentResultForRadix(uint radix, int precision, bool groupDigitsPerRadix)
        {
            Rational rat = m_bRecord ? m_input.ToRational(m_radix, m_precision) : m_currentVal;
            Ratpak.ChangeConstants(m_radix, precision);

            string numberString = GetStringForDisplay(rat, radix);
            if (!string.IsNullOrEmpty(numberString))
            {
                Ratpak.ChangeConstants(m_radix, m_precision);
            }

            return groupDigitsPerRadix ? GroupDigitsPerRadix(numberString, radix) : numberString;
        }

        public string GetStringForDisplay(Rational rat, uint radix)
        {
            if (!m_fIntegerMode)
            {
                return rat.ToString(radix, m_nFE, m_precision);
            }
            else
            {
                var tempRat = TruncateNumForIntMath(rat);
                try
                {
                    ulong w64Bits = tempRat.ToUInt64_t();
                    bool fMsb = ((w64Bits >> (m_dwWordBitWidth - 1)) & 1) != 0;
                    if ((radix == 10) && fMsb)
                    {
                        tempRat = -((tempRat ^ GetChopNumber()) + 1);
                    }
                    return tempRat.ToString(radix, m_nFE, m_precision);
                }
                catch
                {
                    return string.Empty;
                }
            }
        }

        private double GenerateRandomNumber()
        {
            if (m_randomGenerator == null)
            {
                m_randomGenerator = new Random();
            }
            return m_randomGenerator.NextDouble();
        }

        public void DisplayNum()
        {
            if (m_bRecord)
            {
                m_numberString = m_input.ToString(m_radix);
            }
            else
            {
                if (m_fIntegerMode)
                {
                    m_currentVal = TruncateNumForIntMath(m_currentVal);
                }
                m_numberString = GetStringForDisplay(m_currentVal, m_radix);
            }

            if ((m_radix == 10) && IsNumberInvalid(m_numberString, MAX_EXPONENT, m_precision, m_radix) != 0)
            {
                DisplayError(CalcErr.CALC_E_OVERFLOW);
            }
            else
            {
                SetPrimaryDisplay(GroupDigitsPerRadix(m_numberString, m_radix));
            }
        }

        public int IsNumberInvalid(string numberString, int iMaxExp, int iMaxMantissa, uint radix)
        {
            if (string.IsNullOrEmpty(numberString)) return 0;

            if (radix == 10)
            {
                string escapedDec = Regex.Escape(m_decimalSeparator.ToString());
                string pattern = @"^[+-]?(\d*)[" + escapedDec + @"]?(\d*)(?:e[+-]?(\d*))?$";
                var match = Regex.Match(numberString, pattern, RegexOptions.IgnoreCase);
                if (match.Success)
                {
                    if (match.Groups[3].Length > iMaxExp)
                    {
                        return EngineStrings.IDS_ERR_INPUT_OVERFLOW;
                    }
                    string exp = match.Groups[1].Value.TrimStart('0');
                    int iMantissa = exp.Length + match.Groups[2].Length;
                    if (iMantissa > iMaxMantissa)
                    {
                        return EngineStrings.IDS_ERR_INPUT_OVERFLOW;
                    }
                }
                else
                {
                    return EngineStrings.IDS_ERR_UNK_CH;
                }
            }
            else
            {
                foreach (char c in numberString)
                {
                    if (radix == 16)
                    {
                        if (!char.IsDigit(c) && (c < 'A' || c > 'F'))
                        {
                            return EngineStrings.IDS_ERR_UNK_CH;
                        }
                    }
                    else if (c < '0' || c >= '0' + radix)
                    {
                        return EngineStrings.IDS_ERR_UNK_CH;
                    }
                }
            }

            return 0;
        }

        public static List<uint> DigitGroupingStringToGroupingVector(string groupingString)
        {
            var grouping = new List<uint>();
            if (string.IsNullOrEmpty(groupingString)) return grouping;

            string[] parts = groupingString.Split(';');
            foreach (var part in parts)
            {
                if (uint.TryParse(part, out uint val) && val < MAX_GROUPING_SIZE)
                {
                    grouping.Add(val);
                }
            }

            return grouping;
        }

        public string GroupDigitsPerRadix(string numberString, uint radix)
        {
            if (string.IsNullOrEmpty(numberString)) return string.Empty;

            switch (radix)
            {
                case 10:
                    return GroupDigits(m_groupSeparator.ToString(), m_decGrouping, numberString, numberString[0] == '-');
                case 8:
                    return GroupDigits(" ", new List<uint> { 3, 0 }, numberString);
                case 2:
                case 16:
                    return GroupDigits(" ", new List<uint> { 4, 0 }, numberString);
                default:
                    return numberString;
            }
        }

        public string GroupDigits(string delimiter, List<uint> grouping, string displayString, bool isNumNegative = false)
        {
            if (string.IsNullOrEmpty(delimiter) || grouping == null || grouping.Count == 0 || string.IsNullOrEmpty(displayString))
            {
                return displayString;
            }

            int exp = displayString.IndexOf('e');
            bool hasExponent = (exp != -1);

            int dec = displayString.IndexOf(m_decimalSeparator);
            bool hasDecimal = (dec != -1);

            int intPartEnd = displayString.Length;
            if (hasDecimal)
            {
                intPartEnd = dec;
            }
            else if (hasExponent)
            {
                intPartEnd = exp;
            }

            int startIndex = isNumNegative ? 1 : 0;
            string intPart = displayString.Substring(startIndex, intPartEnd - startIndex);

            var sb = new StringBuilder();
            int groupItr = 0;
            uint currGrouping = grouping[0];
            uint groupingSize = 0;

            for (int i = intPart.Length - 1; i >= 0; i--)
            {
                sb.Append(intPart[i]);
                groupingSize++;

                if (currGrouping != 0 && (groupingSize % currGrouping) == 0 && i > 0)
                {
                    sb.Append(delimiter);
                    groupingSize = 0;

                    if (groupItr < grouping.Count)
                    {
                        groupItr++;
                        currGrouping = 0;
                        for (; groupItr < grouping.Count; ++groupItr)
                        {
                            if (grouping[groupItr] != 0)
                            {
                                currGrouping = grouping[groupItr];
                                break;
                            }
                            currGrouping = grouping[groupItr - 1];
                        }
                    }
                }
            }

            if (isNumNegative)
            {
                sb.Append('-');
            }

            char[] chars = sb.ToString().ToCharArray();
            Array.Reverse(chars);
            string result = new string(chars);

            if (hasDecimal)
            {
                result += displayString.Substring(dec);
            }
            else if (hasExponent)
            {
                result += displayString.Substring(exp);
            }

            return result;
        }

        public Rational TruncateNumForIntMath(Rational rat)
        {
            if (!m_fIntegerMode) return rat;

            var result = RationalMath.Integer(rat);
            if (result < 0)
            {
                result = -(result) - 1;
                result ^= GetChopNumber();
            }

            result &= GetChopNumber();
            return result;
        }

        public Rational SciCalcFunctions(Rational rat, uint op)
        {
            Rational result = 0;
            try
            {
                switch (op)
                {
                    case CCommand.IDC_CHOP:
                        result = m_bInv ? RationalMath.Frac(rat) : RationalMath.Integer(rat);
                        break;
                    case CCommand.IDC_COM:
                        if (m_radix == 10 && !m_fIntegerMode)
                        {
                            result = -(RationalMath.Integer(rat) + 1);
                        }
                        else
                        {
                            result = rat ^ GetChopNumber();
                        }
                        break;
                    case CCommand.IDC_ROL:
                    case CCommand.IDC_ROLC:
                        if (m_fIntegerMode)
                        {
                            result = RationalMath.Integer(rat);
                            ulong w64Bits = result.ToUInt64_t();
                            ulong msb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;
                            w64Bits <<= 1;
                            if (op == CCommand.IDC_ROL)
                            {
                                w64Bits |= msb;
                            }
                            else
                            {
                                w64Bits |= m_carryBit;
                                m_carryBit = msb;
                            }
                            result = w64Bits;
                        }
                        break;
                    case CCommand.IDC_ROR:
                    case CCommand.IDC_RORC:
                        if (m_fIntegerMode)
                        {
                            result = RationalMath.Integer(rat);
                            ulong w64Bits = result.ToUInt64_t();
                            ulong lsb = (w64Bits & 0x01) == 1 ? 1UL : 0UL;
                            w64Bits >>= 1;
                            if (op == CCommand.IDC_ROR)
                            {
                                w64Bits |= (lsb << (m_dwWordBitWidth - 1));
                            }
                            else
                            {
                                w64Bits |= (m_carryBit << (m_dwWordBitWidth - 1));
                                m_carryBit = lsb;
                            }
                            result = w64Bits;
                        }
                        break;
                    case CCommand.IDC_PERCENT:
                        if (m_nOpCode == CCommand.IDC_MUL || m_nOpCode == CCommand.IDC_DIV)
                        {
                            result = rat / 100;
                        }
                        else
                        {
                            result = rat * (m_lastVal / 100);
                        }
                        break;
                    case CCommand.IDC_SIN:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ASin(rat, m_angletype) : RationalMath.Sin(rat, m_angletype);
                        }
                        break;
                    case CCommand.IDC_SINH:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ASinh(rat) : RationalMath.Sinh(rat);
                        }
                        break;
                    case CCommand.IDC_COS:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ACos(rat, m_angletype) : RationalMath.Cos(rat, m_angletype);
                        }
                        break;
                    case CCommand.IDC_COSH:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ACosh(rat) : RationalMath.Cosh(rat);
                        }
                        break;
                    case CCommand.IDC_TAN:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ATan(rat, m_angletype) : RationalMath.Tan(rat, m_angletype);
                        }
                        break;
                    case CCommand.IDC_TANH:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ATanh(rat) : RationalMath.Tanh(rat);
                        }
                        break;
                    case CCommand.IDC_SEC:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ACos(RationalMath.Invert(rat), m_angletype) : RationalMath.Invert(RationalMath.Cos(rat, m_angletype));
                        }
                        break;
                    case CCommand.IDC_CSC:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ASin(RationalMath.Invert(rat), m_angletype) : RationalMath.Invert(RationalMath.Sin(rat, m_angletype));
                        }
                        break;
                    case CCommand.IDC_COT:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ATan(RationalMath.Invert(rat), m_angletype) : RationalMath.Invert(RationalMath.Tan(rat, m_angletype));
                        }
                        break;
                    case CCommand.IDC_SECH:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ACosh(RationalMath.Invert(rat)) : RationalMath.Invert(RationalMath.Cosh(rat));
                        }
                        break;
                    case CCommand.IDC_CSCH:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ASinh(RationalMath.Invert(rat)) : RationalMath.Invert(RationalMath.Sinh(rat));
                        }
                        break;
                    case CCommand.IDC_COTH:
                        if (!m_fIntegerMode)
                        {
                            result = m_bInv ? RationalMath.ATanh(RationalMath.Invert(rat)) : RationalMath.Invert(RationalMath.Tanh(rat));
                        }
                        break;
                    case CCommand.IDC_REC:
                        result = RationalMath.Invert(rat);
                        break;
                    case CCommand.IDC_SQR:
                        result = RationalMath.Pow(rat, 2);
                        break;
                    case CCommand.IDC_SQRT:
                        result = RationalMath.Root(rat, 2);
                        break;
                    case CCommand.IDC_CUBEROOT:
                    case CCommand.IDC_CUB:
                        result = (CCommand.IDC_CUBEROOT == op) ? RationalMath.Root(rat, 3) : RationalMath.Pow(rat, 3);
                        break;
                    case CCommand.IDC_LOG:
                        result = RationalMath.Log10(rat);
                        break;
                    case CCommand.IDC_POW10:
                        result = RationalMath.Pow(10, rat);
                        break;
                    case CCommand.IDC_POW2:
                        result = RationalMath.Pow(2, rat);
                        break;
                    case CCommand.IDC_LN:
                        result = m_bInv ? RationalMath.Exp(rat) : RationalMath.Log(rat);
                        break;
                    case CCommand.IDC_FAC:
                        result = RationalMath.Fact(rat);
                        break;
                    case CCommand.IDC_DEGREES:
                        ProcessCommand(CCommand.IDC_INV);
                        goto case CCommand.IDC_DMS;
                    case CCommand.IDC_DMS:
                        if (!m_fIntegerMode)
                        {
                            Rational shftRat = m_bInv ? 100 : 60;
                            Rational degreeRat = RationalMath.Integer(rat);
                            Rational minuteRat = (rat - degreeRat) * shftRat;
                            Rational secondRat = minuteRat;
                            minuteRat = RationalMath.Integer(minuteRat);
                            secondRat = (secondRat - minuteRat) * shftRat;
                            shftRat = m_bInv ? 60 : 100;
                            secondRat /= shftRat;
                            minuteRat = (minuteRat + secondRat) / shftRat;
                            result = degreeRat + minuteRat;
                        }
                        break;
                    case CCommand.IDC_CEIL:
                        result = (RationalMath.Frac(rat) > 0) ? RationalMath.Integer(rat + 1) : RationalMath.Integer(rat);
                        break;
                    case CCommand.IDC_FLOOR:
                        result = (RationalMath.Frac(rat) < 0) ? RationalMath.Integer(rat - 1) : RationalMath.Integer(rat);
                        break;
                    case CCommand.IDC_ABS:
                        result = RationalMath.Abs(rat);
                        break;
                }
            }
            catch (Exception ex)
            {
                if (uint.TryParse(ex.Message, out uint nErrCode))
                {
                    DisplayError(nErrCode);
                }
                else
                {
                    DisplayError(CalcErr.CALC_E_DOMAIN);
                }
                result = rat;
            }

            return result;
        }

        public void DisplayError(uint nError)
        {
            string errorString = GetString((int)(EngineStrings.IDS_ERRORS_FIRST + CalcErr.SCODE_CODE(nError)));
            SetPrimaryDisplay(errorString, true);
            m_bError = true;
            m_HistoryCollector.ClearHistoryLine(errorString);
        }

        public Rational DoOperation(int operation, Rational lhs, Rational rhs)
        {
            var result = (lhs != 0 ? lhs : 0);
            try
            {
                switch (operation)
                {
                    case CCommand.IDC_AND:
                        result &= rhs;
                        break;
                    case CCommand.IDC_OR:
                        result |= rhs;
                        break;
                    case CCommand.IDC_XOR:
                        result ^= rhs;
                        break;
                    case CCommand.IDC_NAND:
                        result = (result & rhs) ^ GetChopNumber();
                        break;
                    case CCommand.IDC_NOR:
                        result = (result | rhs) ^ GetChopNumber();
                        break;
                    case CCommand.IDC_RSHF:
                    {
                        if (m_fIntegerMode && result >= m_dwWordBitWidth)
                        {
                            throw new Exception(CalcErr.CALC_E_NORESULT.ToString());
                        }
                        ulong w64Bits = rhs.ToUInt64_t();
                        bool fMsb = ((w64Bits >> (m_dwWordBitWidth - 1)) & 1) != 0;
                        Rational holdVal = result;
                        result = rhs >> holdVal;
                        if (fMsb)
                        {
                            result = RationalMath.Integer(result);
                            var tempRat = GetChopNumber() >> holdVal;
                            tempRat = RationalMath.Integer(tempRat);
                            result |= tempRat ^ GetChopNumber();
                        }
                        break;
                    }
                    case CCommand.IDC_RSHFL:
                    {
                        if (m_fIntegerMode && result >= m_dwWordBitWidth)
                        {
                            throw new Exception(CalcErr.CALC_E_NORESULT.ToString());
                        }
                        result = rhs >> result;
                        break;
                    }
                    case CCommand.IDC_LSHF:
                    {
                        if (m_fIntegerMode && result >= m_dwWordBitWidth)
                        {
                            throw new Exception(CalcErr.CALC_E_NORESULT.ToString());
                        }
                        result = rhs << result;
                        break;
                    }
                    case CCommand.IDC_ADD:
                        result += rhs;
                        break;
                    case CCommand.IDC_SUB:
                        result = rhs - result;
                        break;
                    case CCommand.IDC_MUL:
                        result *= rhs;
                        break;
                    case CCommand.IDC_DIV:
                    case CCommand.IDC_MOD:
                    {
                        int iNumeratorSign = 1, iDenominatorSign = 1;
                        var temp = result;
                        result = rhs;

                        if (m_fIntegerMode)
                        {
                            ulong w64Bits = rhs.ToUInt64_t();
                            bool fMsb = ((w64Bits >> (m_dwWordBitWidth - 1)) & 1) != 0;
                            if (fMsb)
                            {
                                result = (rhs ^ GetChopNumber()) + 1;
                                iNumeratorSign = -1;
                            }

                            w64Bits = temp.ToUInt64_t();
                            fMsb = ((w64Bits >> (m_dwWordBitWidth - 1)) & 1) != 0;
                            if (fMsb)
                            {
                                temp = (temp ^ GetChopNumber()) + 1;
                                iDenominatorSign = -1;
                            }
                        }

                        if (operation == CCommand.IDC_DIV)
                        {
                            result /= temp;
                            if (m_fIntegerMode && (iNumeratorSign * iDenominatorSign) == -1)
                            {
                                result = -(RationalMath.Integer(result));
                            }
                        }
                        else
                        {
                            if (m_fIntegerMode)
                            {
                                result %= temp;
                                if (iNumeratorSign == -1)
                                {
                                    result = -(RationalMath.Integer(result));
                                }
                            }
                            else
                            {
                                result = RationalMath.Mod(result, temp);
                            }
                        }
                        break;
                    }
                    case CCommand.IDC_PWR:
                        result = RationalMath.Pow(rhs, result);
                        break;
                    case CCommand.IDC_ROOT:
                        result = RationalMath.Root(rhs, result);
                        break;
                    case CCommand.IDC_LOGBASEY:
                        result = RationalMath.Log(rhs) / RationalMath.Log(result);
                        break;
                }
            }
            catch (Exception ex)
            {
                if (uint.TryParse(ex.Message, out uint dwErrCode))
                {
                    DisplayError(dwErrCode);
                }
                else
                {
                    DisplayError(CalcErr.CALC_E_DOMAIN);
                }
                result = lhs;
            }

            return result;
        }

        public void SetRadixTypeAndNumWidth(RadixType radixtype, NUM_WIDTH numwidth)
        {
            if (m_fIntegerMode)
            {
                ulong w64Bits = m_currentVal.ToUInt64_t();
                bool fMsb = ((w64Bits >> (m_dwWordBitWidth - 1)) & 1) != 0;
                if (fMsb)
                {
                    var tempResult = m_currentVal ^ GetChopNumber();
                    m_currentVal = -(tempResult + 1);
                }
            }

            if (radixtype >= RadixType.Hex && radixtype <= RadixType.Binary)
            {
                m_radix = NRadixFromRadixType(radixtype);
            }

            if (numwidth >= NUM_WIDTH.QWORD_WIDTH && numwidth <= NUM_WIDTH.BYTE_WIDTH)
            {
                m_numwidth = numwidth;
                m_dwWordBitWidth = DwWordBitWidthFromNumWidth(numwidth);
            }

            BaseOrPrecisionChanged();
            DisplayNum();
        }

        public static int DwWordBitWidthFromNumWidth(NUM_WIDTH numwidth)
        {
            switch (numwidth)
            {
                case NUM_WIDTH.DWORD_WIDTH:
                    return 32;
                case NUM_WIDTH.WORD_WIDTH:
                    return 16;
                case NUM_WIDTH.BYTE_WIDTH:
                    return 8;
                case NUM_WIDTH.QWORD_WIDTH:
                default:
                    return 64;
            }
        }

        public static uint NRadixFromRadixType(RadixType radixtype)
        {
            switch (radixtype)
            {
                case RadixType.Hex:
                    return 16;
                case RadixType.Octal:
                    return 8;
                case RadixType.Binary:
                    return 2;
                case RadixType.Decimal:
                default:
                    return 10;
            }
        }

        public bool TryToggleBit(ref Rational rat, uint wbitno)
        {
            uint wmax = (uint)DwWordBitWidthFromNumWidth(m_numwidth);
            if (wbitno >= wmax)
            {
                return false;
            }

            Rational result = RationalMath.Integer(rat);
            result = (result != 0 ? result : 0);
            rat = result ^ RationalMath.Pow(2, (int)wbitno);
            return true;
        }

        public static int QuickLog2(int iNum)
        {
            int iRes = 0;
            while ((iNum & 1) == 0 && iNum != 0)
            {
                iRes++;
                iNum >>= 1;
            }

            iNum >>= 1;
            if (iNum != 0)
            {
                for (iNum >>= 1; iNum != 0; iNum >>= 1)
                {
                    iRes++;
                }
                iRes += 2;
            }

            return iRes;
        }

        public void UpdateMaxIntDigits()
        {
            if (m_radix == 10)
            {
                if (m_fIntegerMode)
                {
                    m_cIntDigitsSav = GetMaxDecimalValueString().Length - 1;
                }
                else
                {
                    m_cIntDigitsSav = m_precision;
                }
            }
            else
            {
                m_cIntDigitsSav = m_dwWordBitWidth / QuickLog2((int)m_radix);
            }
        }

        public static void ChangeBaseConstants(uint radix, int maxIntDigits, int precision)
        {
            if (10 == radix)
            {
                Ratpak.ChangeConstants(radix, precision);
            }
            else
            {
                Ratpak.ChangeConstants(radix, maxIntDigits + 1);
            }
        }

        public void BaseOrPrecisionChanged()
        {
            UpdateMaxIntDigits();
            ChangeBaseConstants(m_radix, m_cIntDigitsSav, m_precision);
        }

        public void ChangePrecision(int precision)
        {
            m_precision = precision;
            Ratpak.ChangeConstants(m_radix, precision);
        }
    }
}
