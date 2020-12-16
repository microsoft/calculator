#pragma once

namespace CalculatorApp
{
    enum KeyGraphFeaturesFlag
    {
        Domain = 1,
        Range = 2,
        Parity = 4,
        Periodicity = 8,
        Zeros = 16,
        YIntercept = 32,
        Minima = 64,
        Maxima = 128,
        InflectionPoints = 256,
        VerticalAsymptotes = 512,
        HorizontalAsymptotes = 1024,
        ObliqueAsymptotes = 2048,
        MonotoneIntervals = 4096
    };

    enum AnalysisErrorType
    {
        NoError,
        AnalysisCouldNotBePerformed,
        AnalysisNotSupported,
        VariableIsNotX
    };
}
