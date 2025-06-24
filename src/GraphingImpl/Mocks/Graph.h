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
        virtual std::optional<std::vector<std::shared_ptr<Graphing::IEquation>>> TryInitialize(const Graphing::IExpression* graphingExp = nullptr)
        {
            if (graphingExp != nullptr)
            {
                std::vector<std::shared_ptr<Graphing::IEquation>> equations;
                equations.push_back(nullptr);
                m_variables.push_back(std::make_shared<MockVariable>(MockVariable{}));
                return std::optional<std::vector<std::shared_ptr<Graphing::IEquation>>>(equations);
            }

            return std::nullopt;
        }

        HRESULT GetInitializationError() const
        {
            return S_OK;
        }

        virtual Graphing::IGraphingOptions& GetOptions()
        {
            return m_graphingOptions;
        }

        virtual std::vector<std::shared_ptr<Graphing::IVariable>> GetVariables()
        {
            return m_variables;
        }

        virtual void SetArgValue(std::wstring /*variableName*/, double /*value*/)
        {
        }

        virtual std::shared_ptr<Graphing::Renderer::IGraphRenderer> GetRenderer() const
        {
            return m_graphRenderer;
        }

        virtual bool TryResetSelection()
        {
            return true;
        }

        virtual std::shared_ptr<Graphing::Analyzer::IGraphAnalyzer> GetAnalyzer() const
        {
            return nullptr;
        }

    private:
        std::vector<std::shared_ptr<Graphing::IVariable>> m_variables;
        GraphingOptions m_graphingOptions;
        std::shared_ptr<Graphing::Renderer::IGraphRenderer> m_graphRenderer;
    };
}
