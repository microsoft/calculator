#pragma once

namespace CalculatorApp::ViewModel
{
    namespace Common
    {
    public
        enum class BitLength : int
        {
            BitLengthUnknown = -1,
            BitLengthByte = 8,
            BitLengthWord = 16,
            BitLengthDWord = 32,
            BitLengthQWord = 64,
        };
    }
}
