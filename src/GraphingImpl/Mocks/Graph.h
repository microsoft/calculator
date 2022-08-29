// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "GraphingInterfaces/IGraph.h"
#include "GraphingOptions.h"
#include "Mocks/GraphRenderer.h"

namespace MockGraphingImpl
{
    class Graph : public Graphing::IGraph
    {
    public:
        Graph()
        {
            m_graphRenderer = std::make_shared<GraphRenderer>();
        }

        std::optional<std::vector<std::shared_ptr<Graphing::IEquation>>> TryInitialize(const Graphing::IExpression* graphingExp = nullptr) override
        {
            if (graphingExp != nullptr)
            {
                std::vector<std::shared_ptr<Graphing::IEquation>> equations;
                equations.push_back(nullptr);
                m_variables.push_back(std::make_shared<MockVariable>(MockVariable{}));
                return std::optional(equations);
            }

            return std::nullopt;
        }

        HRESULT GetInitializationError() override
        {
            return S_OK;
        }

        Graphing::IGraphingOptions& GetOptions() override
        {
            return m_graphingOptions;
        }

        std::vector<std::shared_ptr<Graphing::IVariable>> GetVariables() override
        {
            return m_variables;
        }

        void SetArgValue(std::wstring variableName, double value) override
        {
        }

        std::shared_ptr<Graphing::Renderer::IGraphRenderer> GetRenderer() const override
        {
            return m_graphRenderer;
        }

        bool TryResetSelection() override
        {
            return true;
        }

        std::shared_ptr<Graphing::Analyzer::IGraphAnalyzer> GetAnalyzer() const override
        {
            return nullptr;
        }

    private:
        std::vector<std::shared_ptr<Graphing::IVariable>> m_variables;
        GraphingOptions m_graphingOptions;
        std::shared_ptr<Graphing::Renderer::IGraphRenderer> m_graphRenderer;
    };
}
