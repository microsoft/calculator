// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common.h"

// A trace logging provider can only be instantiated and registered once per module.
// This class implements a singleton model ensure that only one instance is created.
namespace GraphControl
{

    public ref class TraceLogger sealed
    {
    internal:
        static TraceLogger ^ GetInstance();

        void LogEquationCountChanged(int currentValidEquations, int currentInvalidEquations);
        void LogFunctionAnalysisPerformed(int analysisErrorType, uint32 tooComplexFlag);
        void LogVariableCountChanged(int variablesCount);
        void LogLineWidthChanged();

    private:
        TraceLogger()
        {
        }
    };
}
