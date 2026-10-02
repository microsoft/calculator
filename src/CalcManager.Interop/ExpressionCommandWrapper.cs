// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using CalcManager;

namespace CalcManager.Interop
{
    public sealed class ExpressionCommandWrapper
    {
        public CommandType Type { get; }
        public int Command { get; }
        public int[] Commands { get; }
        public bool IsNegative { get; }
        public bool IsDecimalPresent { get; }
        public bool IsSciFmt { get; }

        public ExpressionCommandWrapper()
        {
            Type = CommandType.UnaryCommand;
            Command = 0;
            Commands = Array.Empty<int>();
            IsNegative = false;
            IsDecimalPresent = false;
            IsSciFmt = false;
        }

        public ExpressionCommandWrapper(
            CommandType type,
            int command,
            int[] commands,
            bool isNegative,
            bool isDecimalPresent,
            bool isSciFmt)
        {
            Type = type;
            Command = command;
            Commands = commands ?? Array.Empty<int>();
            IsNegative = isNegative;
            IsDecimalPresent = isDecimalPresent;
            IsSciFmt = isSciFmt;
        }

        public ExpressionCommandWrapper(IExpressionCommand command)
        {
            if (command == null)
            {
                Type = CommandType.UnaryCommand;
                Command = 0;
                Commands = Array.Empty<int>();
                IsNegative = false;
                IsDecimalPresent = false;
                IsSciFmt = false;
                return;
            }

            var nativeType = command.GetCommandType();
            Type = (CommandType)(int)nativeType;

            switch (nativeType)
            {
                case CalculationManager.CommandType.BinaryCommand:
                    if (command is CBinaryCommand binaryCmd)
                    {
                        Command = binaryCmd.GetCommand();
                    }
                    Commands = Array.Empty<int>();
                    break;

                case CalculationManager.CommandType.Parentheses:
                    if (command is CParentheses parenCmd)
                    {
                        Command = parenCmd.GetCommand();
                    }
                    Commands = Array.Empty<int>();
                    break;

                case CalculationManager.CommandType.UnaryCommand:
                    if (command is CUnaryCommand unaryCmd)
                    {
                        var cmds = unaryCmd.GetCommands();
                        Commands = cmds != null ? cmds.ToArray() : Array.Empty<int>();
                    }
                    else
                    {
                        Commands = Array.Empty<int>();
                    }
                    break;

                case CalculationManager.CommandType.OperandCommand:
                    if (command is COpndCommand opndCmd)
                    {
                        var cmds = opndCmd.GetCommands();
                        Commands = cmds != null ? cmds.ToArray() : Array.Empty<int>();
                        IsNegative = opndCmd.IsNegative();
                        IsDecimalPresent = opndCmd.IsDecimalPresent();
                        IsSciFmt = opndCmd.IsSciFmt();
                    }
                    else
                    {
                        Commands = Array.Empty<int>();
                    }
                    break;

                default:
                    Commands = Array.Empty<int>();
                    break;
            }
        }

        public IExpressionCommand ToUnderlying()
        {
            switch (Type)
            {
                case CommandType.UnaryCommand:
                    if (Commands.Length == 1)
                    {
                        return new CUnaryCommand(Commands[0]);
                    }
                    if (Commands.Length == 2)
                    {
                        return new CUnaryCommand(Commands[0], Commands[1]);
                    }
                    throw new ArgumentException("ill-formed unary command.");

                case CommandType.BinaryCommand:
                    return new CBinaryCommand(Command);

                case CommandType.Parentheses:
                    return new CParentheses(Command);

                case CommandType.OperandCommand:
                    var subCommands = new List<int>(Commands);
                    return new COpndCommand(subCommands, IsNegative, IsDecimalPresent, IsSciFmt);

                default:
                    throw new ArgumentException("unhandled command type.");
            }
        }
    }
}
