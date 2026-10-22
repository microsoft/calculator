// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using CalcManager.CalculationManager;
using CalcManager.Ratpack;

namespace CalcManager.CEngine
{
    public class HistoryCollector
    {
        public const int MAXPRECDEPTH = 25;
        private const int ASCII_0 = 48;

        private IHistoryDisplay m_pHistoryDisplay;
        private ICalcDisplay m_pCalcDisplay;

        private int m_iCurLineHistStart;
        private int m_lastOpStartIndex;
        private int m_lastBinOpStartIndex;
        private readonly int[] m_operandIndices = new int[MAXPRECDEPTH];
        private int m_curOperandIndex;
        private bool m_bLastOpndBrace;
        private char m_decimalSymbol;
        private List<Tuple<string, int>> m_spTokens;
        private List<IExpressionCommand> m_spCommands;

        public HistoryCollector(ICalcDisplay pCalcDisplay, IHistoryDisplay pHistoryDisplay, char decimalSymbol)
        {
            m_pHistoryDisplay = pHistoryDisplay;
            m_pCalcDisplay = pCalcDisplay;
            m_iCurLineHistStart = -1;
            m_decimalSymbol = decimalSymbol;
            ReinitHistory();
        }

        private void ReinitHistory()
        {
            m_lastOpStartIndex = -1;
            m_lastBinOpStartIndex = -1;
            m_curOperandIndex = 0;
            m_bLastOpndBrace = false;
            m_spTokens?.Clear();
            m_spCommands?.Clear();
        }

        public void AddOpndToHistory(string numStr, Rational rat, bool fRepetition = false)
        {
            int iCommandEnd = AddCommand(GetOperandCommandsFromString(numStr, rat));
            m_lastOpStartIndex = IchAddSzToEquationSz(numStr, iCommandEnd);

            if (fRepetition)
            {
                SetExpressionDisplay();
            }
            m_bLastOpndBrace = false;
            m_lastBinOpStartIndex = -1;
        }

        public void RemoveLastOpndFromHistory()
        {
            TruncateEquationSzFromIch(m_lastOpStartIndex);
            SetExpressionDisplay();
            m_lastOpStartIndex = -1;
        }

        public void AddBinOpToHistory(int nOpCode, bool isIntegerMode, bool fNoRepetition = true)
        {
            int iCommandEnd = AddCommand(new CBinaryCommand(nOpCode));
            m_lastBinOpStartIndex = IchAddSzToEquationSz(" ", -1);

            IchAddSzToEquationSz(CCalcEngine.OpCodeToBinaryString(nOpCode, isIntegerMode), iCommandEnd);
            IchAddSzToEquationSz(" ", -1);

            if (fNoRepetition)
            {
                SetExpressionDisplay();
            }
            m_lastOpStartIndex = -1;
        }

        public void ChangeLastBinOp(int nOpCode, bool fPrecInvToHigher, bool isIntegerMode)
        {
            TruncateEquationSzFromIch(m_lastBinOpStartIndex);
            if (fPrecInvToHigher)
            {
                EnclosePrecInversionBrackets();
            }
            AddBinOpToHistory(nOpCode, isIntegerMode);
        }

        public void PushLastOpndStart(int ichOpndStart = -1)
        {
            int ich = (ichOpndStart == -1) ? m_lastOpStartIndex : ichOpndStart;
            if (m_curOperandIndex < m_operandIndices.Length)
            {
                m_operandIndices[m_curOperandIndex++] = ich;
            }
        }

        public void PopLastOpndStart()
        {
            if (m_curOperandIndex > 0)
            {
                m_lastOpStartIndex = m_operandIndices[--m_curOperandIndex];
            }
        }

        public void AddOpenBraceToHistory()
        {
            AddCommand(new CParentheses(CCommand.IDC_OPENP));
            int ichOpndStart = IchAddSzToEquationSz(CCalcEngine.OpCodeToString(CCommand.IDC_OPENP), -1);
            PushLastOpndStart(ichOpndStart);

            SetExpressionDisplay();
            m_lastBinOpStartIndex = -1;
        }

