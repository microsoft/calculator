// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/ExpressionCommand.h"

namespace CalculatorApp::ViewModel
{
    namespace Common
    {
        class SerializeCommandVisitor : public ISerializeCommandVisitor
        {
        public:
            SerializeCommandVisitor(_In_ Windows::Storage::Streams::DataWriter ^ dataWriter);

            void Visit(_In_ COpndCommand& opndCmd);
            void Visit(_In_ CUnaryCommand& unaryCmd);
            void Visit(_In_ CBinaryCommand& binaryCmd);
            void Visit(_In_ CParentheses& paraCmd);

        private:
            Windows::Storage::Streams::DataWriter ^ m_dataWriter;
        };
    }
}
