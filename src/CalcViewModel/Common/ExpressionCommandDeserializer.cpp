// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ExpressionCommandDeserializer.h"

using namespace CalculatorApp::ViewModel::Common;
using namespace Windows::Storage::Streams;

CommandDeserializer::CommandDeserializer(_In_ DataReader ^ dataReader)
    : m_dataReader(dataReader)
{
}

std::shared_ptr<IExpressionCommand> CommandDeserializer::Deserialize(_In_ CalculationManager::CommandType cmdType)
{
    switch (cmdType)
    {
    case CalculationManager::CommandType::OperandCommand:
        return std::make_shared<COpndCommand>(DeserializeOperand());

    case CalculationManager::CommandType::Parentheses:
        return std::make_shared<CParentheses>(DeserializeParentheses());

    case CalculationManager::CommandType::UnaryCommand:
        return std::make_shared<CUnaryCommand>(DeserializeUnary());

    case CalculationManager::CommandType::BinaryCommand:
        return std::make_shared<CBinaryCommand>(DeserializeBinary());

    default:
        throw ref new Platform::Exception(E_INVALIDARG, ref new Platform::String(L"Unknown command type"));
    }
}

COpndCommand CommandDeserializer::DeserializeOperand()
{
    bool fNegative = m_dataReader->ReadBoolean();
    bool fDecimal = m_dataReader->ReadBoolean();
    bool fSciFmt = m_dataReader->ReadBoolean();

    std::shared_ptr<std::vector<int>> cmdVector = std::make_shared<std::vector<int>>();
    auto cmdVectorSize = m_dataReader->ReadUInt32();

    for (unsigned int j = 0; j < cmdVectorSize; ++j)
    {
        int eachOpndcmd = m_dataReader->ReadInt32();
        cmdVector->push_back(eachOpndcmd);
    }

    return COpndCommand(cmdVector, fNegative, fDecimal, fSciFmt);
}

CParentheses CommandDeserializer::DeserializeParentheses()
{
    int parenthesisCmd = m_dataReader->ReadInt32();
    return CParentheses(parenthesisCmd);
}

CUnaryCommand CommandDeserializer::DeserializeUnary()
{
    auto cmdSize = m_dataReader->ReadUInt32();

    if (cmdSize == 1)
    {
        int eachOpndcmd = m_dataReader->ReadInt32();
        return CUnaryCommand(eachOpndcmd);
    }
    else
    {
        int eachOpndcmd1 = m_dataReader->ReadInt32();
        int eachOpndcmd2 = m_dataReader->ReadInt32();
        return CUnaryCommand(eachOpndcmd1, eachOpndcmd2);
    }
}

CBinaryCommand CommandDeserializer::DeserializeBinary()
{
    int cmd = m_dataReader->ReadInt32();
    return CBinaryCommand(cmd);
}
