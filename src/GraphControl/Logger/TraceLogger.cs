// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.Foundation.Diagnostics;
using TraceLogging;

namespace GraphControl
{
    public sealed class TraceLogger
    {
        private static readonly TraceLogger s_selfInstance = new TraceLogger();

        private const string GRAPHING_MODE = "Graphing";
        private const string CALC_MODE = "CalcMode";

        private const string EVENT_NAME_EQUATION_COUNT_CHANGED = "EquationCountChanged";
        private const string EVENT_NAME_FUNCTION_ANALYSIS_PERFORMED = "FunctionAnalysisPerformed";
        private const string EVENT_NAME_VARIABLES_COUNT_CHANGED = "VariablesCountChanged";
        private const string EVENT_NAME_LINE_WIDTH_CHANGED = "LineWidthChanged";

        private bool _firstRun = true;
        private ulong _totalValidEquations;
        private ulong _totalInvalidEquations;
        private ulong _previousValidEquations;
        private ulong _previousInvalidEquations;

        private TraceLogger()
        {
        }

        public static TraceLogger GetInstance() => s_selfInstance;

        public void LogEquationCountChanged(int currentValidEquations, int currentInvalidEquations)
        {
            if (_firstRun)
            {
                _firstRun = false;
                return;
            }

            if ((ulong)currentValidEquations > _previousValidEquations)
            {
                _totalValidEquations++;
            }
            else if ((ulong)currentInvalidEquations > _previousInvalidEquations)
            {
                _totalInvalidEquations++;
            }

            _previousValidEquations = (ulong)currentValidEquations;
            _previousInvalidEquations = (ulong)currentInvalidEquations;

            var fields = new LoggingFields();
            fields.AddString(CALC_MODE, GRAPHING_MODE);
            fields.AddUInt64("ConcurrentValidFunctions", (ulong)currentValidEquations);
            fields.AddUInt64("ConcurrentInvalidFunctions", (ulong)currentInvalidEquations);
            fields.AddUInt64("TotalValidFunctions", _totalValidEquations);
            fields.AddUInt64("TotalInvalidFunctions", _totalInvalidEquations);
            TraceLoggingCommon.GetInstance()?.LogLevel2Event(EVENT_NAME_EQUATION_COUNT_CHANGED, fields);
        }

        public void LogFunctionAnalysisPerformed(int analysisErrorType, uint tooComplexFlag)
        {
            var fields = new LoggingFields();
            fields.AddString(CALC_MODE, GRAPHING_MODE);
            fields.AddInt32("AnalysisErrorType", analysisErrorType);
            fields.AddUInt32("TooComplexFeatures", tooComplexFlag);
            TraceLoggingCommon.GetInstance()?.LogLevel2Event(EVENT_NAME_FUNCTION_ANALYSIS_PERFORMED, fields);
        }

        public void LogVariableCountChanged(int variablesCount)
        {
            var fields = new LoggingFields();
            fields.AddString(CALC_MODE, GRAPHING_MODE);
            fields.AddInt64("VariableCount", variablesCount);
            TraceLoggingCommon.GetInstance()?.LogLevel2Event(EVENT_NAME_VARIABLES_COUNT_CHANGED, fields);
        }

        public void LogLineWidthChanged()
        {
            var fields = new LoggingFields();
            fields.AddString(CALC_MODE, GRAPHING_MODE);
            TraceLoggingCommon.GetInstance()?.LogLevel2Event(EVENT_NAME_LINE_WIDTH_CHANGED, fields);
        }
    }
}
