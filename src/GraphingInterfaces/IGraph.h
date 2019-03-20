#pragma once

#include "Common.h"
#include "IGraphingOptions.h"
#include "IGraphRenderer.h"

namespace Graphing
{
    struct IGraph : public NonCopyable, public NonMoveable
    {
        virtual ~IGraph() = default;

        virtual bool TryInitialize(const IExpression* graphingExp) = 0;

        virtual IGraphingOptions& GetOptions() = 0;

        virtual std::shared_ptr< Renderer::IGraphRenderer > GetRenderer() const = 0;
    };
}
