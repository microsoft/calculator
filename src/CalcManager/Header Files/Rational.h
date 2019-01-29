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
        Rational(int32_t i) noexcept;
        Rational(uint32_t ui) noexcept;
        Rational(uint64_t ui, uint32_t radix, int32_t precision) noexcept;

        explicit Rational(PRAT prat) noexcept;
        PRAT ToPRAT() const;

        Number const& P() const;
        Number const& Q() const;

        bool IsZero() const;
        bool IsLess(Rational const& r, int32_t precision) const;
        bool IsLessEq(Rational const& r, int32_t precision) const;
        bool IsGreaterEq(Rational const& r, int32_t precision) const;
        bool IsEq(Rational const& r, int32_t precision) const;

        std::wstring ToString(uint32_t radix, NUMOBJ_FMT fmt, int32_t precision) const;
        uint64_t ToUInt64_t(uint32_t radix, int32_t precision) const;

    private:
        Number m_p;
        Number m_q;
    };
}
