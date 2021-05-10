// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common.h"
#include "IGraph.h"
#include "IGraphAnalyzer.h"
#include "GraphingEnums.h"

namespace Graphing
{
    struct IGraphFunctionAnalysisData
    {
        std::wstring Domain;
        std::wstring Range;
        int Parity;
        int PeriodicityDirection;
        std::wstring PeriodicityExpression;
        std::wstring Zeros;
        std::wstring YIntercept;
        std::vector<std::wstring> Minima;
        std::vector<std::wstring> Maxima;
        std::vector<std::wstring> InflectionPoints;
        std::vector<std::wstring> VerticalAsymptotes;
        std::vector<std::wstring> HorizontalAsymptotes;
        std::vector<std::wstring> ObliqueAsymptotes;
        std::map<std::wstring, int> MonotoneIntervals;
        int TooComplexFeatures; // to-do: refactor to remove bitwise flag
    };

    struct IParsingOptions : public NonCopyable, public NonMoveable
    {
        virtual ~IParsingOptions() = default;

        virtual void SetFormatType(FormatType type) = 0;
        virtual void SetLocalizationType(LocalizationType value) = 0;
    };

    struct IEvalOptions : public NonCopyable, public NonMoveable
    {
        virtual ~IEvalOptions() = default;

        virtual EvalTrigUnitMode GetTrigUnitMode() const = 0;
        virtual void SetTrigUnitMode(EvalTrigUnitMode value) = 0;
    };

    struct IFormatOptions : public NonCopyable, public NonMoveable
    {
        virtual ~IFormatOptions() = default;

        virtual void SetFormatType(FormatType type) = 0;
        virtual void SetMathMLPrefix(const std::wstring& value) = 0;
        virtual void SetLocalizationType(LocalizationType value) = 0;
    };

    struct IMathSolver : public NonCopyable, public NonMoveable
    {
        virtual ~IMathSolver() = default;

        static GRAPHINGAPI std::unique_ptr<IMathSolver> CreateMathSolver();

        virtual IParsingOptions& ParsingOptions() = 0;
        virtual IEvalOptions& EvalOptions() = 0;
        virtual IFormatOptions& FormatOptions() = 0;

        virtual std::unique_ptr<IExpression> ParseInput(const std::wstring& input, int& errorCodeOut, int& errorTypeOut) = 0;

        virtual void HRErrorToErrorInfo(HRESULT hr, int& errorCodeOut, int& errorTypeOut) = 0;

        virtual std::shared_ptr<IGraph> CreateGrapher(const IExpression* expression) = 0;
        virtual std::shared_ptr<Graphing::IGraph> CreateGrapher() = 0;

        virtual std::wstring Serialize(const IExpression* expression) = 0;

        virtual Graphing::IGraphFunctionAnalysisData Analyze(const Graphing::Analyzer::IGraphAnalyzer* analyzer) = 0;
    };
}
