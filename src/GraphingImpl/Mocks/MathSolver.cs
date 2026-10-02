// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using Graphing;
using Graphing.Analyzer;

namespace MockGraphingImpl
{
    public class ParsingOptions : IParsingOptions
    {
        public void SetFormatType(FormatType type) { }
        public void SetLocalizationType(LocalizationType value) { }
    }

    public class EvalOptions : IEvalOptions
    {
        private EvalTrigUnitMode _unit = EvalTrigUnitMode.Invalid;

        public EvalTrigUnitMode GetTrigUnitMode() => _unit;
        public void SetTrigUnitMode(EvalTrigUnitMode value) => _unit = value;
    }

    public class FormatOptions : IFormatOptions
    {
        public void SetFormatType(FormatType type) { }
        public void SetMathMLPrefix(string value) { }
        public void SetLocalizationType(LocalizationType value) { }
    }

    public class MockExpression : IExpression
    {
        public uint GetExpressionID() => 0;
        public bool IsEmptySet() => false;
    }

    public class MockVariable : IVariable
    {
        private const string VarName = "m";
        public int GetVariableID() => 0;
        public string GetVariableName() => VarName;
    }

    public class MathSolver : IMathSolver
    {
        private readonly ParsingOptions _parsingOptions = new ParsingOptions();
        private readonly EvalOptions _evalOptions = new EvalOptions();
        private readonly FormatOptions _formatOptions = new FormatOptions();

        public static IMathSolver CreateMathSolver() => new MathSolver();

        public IParsingOptions ParsingOptions() => _parsingOptions;
        public IEvalOptions EvalOptions() => _evalOptions;
        public IFormatOptions FormatOptions() => _formatOptions;

        public IExpression ParseInput(string input, out int errorCodeOut, out int errorTypeOut)
        {
            errorCodeOut = 0;
            errorTypeOut = 0;
            if (string.IsNullOrEmpty(input))
            {
                return null;
            }
            return new MockExpression();
        }

        public void HRErrorToErrorInfo(int hr, out int errorCodeOut, out int errorTypeOut)
        {
            errorCodeOut = 0;
            errorTypeOut = 0;
        }

        public IGraph CreateGrapher(IExpression expression = null)
        {
            return new Graph();
        }

        public string Serialize(IExpression expression)
        {
            return string.Empty;
        }

        public IGraphFunctionAnalysisData Analyze(IGraphAnalyzer analyzer)
        {
            return new IGraphFunctionAnalysisData
            {
                Minima = new List<string>(),
                Maxima = new List<string>(),
                InflectionPoints = new List<string>(),
                VerticalAsymptotes = new List<string>(),
                HorizontalAsymptotes = new List<string>(),
                ObliqueAsymptotes = new List<string>(),
                MonotoneIntervals = new Dictionary<string, int>()
            };
        }
    }
}
