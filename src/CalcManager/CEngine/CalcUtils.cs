// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalcManager.CEngine
{
    public static class CalcUtils
    {
        public static bool IsOpInRange(uint op, uint x, uint y)
        {
            return (op >= x) && (op <= y);
        }

        public static bool IsBinOpCode(uint opCode)
        {
            return IsOpInRange(opCode, CCommand.IDC_AND, CCommand.IDC_PWR) ||
                   IsOpInRange(opCode, CCommand.IDC_BINARYEXTENDEDFIRST, CCommand.IDC_BINARYEXTENDEDLAST);
        }

        // WARNING: IDC_SIGN is a special unary op but still this doesn't catch this. Caller has to be aware
        // of it and catch it themselves or not needing this
        public static bool IsUnaryOpCode(uint opCode)
        {
            return IsOpInRange(opCode, CCommand.IDC_UNARYFIRST, CCommand.IDC_UNARYLAST) ||
                   IsOpInRange(opCode, CCommand.IDC_UNARYEXTENDEDFIRST, CCommand.IDC_UNARYEXTENDEDLAST);
        }

        public static bool IsDigitOpCode(uint opCode)
        {
            return IsOpInRange(opCode, CCommand.IDC_0, CCommand.IDC_F);
        }

        // Some commands are not affecting the state machine state of the calc flow. But these are more of
        // some gui mode kind of settings (eg Inv button, or Deg,Rad , Back etc.). This list is getting bigger & bigger
        // so we abstract this as a separate routine. Note: There is another side to this. Some commands are not
        // gui mode setting to begin with, but once it is discovered it is invalid and we want to behave as though it
        // was never inout, we need to revert the state changes made as a result of this test
        public static bool IsGuiSettingOpCode(uint opCode)
        {
            if (IsOpInRange(opCode, CCommand.IDM_HEX, CCommand.IDM_BIN) ||
                IsOpInRange(opCode, CCommand.IDM_QWORD, CCommand.IDM_BYTE) ||
                IsOpInRange(opCode, CCommand.IDM_DEG, CCommand.IDM_GRAD))
            {
                return true;
            }

            switch (opCode)
            {
                case CCommand.IDC_INV:
                case CCommand.IDC_FE:
                case CCommand.IDC_MCLEAR:
                case CCommand.IDC_BACK:
                case CCommand.IDC_EXP:
                case CCommand.IDC_STORE:
                case CCommand.IDC_MPLUS:
                case CCommand.IDC_MMINUS:
                    return true;
            }

            return false;
        }
    }
}
