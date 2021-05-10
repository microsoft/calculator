// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common.h"
#include "IGraphAnalyzer.h"
#include "IGraphingOptions.h"
#include "IGraphRenderer.h"
#include "IEquation.h"
#include <optional>

namespace Graphing
{
    struct IGraph : public NonCopyable, public NonMoveable
    {
        virtual ~IGraph() = default;

        virtual std::optional<std::vector<std::shared_ptr<IEquation>>> TryInitialize(const IExpression* graphingExp = nullptr) = 0;

        virtual HRESULT GetInitializationError() = 0;

        virtual IGraphingOptions& GetOptions() = 0;

        virtual std::vector<std::shared_ptr<Graphing::IVariable>> GetVariables() = 0;

        virtual void SetArgValue(std::wstring variableName, double value) = 0;

        virtual std::shared_ptr<Renderer::IGraphRenderer> GetRenderer() const = 0;

        virtual bool TryResetSelection() = 0;

        virtual std::shared_ptr<Graphing::Analyzer::IGraphAnalyzer> GetAnalyzer() const = 0;
    };
}