        public void AddCloseBraceToHistory()
        {
            AddCommand(new CParentheses(CCommand.IDC_CLOSEP));
            IchAddSzToEquationSz(CCalcEngine.OpCodeToString(CCommand.IDC_CLOSEP), -1);
            SetExpressionDisplay();
            PopLastOpndStart();

            m_lastBinOpStartIndex = -1;
            m_bLastOpndBrace = true;
        }

        public void EnclosePrecInversionBrackets()
        {
            int ichStart = (m_curOperandIndex > 0) ? m_operandIndices[m_curOperandIndex - 1] : 0;
            InsertSzInEquationSz(CCalcEngine.OpCodeToString(CCommand.IDC_OPENP), -1, ichStart);
            IchAddSzToEquationSz(CCalcEngine.OpCodeToString(CCommand.IDC_CLOSEP), -1);
        }

        public bool FOpndAddedToHistory()
        {
            return -1 != m_lastOpStartIndex;
        }

        public void AddUnaryOpToHistory(int nOpCode, bool fInv, AngleType angletype)
        {
            int iCommandEnd;
            if (CCommand.IDC_PERCENT == nOpCode)
            {
                iCommandEnd = AddCommand(new CUnaryCommand(nOpCode));
                IchAddSzToEquationSz(CCalcEngine.OpCodeToString(nOpCode), iCommandEnd);
            }
            else
            {
                IOperatorCommand spExpressionCommand;
                if (CCommand.IDC_SIGN == nOpCode)
                {
                    spExpressionCommand = new CUnaryCommand(nOpCode);
                }
                else
                {
                    Command angleOpCode;
                    if (angletype == AngleType.Degrees)
                    {
                        angleOpCode = Command.CommandDEG;
                    }
                    else if (angletype == AngleType.Radians)
                    {
                        angleOpCode = Command.CommandRAD;
                    }
                    else
                    {
                        angleOpCode = Command.CommandGRAD;
                    }

                    int command = nOpCode;
                    switch (nOpCode)
                    {
                        case CCommand.IDC_SIN:
                            command = fInv ? (int)Command.CommandASIN : CCommand.IDC_SIN;
                            spExpressionCommand = new CUnaryCommand((int)angleOpCode, command);
                            break;
                        case CCommand.IDC_COS:
                            command = fInv ? (int)Command.CommandACOS : CCommand.IDC_COS;
                            spExpressionCommand = new CUnaryCommand((int)angleOpCode, command);
                            break;
                        case CCommand.IDC_TAN:
                            command = fInv ? (int)Command.CommandATAN : CCommand.IDC_TAN;
                            spExpressionCommand = new CUnaryCommand((int)angleOpCode, command);
                            break;
                        case CCommand.IDC_SINH:
                            command = fInv ? (int)Command.CommandASINH : CCommand.IDC_SINH;
                            spExpressionCommand = new CUnaryCommand(command);
                            break;
                        case CCommand.IDC_COSH:
                            command = fInv ? (int)Command.CommandACOSH : CCommand.IDC_COSH;
                            spExpressionCommand = new CUnaryCommand(command);
                            break;
                        case CCommand.IDC_TANH:
                            command = fInv ? (int)Command.CommandATANH : CCommand.IDC_TANH;
                            spExpressionCommand = new CUnaryCommand(command);
                            break;
                        case CCommand.IDC_SEC:
                            command = fInv ? (int)Command.CommandASEC : CCommand.IDC_SEC;
                            spExpressionCommand = new CUnaryCommand((int)angleOpCode, command);
                            break;
                        case CCommand.IDC_CSC:
                            command = fInv ? (int)Command.CommandACSC : CCommand.IDC_CSC;
                            spExpressionCommand = new CUnaryCommand((int)angleOpCode, command);
                            break;
                        case CCommand.IDC_COT:
                            command = fInv ? (int)Command.CommandACOT : CCommand.IDC_COT;
                            spExpressionCommand = new CUnaryCommand((int)angleOpCode, command);
                            break;
                        case CCommand.IDC_SECH:
                            command = fInv ? (int)Command.CommandASECH : CCommand.IDC_SECH;
                            spExpressionCommand = new CUnaryCommand(command);
                            break;
                        case CCommand.IDC_CSCH:
                            command = fInv ? (int)Command.CommandACSCH : CCommand.IDC_CSCH;
                            spExpressionCommand = new CUnaryCommand(command);
                            break;
                        case CCommand.IDC_COTH:
                            command = fInv ? (int)Command.CommandACOTH : CCommand.IDC_COTH;
                            spExpressionCommand = new CUnaryCommand(command);
                            break;
                        case CCommand.IDC_LN:
                            command = fInv ? (int)Command.CommandPOWE : CCommand.IDC_LN;
                            spExpressionCommand = new CUnaryCommand(command);
                            break;
                        default:
                            spExpressionCommand = new CUnaryCommand(nOpCode);
                            break;
                    }
                }

                iCommandEnd = AddCommand(spExpressionCommand);

                string operandStr = CCalcEngine.OpCodeToUnaryString(nOpCode, fInv, angletype);
                if (!m_bLastOpndBrace)
                {
                    operandStr += CCalcEngine.OpCodeToString(CCommand.IDC_OPENP);
                }
                InsertSzInEquationSz(operandStr, iCommandEnd, m_lastOpStartIndex);

                if (!m_bLastOpndBrace)
                {
                    IchAddSzToEquationSz(CCalcEngine.OpCodeToString(CCommand.IDC_CLOSEP), -1);
                }
            }

            SetExpressionDisplay();
            m_bLastOpndBrace = false;
            m_lastBinOpStartIndex = -1;
        }

