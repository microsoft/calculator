// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using CalcManager.Interop;
using Windows.Storage.Streams;

namespace CalculatorApp.ViewModel.Common
{

    /// <summary>
    /// Represents a deserialized operand command.
    /// </summary>
    public class DeserializedOperand
    {
        public bool IsNegative { get; }
        public bool IsDecimalPresent { get; }
        public bool IsSciFmt { get; }
        public IList<int> Commands { get; }

        public DeserializedOperand(bool isNegative, bool isDecimalPresent, bool isSciFmt, IList<int> commands)
        {
            IsNegative = isNegative;
            IsDecimalPresent = isDecimalPresent;
            IsSciFmt = isSciFmt;
            Commands = commands;
        }
    }

    /// <summary>
    /// Represents a deserialized unary command.
    /// </summary>
    public class DeserializedUnary
    {
        public IList<int> Commands { get; }

        public DeserializedUnary(IList<int> commands)
        {
            Commands = commands;
        }
    }

    /// <summary>
    /// Represents a deserialized binary command.
    /// </summary>
    public class DeserializedBinary
    {
        public int Command { get; }

        public DeserializedBinary(int command)
        {
            Command = command;
        }
    }

    /// <summary>
    /// Represents a deserialized parentheses command.
    /// </summary>
    public class DeserializedParentheses
    {
        public int Command { get; }

        public DeserializedParentheses(int command)
        {
            Command = command;
        }
    }

    /// <summary>
    /// Deserializes expression commands from a DataReader.
    /// This is the C# port of the C++ CommandDeserializer.
    /// </summary>
    public class ExpressionCommandDeserializer
    {
        private readonly DataReader _dataReader;

        public ExpressionCommandDeserializer(DataReader dataReader)
        {
            _dataReader = dataReader;
        }

        public object Deserialize(CommandType cmdType)
        {
            switch (cmdType)
            {
                case CommandType.OperandCommand:
                    return DeserializeOperand();
                case CommandType.Parentheses:
                    return DeserializeParentheses();
                case CommandType.UnaryCommand:
                    return DeserializeUnary();
                case CommandType.BinaryCommand:
                    return DeserializeBinary();
                default:
                    throw new ArgumentException("Unknown command type");
            }
        }

        private DeserializedOperand DeserializeOperand()
        {
            bool isNegative = _dataReader.ReadBoolean();
            bool isDecimal = _dataReader.ReadBoolean();
            bool isSciFmt = _dataReader.ReadBoolean();

            var commands = new List<int>();
            uint cmdVectorSize = _dataReader.ReadUInt32();
            for (uint j = 0; j < cmdVectorSize; j++)
            {
                commands.Add(_dataReader.ReadInt32());
            }

            return new DeserializedOperand(isNegative, isDecimal, isSciFmt, commands);
        }

        private DeserializedParentheses DeserializeParentheses()
        {
            int cmd = _dataReader.ReadInt32();
            return new DeserializedParentheses(cmd);
        }

        private DeserializedUnary DeserializeUnary()
        {
            uint cmdSize = _dataReader.ReadUInt32();
            var commands = new List<int>();
            for (uint i = 0; i < cmdSize; i++)
            {
                commands.Add(_dataReader.ReadInt32());
            }
            return new DeserializedUnary(commands);
        }

        private DeserializedBinary DeserializeBinary()
        {
            int cmd = _dataReader.ReadInt32();
            return new DeserializedBinary(cmd);
        }
    }
}
