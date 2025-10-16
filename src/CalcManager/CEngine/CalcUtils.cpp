// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Header Files/CalcEngine.h"
#include "Header Files/CalcUtils.h"

bool IsOpInRange(OpCode op, uint32_t x, uint32_t y)
{
    return ((op >= x) && (op <= y));
}

bool IsBinOpCode(OpCode opCode)
{
    return IsOpInRange(opCode, IDC_AND, IDC_PWR) || IsOpInRange(opCode, IDC_BINARYEXTENDEDFIRST, IDC_BINARYEXTENDEDLAST);
}

// WARNING: IDC_SIGN is a special unary op but still this doesn't catch this. Caller has to be aware
// of it and catch it themselves or not needing this
bool IsUnaryOpCode(OpCode opCode)
{
    return (IsOpInRange(opCode, IDC_UNARYFIRST, IDC_UNARYLAST) || IsOpInRange(opCode, IDC_UNARYEXTENDEDFIRST, IDC_UNARYEXTENDEDLAST));
}

bool IsDigitOpCode(OpCode opCode)
{
    return IsOpInRange(opCode, IDC_0, IDC_F);
}

// Some commands are not affecting the state machine state of the calc flow. But these are more of
// some gui mode kind of settings (eg Inv button, or Deg,Rad , Back etc.). This list is getting bigger & bigger
// so we abstract this as a separate routine. Note: There is another side to this. Some commands are not
// gui mode setting to begin with, but once it is discovered it is invalid and we want to behave as though it
// was never inout, we need to revert the state changes made as a result of this test
bool IsGuiSettingOpCode(OpCode opCode)
{
    if (IsOpInRange(opCode, IDM_HEX, IDM_BIN) || IsOpInRange(opCode, IDM_QWORD, IDM_BYTE) || IsOpInRange(opCode, IDM_DEG, IDM_GRAD))
    {
        return true;
    }

    switch (opCode)
    {
    case IDC_INV:
    case IDC_FE:
    case IDC_MCLEAR:
    case IDC_BACK:
    case IDC_STORE:
    case IDC_MPLUS:
    case IDC_MMINUS:
        return true;
    }

    // most of the commands
    return false;
}
