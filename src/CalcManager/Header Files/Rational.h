// Copyright (c) Microsoft Corporation. All rights reserved.

#pragma once

#include "Number.h"

namespace CalcEngine
{
    class Rational
    {
    public:
        Rational() noexcept;
        Rational(Number const& n) noexcept;
        Rational(Number const& p, Number const& q) noexcept;

        explicit Rational(PRAT prat) noexcept;
        PRAT ToPRAT() const;

        Number const& P() const;
        Number const& Q() const;

        bool IsZero() const;

    private:
        Number m_p;
        Number m_q;
    };
}
