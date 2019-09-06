#pragma once

#include "Common.h"
#include "IGraph.h"
#include "GraphingEnums.h"

namespace Graphing
{
    struct IParsingOptions : public NonCopyable, public NonMoveable
    {
        virtual ~IParsingOptions() = default;

        virtual void SetFormatType(FormatType type) = 0;
    };

    struct IEvalOptions : public NonCopyable, public NonMoveable
    {
        virtual ~IEvalOptions() = default;

        virtual void SetTrigUnitMode(Graphing::EvalTrigUnitMode value) = 0;

    };

    struct IFormatOptions : public NonCopyable, public NonMoveable
    {
        virtual ~IFormatOptions() = default;

        virtual void SetFormatType(FormatType type) = 0;
    };

    struct IMathSolver : public NonCopyable, public NonMoveable
    {
        virtual ~IMathSolver() = default;

        static GRAPHINGAPI std::unique_ptr<IMathSolver> CreateMathSolver();

        virtual IParsingOptions& ParsingOptions() = 0;
        virtual IEvalOptions& EvalOptions() = 0;
        virtual IFormatOptions& FormatOptions() = 0;

        virtual std::unique_ptr<IExpression> ParseInput(const std::wstring& input) = 0;
        virtual std::shared_ptr<IGraph> CreateGrapher(const IExpression* expression) = 0;

        virtual std::shared_ptr<Graphing::IGraph> CreateGrapher() = 0;

        virtual std::wstring Serialize(const IExpression* expression) = 0;
    };
}
