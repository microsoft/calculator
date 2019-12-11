// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common.h"
#include "IEquationOptions.h"

namespace Graphing
{
    struct IEquation : public NonCopyable, public NonMoveable
    {
        virtual ~IEquation() = default;

        virtual std::shared_ptr<IEquationOptions> GetGraphEquationOptions() const = 0;

        virtual unsigned int GetGraphEquationID() const = 0;

        virtual bool TrySelectEquation() = 0;
        virtual bool IsEquationSelected() const = 0;
    };
}