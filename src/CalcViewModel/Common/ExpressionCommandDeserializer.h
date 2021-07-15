// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/ExpressionCommand.h"

namespace CalculatorApp::ViewModel
{
    namespace Common
    {
        class CommandDeserializer
        {
        public:
            CommandDeserializer(_In_ Windows::Storage::Streams::DataReader ^ dataReader);
            std::shared_ptr<IExpressionCommand> Deserialize(_In_ CalculationManager::CommandType cmdType);

        private:
            Windows::Storage::Streams::DataReader ^ m_dataReader;
            COpndCommand DeserializeOperand();
            CParentheses DeserializeParentheses();
            CUnaryCommand DeserializeUnary();
            CBinaryCommand DeserializeBinary();
        };
    }
}
