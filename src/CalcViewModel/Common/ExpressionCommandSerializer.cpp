// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Common/ExpressionCommandSerializer.h"

using namespace CalculatorApp::Common;
using namespace Windows::Storage::Streams;

SerializeCommandVisitor::SerializeCommandVisitor(_In_ DataWriter^ dataWriter) :m_dataWriter(dataWriter)
{
}

void SerializeCommandVisitor::Visit(_In_ COpndCommand &opndCmd)
{
    m_dataWriter->WriteBoolean(opndCmd.IsNegative());
    m_dataWriter->WriteBoolean(opndCmd.IsDecimalPresent());
    m_dataWriter->WriteBoolean(opndCmd.IsSciFmt());

    auto opndCmds = opndCmd.GetCommands();
    unsigned int opndCmdSize;
    opndCmds->GetSize(&opndCmdSize);
    m_dataWriter->WriteUInt32(opndCmdSize);
    for (unsigned int j = 0; j < opndCmdSize; ++j)
    {
        int eachOpndcmd;
        opndCmds->GetAt(j, &eachOpndcmd);
        m_dataWriter->WriteInt32(eachOpndcmd);
    }
}

void SerializeCommandVisitor::Visit(_In_ CUnaryCommand &unaryCmd)
{
    auto cmds = unaryCmd.GetCommands();
    unsigned int cmdSize;
    cmds->GetSize(&cmdSize);
    m_dataWriter->WriteUInt32(cmdSize);
    for (unsigned int j = 0; j < cmdSize; ++j)
    {
        int eachOpndcmd;
        cmds->GetAt(j, &eachOpndcmd);
        m_dataWriter->WriteInt32(eachOpndcmd);
    }
}

void SerializeCommandVisitor::Visit(_In_ CBinaryCommand &binaryCmd)
{
    int cmd = binaryCmd.GetCommand();
    m_dataWriter->WriteInt32(cmd);
}

void SerializeCommandVisitor::Visit(_In_ CParentheses &paraCmd)
{
    int parenthesisCmd = paraCmd.GetCommand();
    m_dataWriter->WriteInt32(parenthesisCmd);
}
