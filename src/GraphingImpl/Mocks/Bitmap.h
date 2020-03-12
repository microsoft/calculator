// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "GraphingInterfaces/IBitmap.h"
#include <WinDef.h>
namespace MockGraphingImpl
{
    class Bitmap : public Graphing::IBitmap
    {
        virtual const std::vector<BYTE>& GetData() const
        {
            return std::vector<BYTE>();
        }
    };
}
