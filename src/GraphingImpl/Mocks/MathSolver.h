// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "GraphingInterfaces/IMathSolver.h"

namespace MockGraphingImpl
{
    class ParsingOptions : public Graphing::IParsingOptions
    {
    public:
        void SetFormatType(Graphing::FormatType type) override
        {
        }
    };

    class EvalOptions : public Graphing::IEvalOptions
    {
        Graphing::EvalTrigUnitMode GetTrigUnitMode() const override
        {
            return Graphing::EvalTrigUnitMode::Invalid;
        }
        void SetTrigUnitMode(Graphing::EvalTrigUnitMode value) override
        {
        }
    };

    class FormatOptions : public Graphing::IFormatOptions
    {
    public:
        void SetFormatType(Graphing::FormatType type) override
        {
        }

        void SetMathMLPrefix(const std::wstring& value) override
        {
        }
    };

    class MathSolver : public Graphing::IMathSolver
    {
    public:
        Graphing::IParsingOptions& ParsingOptions() override
        {
            return m_parsingOptions;
        }

        Graphing::IEvalOptions& EvalOptions() override
        {
            return m_evalOptions;
        }

        Graphing::IFormatOptions& FormatOptions() override
        {
            return m_formatOptions;
        }

        std::unique_ptr<Graphing::IExpression> ParseInput(const std::wstring& input) override
        {
            return nullptr;
        }

        std::shared_ptr<Graphing::IGraph> CreateGrapher(const Graphing::IExpression* expression) override
        {
            return nullptr;
        }

        std::shared_ptr<Graphing::IGraph> CreateGrapher() override
        {
            return nullptr;
        }

        std::wstring Serialize(const Graphing::IExpression* expression) override
        {
            return std::wstring{};
        }

        Graphing::IGraphFunctionAnalysisData IMathSolver::Analyze(const Graphing::Analyzer::IGraphAnalyzer* analyzer)
        {
            return Graphing::IGraphFunctionAnalysisData{};
        }

    private:
        MockGraphingImpl::ParsingOptions m_parsingOptions;
        MockGraphingImpl::EvalOptions m_evalOptions;
        MockGraphingImpl::FormatOptions m_formatOptions;
    };
}