        public void CompleteHistoryLine(string numStr)
        {
            if (m_pHistoryDisplay != null)
            {
                uint addedItemIndex = m_pHistoryDisplay.AddToHistory(m_spTokens, m_spCommands, numStr);
                m_pCalcDisplay?.OnHistoryItemAdded(addedItemIndex);
            }

            m_spTokens = null;
            m_spCommands = null;
            m_iCurLineHistStart = -1;
            ReinitHistory();
        }

        public void CompleteEquation(string numStr)
        {
            IchAddSzToEquationSz(CCalcEngine.OpCodeToString(CCommand.IDC_EQU), -1);
            SetExpressionDisplay();
            CompleteHistoryLine(numStr);
        }

        public void ClearHistoryLine(string errStr)
        {
            if (string.IsNullOrEmpty(errStr))
            {
                m_pCalcDisplay?.SetExpressionDisplay(new List<Tuple<string, int>>(), new List<IExpressionCommand>());
                m_iCurLineHistStart = -1;
                ReinitHistory();
            }
        }

        private int IchAddSzToEquationSz(string str, int icommandIndex)
        {
            if (m_spTokens == null)
            {
                m_spTokens = new List<Tuple<string, int>>();
            }

            m_spTokens.Add(Tuple.Create(str, icommandIndex));
            return m_spTokens.Count - 1;
        }

        private void InsertSzInEquationSz(string str, int icommandIndex, int ich)
        {
            if (m_spTokens == null)
            {
                m_spTokens = new List<Tuple<string, int>>();
            }
            m_spTokens.Insert(ich, Tuple.Create(str, icommandIndex));
        }

        private void TruncateEquationSzFromIch(int ich)
        {
            if (m_spTokens == null || ich < 0 || ich >= m_spTokens.Count) return;

            int minIdx = -1;
            for (int i = ich; i < m_spTokens.Count; i++)
            {
                int curTokenId = m_spTokens[i].Item2;
                if (curTokenId != -1)
                {
                    if (minIdx == -1 || curTokenId < minIdx)
                    {
                        minIdx = curTokenId;
                    }
                }
            }

            if (minIdx != -1 && m_spCommands != null && minIdx < m_spCommands.Count)
            {
                m_spCommands.RemoveRange(minIdx, m_spCommands.Count - minIdx);
            }

            m_spTokens.RemoveRange(ich, m_spTokens.Count - ich);
        }

