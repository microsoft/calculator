// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <vector>

namespace Graphing
{
    struct IBitmap
    {
        virtual const std::vector<BYTE>& GetData() const = 0;
    };
}
