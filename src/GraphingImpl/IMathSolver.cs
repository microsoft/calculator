// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Graphing.Analyzer;

namespace Graphing
{
    public interface IParsingOptions
    {
        void SetFormatType(FormatType type);
        void SetLocalizationType(LocalizationType value);
    }

    public interface IEvalOptions
    {
        EvalTrigUnitMode GetTrigUnitMode();
        void SetTrigUnitMode(EvalTrigUnitMode value);
    }

    public interface IFormatOptions
    {
        void SetFormatType(FormatType type);
        void SetMathMLPrefix(string value);
        void SetLocalizationType(LocalizationType value);
    }

    public interface IMathSolver
    {
        IParsingOptions ParsingOptions();
        IEvalOptions EvalOptions();
        IFormatOptions FormatOptions();

        IExpression ParseInput(string input, out int errorCodeOut, out int errorTypeOut);
        void HRErrorToErrorInfo(int hr, out int errorCodeOut, out int errorTypeOut);

        IGraph CreateGrapher(IExpression expression = null);
        string Serialize(IExpression expression);
        IGraphFunctionAnalysisData Analyze(IGraphAnalyzer analyzer);
    }
}
