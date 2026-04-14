// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using Windows.Storage.Streams;

namespace CalculatorApp.ViewModel.Common
{
    /// <summary>
    /// Serializes expression commands (operand, unary, binary, parentheses) to a DataWriter.
    /// This is the C# port of the C++ SerializeCommandVisitor.
    /// </summary>
    public class ExpressionCommandSerializer
    {
        private readonly DataWriter _dataWriter;

        public ExpressionCommandSerializer(DataWriter dataWriter)
        {
            _dataWriter = dataWriter;
        }

        public void SerializeOperand(bool isNegative, bool isDecimalPresent, bool isSciFmt, IList<int> commands)
        {
            _dataWriter.WriteBoolean(isNegative);
            _dataWriter.WriteBoolean(isDecimalPresent);
            _dataWriter.WriteBoolean(isSciFmt);

            _dataWriter.WriteUInt32((uint)commands.Count);
            foreach (int cmd in commands)
            {
                _dataWriter.WriteInt32(cmd);
            }
        }

        public void SerializeUnary(IList<int> commands)
        {
            _dataWriter.WriteUInt32((uint)commands.Count);
            foreach (int cmd in commands)
            {
                _dataWriter.WriteInt32(cmd);
            }
        }

        public void SerializeBinary(int command)
        {
            _dataWriter.WriteInt32(command);
        }

        public void SerializeParentheses(int command)
        {
            _dataWriter.WriteInt32(command);
        }
    }
}
