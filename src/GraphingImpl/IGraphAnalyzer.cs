// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;

namespace Graphing.Analyzer
{
    public interface IGraphAnalyzer
    {
        bool CanFunctionAnalysisBePerformed(out bool variableIsNotX);
        int PerformFunctionAnalysis(uint analysisType);
        int GetAnalysisTypeCaption(AnalysisType type, out string captionOut);
        int GetMessage(GraphAnalyzerMessage msg, out string msgOut);
    }
}

namespace Graphing
{
    public struct IGraphFunctionAnalysisData
    {
        public string Domain { get; set; }
        public string Range { get; set; }
        public int Parity { get; set; }
        public int PeriodicityDirection { get; set; }
        public string PeriodicityExpression { get; set; }
        public string Zeros { get; set; }
        public string YIntercept { get; set; }
        public IList<string> Minima { get; set; }
        public IList<string> Maxima { get; set; }
        public IList<string> InflectionPoints { get; set; }
        public IList<string> VerticalAsymptotes { get; set; }
        public IList<string> HorizontalAsymptotes { get; set; }
        public IList<string> ObliqueAsymptotes { get; set; }
        public IDictionary<string, int> MonotoneIntervals { get; set; }
        public int TooComplexFeatures { get; set; }
    }
}