        private void SetExpressionDisplay()
        {
            m_pCalcDisplay?.SetExpressionDisplay(m_spTokens, m_spCommands);
        }

        public int AddCommand(IExpressionCommand spCommand)
        {
            if (m_spCommands == null)
            {
                m_spCommands = new List<IExpressionCommand>();
            }

            m_spCommands.Add(spCommand);
            return m_spCommands.Count - 1;
        }

        public void UpdateHistoryExpression(uint radix, int precision)
        {
            if (m_spTokens == null) return;

            for (int i = 0; i < m_spTokens.Count; i++)
            {
                var token = m_spTokens[i];
                int commandPosition = token.Item2;
                if (commandPosition != -1 && m_spCommands != null && commandPosition < m_spCommands.Count)
                {
                    var expCommand = m_spCommands[commandPosition];
                    if (expCommand is COpndCommand opndCommand)
                    {
                        string newTokenStr = opndCommand.GetString(radix, precision);
                        m_spTokens[i] = Tuple.Create(newTokenStr, commandPosition);
                        opndCommand.SetCommands(GetOperandCommandsFromString(newTokenStr));
                    }
                }
            }

            SetExpressionDisplay();
        }

        public void SetDecimalSymbol(char decimalSymbol)
        {
            m_decimalSymbol = decimalSymbol;
        }

        public List<int> GetOperandCommandsFromString(string numStr)
        {
            var commands = new List<int>();
            bool fNegative = !string.IsNullOrEmpty(numStr) && numStr[0] == '-';

            for (int i = (fNegative ? 1 : 0); i < numStr.Length; i++)
            {
                if (numStr[i] == m_decimalSymbol)
                {
                    commands.Add(CCommand.IDC_PNT);
                }
                else if (numStr[i] == 'e')
                {
                    commands.Add(CCommand.IDC_EXP);
                }
                else if (numStr[i] == '-')
                {
                    commands.Add(CCommand.IDC_SIGN);
                }
                else if (numStr[i] == '+')
                {
                    // Ignore.
                }
                else
                {
                    int num = (int)numStr[i] - ASCII_0;
                    num += CCommand.IDC_0;
                    commands.Add(num);
                }
            }

            if (fNegative)
            {
                commands.Add(CCommand.IDC_SIGN);
            }

            return commands;
        }

        public COpndCommand GetOperandCommandsFromString(string numStr, Rational rat)
        {
            var commands = new List<int>();
            bool fNegative = !string.IsNullOrEmpty(numStr) && numStr[0] == '-';
            bool fSciFmt = false;
            bool fDecimal = false;

            for (int i = (fNegative ? 1 : 0); i < numStr.Length; i++)
            {
                if (numStr[i] == m_decimalSymbol)
                {
                    commands.Add(CCommand.IDC_PNT);
                    if (!fSciFmt)
                    {
                        fDecimal = true;
                    }
                }
                else if (numStr[i] == 'e')
                {
                    commands.Add(CCommand.IDC_EXP);
                    fSciFmt = true;
                }
                else if (numStr[i] == '-')
                {
                    commands.Add(CCommand.IDC_SIGN);
                }
                else if (numStr[i] == '+')
                {
                    // Ignore.
                }
                else
                {
                    int num = (int)numStr[i] - ASCII_0;
                    num += CCommand.IDC_0;
                    commands.Add(num);
                }
            }

            var operandCommand = new COpndCommand(commands, fNegative, fDecimal, fSciFmt);
            operandCommand.Initialize(rat);
            return operandCommand;
        }

        public List<IExpressionCommand> GetCommands()
        {
            return m_spCommands != null ? new List<IExpressionCommand>(m_spCommands) : new List<IExpressionCommand>();
        }
    }
}
