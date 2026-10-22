// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using CalcManager.CalculationManager;
using CalcManager.CEngine;
using CalcManager.Ratpack;

namespace CalcManager
{
    public class CParentheses : IParenthesisCommand
    {
        private readonly int m_command;

        public CParentheses(int command)
        {
            m_command = command;
        }

        public int GetCommand() => m_command;

        public CommandType GetCommandType() => CommandType.Parentheses;

        public void Accept(ISerializeCommandVisitor commandVisitor)
        {
            commandVisitor.Visit(this);
        }
    }

    public class CUnaryCommand : IUnaryCommand
    {
        private List<int> m_command;

        public CUnaryCommand(int command)
        {
            m_command = new List<int> { command };
        }

        public CUnaryCommand(int command1, int command2)
        {
            m_command = new List<int> { command1, command2 };
        }

        public List<int> GetCommands() => m_command;

        public CommandType GetCommandType() => CommandType.UnaryCommand;

        public void SetCommand(int command)
        {
            m_command = new List<int> { command };
        }

        public void SetCommands(int command1, int command2)
        {
            m_command = new List<int> { command1, command2 };
        }

        public void Accept(ISerializeCommandVisitor commandVisitor)
        {
            commandVisitor.Visit(this);
        }
    }

    public class CBinaryCommand : IBinaryCommand
    {
        private int m_command;

        public CBinaryCommand(int command)
        {
            m_command = command;
        }

        public void SetCommand(int command)
        {
            m_command = command;
        }

        public int GetCommand() => m_command;

        public CommandType GetCommandType() => CommandType.BinaryCommand;

        public void Accept(ISerializeCommandVisitor commandVisitor)
        {
            commandVisitor.Visit(this);
        }
    }

    public class COpndCommand : IOpndCommand
    {
        private const char chNegate = '-';
        private const char chExp = 'e';
        private const char chPlus = '+';
        private const char chZero = '0';

        private List<int> m_commands;
        private bool m_fNegative;
        private bool m_fSciFmt;
        private bool m_fDecimal;
        private bool m_fInitialized;
        private string m_token;
        private Rational m_value;

        public COpndCommand(List<int> commands, bool fNegative, bool fDecimal, bool fSciFmt)
        {
            m_commands = commands != null ? new List<int>(commands) : new List<int>();
            m_fNegative = fNegative;
            m_fSciFmt = fSciFmt;
            m_fDecimal = fDecimal;
            m_fInitialized = false;
            m_value = default;
            m_token = string.Empty;
        }

        public void Initialize(Rational rat)
        {
            m_value = rat;
            m_fInitialized = true;
        }

        public List<int> GetCommands() => m_commands;

        public void SetCommands(List<int> commands)
        {
            m_commands = commands != null ? new List<int>(commands) : new List<int>();
        }

        public void AppendCommand(int command)
        {
            if (m_fSciFmt)
            {
                ClearAllAndAppendCommand((Command)command);
            }
            else
            {
                m_commands.Add(command);
            }

            if (command == CCommand.IDC_PNT)
            {
                m_fDecimal = true;
            }
        }

        public void ToggleSign()
        {
            foreach (int nOpCode in m_commands)
            {
                if (nOpCode != CCommand.IDC_0)
                {
                    m_fNegative = !m_fNegative;
                    break;
                }
            }
        }

        public void RemoveFromEnd()
        {
            if (m_fSciFmt)
            {
                ClearAllAndAppendCommand(Command.Command0);
            }
            else
            {
                int nCommands = m_commands.Count;
                if (nCommands <= 1)
                {
                    ClearAllAndAppendCommand(Command.Command0);
                }
                else
                {
                    int nOpCode = m_commands[nCommands - 1];
                    if (nOpCode == CCommand.IDC_PNT)
                    {
                        m_fDecimal = false;
                    }
                    m_commands.RemoveAt(nCommands - 1);
                }
            }
        }

        public bool IsNegative() => m_fNegative;
        public bool IsSciFmt() => m_fSciFmt;
        public bool IsDecimalPresent() => m_fDecimal;
        public CommandType GetCommandType() => CommandType.OperandCommand;

        public void ClearAllAndAppendCommand(Command command)
        {
            m_commands.Clear();
            m_commands.Add((int)command);
            m_fSciFmt = false;
            m_fNegative = false;
            m_fDecimal = false;
        }

        public string GetToken(char decimalSymbol)
        {
            int nCommands = m_commands.Count;
            var sb = new StringBuilder();

            for (int i = 0; i < nCommands; i++)
            {
                int nOpCode = m_commands[i];

                if (nOpCode == CCommand.IDC_PNT)
                {
                    sb.Append(decimalSymbol);
                }
                else if (nOpCode == CCommand.IDC_EXP)
                {
                    sb.Append(chExp);
                    if (i + 1 < nCommands)
                    {
                        int nextOpCode = m_commands[i + 1];
                        if (nextOpCode != CCommand.IDC_SIGN)
                        {
                            sb.Append(chPlus);
                        }
                    }
                }
                else if (nOpCode == CCommand.IDC_SIGN)
                {
                    sb.Append(chNegate);
                }
                else
                {
                    sb.Append(nOpCode - CCommand.IDC_0);
                }
            }

            m_token = sb.ToString();

            // Remove zeros
            for (int i = 0; i < m_token.Length; i++)
            {
                if (m_token[i] != chZero)
                {
                    if (m_token[i] == decimalSymbol)
                    {
                        m_token = m_token.Substring(i > 0 ? i - 1 : 0);
                    }
                    else
                    {
                        m_token = m_token.Substring(i);
                    }

                    if (m_fNegative)
                    {
                        m_token = chNegate + m_token;
                    }

                    return m_token;
                }
            }

            m_token = "0";
            return m_token;
        }

        public string GetString(uint radix, int precision)
        {
            if (m_fInitialized)
            {
                return m_value.ToString(radix, NumberFormat.Float, precision);
            }

            return string.Empty;
        }

        public void Accept(ISerializeCommandVisitor commandVisitor)
        {
            commandVisitor.Visit(this);
        }
    }
}
