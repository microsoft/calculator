// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "GraphingInterfaces/IBitmap.h"
#include <WinDef.h>
namespace MockGraphingImpl
{
    class Bitmap : public Graphing::IBitmap
    {
        const std::vector<BYTE>& GetData() const override
        {
            return std::vector<BYTE>();
        }
    };
}
