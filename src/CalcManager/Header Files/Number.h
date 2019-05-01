// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <vector>
#include "Ratpack/ratpak.h"

namespace CalcEngine
{
    class Number
    {
    public:
        Number() noexcept;
        Number(int32_t sign, int32_t exp, std::vector<uint32_t> const& mantissa) noexcept;

        explicit Number(PNUMBER p) noexcept;
        PNUMBER ToPNUMBER() const;

        int32_t const& Sign() const;
        int32_t const& Exp() const;
        std::vector<uint32_t> const& Mantissa() const;

        bool IsZero() const;

    private:
        int32_t m_sign;
        int32_t m_exp;
        std::vector<uint32_t> m_mantissa;
    };
}
