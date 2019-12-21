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
    public:
        EvalOptions()
            : m_unit(Graphing::EvalTrigUnitMode::Invalid)
        {
        }

        Graphing::EvalTrigUnitMode GetTrigUnitMode() const override
        {
            return m_unit;
        }
        void SetTrigUnitMode(Graphing::EvalTrigUnitMode value) override
        {
            m_unit = value;
        }

    private:
        Graphing::EvalTrigUnitMode m_unit;
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
        MathSolver()
        {
        }

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

        std::shared_ptr<Graphing::IGraph> CreateGrapher(const Graphing::IExpression* expression) override;

        std::shared_ptr<Graphing::IGraph> CreateGrapher() override;

        std::wstring Serialize(const Graphing::IExpression* expression) override
        {
            return L"";
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
