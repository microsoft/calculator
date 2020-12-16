// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "Utils.h"

namespace GraphControl
{
public
    ref class Variable sealed
    {
    public:
        PROPERTY_RW(double, Value);
        PROPERTY_RW(double, Step);
        PROPERTY_RW(double, Min);
        PROPERTY_RW(double, Max);

        Variable(double value)
            : m_Value{ value }
            , m_Step{ 0.1 }
            , m_Min{ -5.0 }
            , m_Max{ 5.0 }
        {
        }
    };
}
