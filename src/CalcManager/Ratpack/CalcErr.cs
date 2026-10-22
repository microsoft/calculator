// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalcManager.Ratpack
{
    // CalcErr.cs
    //
    // Defines the error codes thrown by ratpak and caught by Calculator
    //
    //  Ratpak errors are 32 bit values laid out as follows:
    //
    //   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
    //   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    //  +-+-------+---------------------+-------------------------------+
    //  |S|   R   |    Facility         |               Code            |
    //  +-+-------+---------------------+-------------------------------+
    //
    //  where
    //
    //      S - Severity - indicates success/fail
    //          0 - Success
    //          1 - Fail
    //
    //      R - Reserved - not currently used for anything
    //
    //      r - reserved portion of the facility code. Reserved for internal
    //              use. Used to indicate int32 values that are not status
    //              values, but are instead message ids for display strings.
    //
    //      Facility - is the facility code
    //      Code - is the actual error code
    //
    // This format is based loosely on an OLE HRESULT and is compatible with the
    // SUCCEEDED and FAILED macros as well as the HRESULT_CODE macro

    public static class CalcErr
    {
        public const int S_OK = 0;
        public const int E_FAIL = unchecked((int)0x80004005);
        public const int E_BOUNDS = unchecked((int)0x8000000B);
        public const int E_OUTOFMEMORY = unchecked((int)0x8007000E);
        public const int E_INVALIDARG = unchecked((int)0x80070057);

        // CALC_E_DIVIDEBYZERO
        // The current operation would require a divide by zero to complete
        public const uint CALC_E_DIVIDEBYZERO = 0x80000000;

        // CALC_E_DOMAIN
        // The given input is not within the domain of this function
        public const uint CALC_E_DOMAIN = 0x80000001;

        // CALC_E_INDEFINITE
        // The result of this function is undefined
        public const uint CALC_E_INDEFINITE = 0x80000002;

        public const uint CALC_E_INSUFFICIENT_DATA = 0x8000000B;

        public static uint SCODE_CODE(uint sc) => sc & 0xFFFF;

        // CALC_E_POSINFINITY
        // The result of this function is Positive Infinity.
        public const uint CALC_E_POSINFINITY = 0x80000003;

        // CALC_E_NEGINFINITY
        // The result of this function is Negative Infinity
        public const uint CALC_E_NEGINFINITY = 0x80000004;

        // CALC_E_INVALIDRANGE
        // The given input is within the domain of the function but is beyond
        // the range for which calc can successfully compute the answer
        public const uint CALC_E_INVALIDRANGE = 0x80000006;

        // CALC_E_OUTOFMEMORY
        // There is not enough free memory to complete the requested function
        public const uint CALC_E_OUTOFMEMORY = 0x80000007;

        // CALC_E_OVERFLOW
        // The result of this operation is an overflow
        public const uint CALC_E_OVERFLOW = 0x80000008;

        // CALC_E_NORESULT
        // The result of this operation is undefined
        public const uint CALC_E_NORESULT = 0x80000009;

        public static bool SUCCEEDED(int hr) => hr >= 0;
        public static bool FAILED(int hr) => hr < 0;
        public static bool SUCCEEDED(uint hr) => (hr & 0x80000000) == 0;
        public static bool FAILED(uint hr) => (hr & 0x80000000) != 0;
    }
}
