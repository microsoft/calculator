// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Collections.ObjectModel;
using CalculatorApp;

namespace GraphControl
{
    public sealed class KeyGraphFeaturesInfo
    {
        public string XIntercept { get; set; }
        public string YIntercept { get; set; }
        public int Parity { get; set; }
        public int PeriodicityDirection { get; set; }
        public string PeriodicityExpression { get; set; }
        public IList<string> Minima { get; set; }
        public IList<string> Maxima { get; set; }
        public string Domain { get; set; }
        public string Range { get; set; }
        public IList<string> InflectionPoints { get; set; }
        public IDictionary<string, string> Monotonicity { get; set; }
        public IList<string> VerticalAsymptotes { get; set; }
        public IList<string> HorizontalAsymptotes { get; set; }
        public IList<string> ObliqueAsymptotes { get; set; }
        public int TooComplexFeatures { get; set; }
        public int AnalysisError { get; set; }

        public KeyGraphFeaturesInfo()
        {
            Minima = new ObservableCollection<string>();
            Maxima = new ObservableCollection<string>();
            InflectionPoints = new ObservableCollection<string>();
            Monotonicity = new Dictionary<string, string>();
            VerticalAsymptotes = new ObservableCollection<string>();
            HorizontalAsymptotes = new ObservableCollection<string>();
            ObliqueAsymptotes = new ObservableCollection<string>();
        }

        public static KeyGraphFeaturesInfo Create(AnalysisErrorType type)
        {
            var res = new KeyGraphFeaturesInfo();
            res.AnalysisError = (int)type;
            TraceLogger.GetInstance().LogFunctionAnalysisPerformed((int)type, 0);
            return res;
        }
    }
}
