#pragma once

#include "Common.h"
#include "IGraphingOptions.h"
#include "IGraphRenderer.h"
#include "IEquation.h"

namespace Graphing
{
    struct IGraph : public NonCopyable, public NonMoveable
    {
        virtual ~IGraph() = default;

        virtual std::optional<std::vector<std::shared_ptr<IEquation>>> TryInitialize(const IExpression* graphingExp) = 0;

        virtual IGraphingOptions& GetOptions() = 0;

        virtual std::shared_ptr< Renderer::IGraphRenderer > GetRenderer() const = 0;

        virtual bool TryResetSelection() = 0;
    };
}
