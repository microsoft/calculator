// Copyright (c) Microsoft Corporation. All rights reserved.

#pragma once

#include "Number.h"

namespace CalcEngine
{
    // Default Base/Radix to use for Rational calculations
    // RatPack calculations currently support up to Base64.
    inline constexpr uint32_t RATIONAL_BASE = 10;

    // Default Precision to use for Rational calculations
    static constexpr int32_t RATIONAL_PRECISION = 128;

    class Rational
    {
    public:
        Rational() noexcept;
        Rational(Number const& n) noexcept;
        Rational(Number const& p, Number const& q) noexcept;
        Rational(int32_t i);
        Rational(uint32_t ui);
        Rational(uint64_t ui, int32_t precision);

        explicit Rational(PRAT prat) noexcept;
        PRAT ToPRAT() const;

        Number const& P() const;
        Number const& Q() const;

        Rational operator-() const;
        Rational Add(Rational const& rhs, int32_t precision) const;
        Rational Sub(Rational const& rhs, int32_t precision) const;
        Rational Mul(Rational const& rhs, int32_t precision) const;
        Rational Div(Rational const& rhs, int32_t precision) const;
        Rational Mod(Rational const& rhs) const;

        Rational Lsh(Rational const& r, int32_t precision) const;
        Rational Rsh(Rational const& r, int32_t precision) const;

        Rational Not(Rational const& chopNum, int32_t precision) const;
        Rational And(Rational const& r, int32_t precision) const;
        Rational Or(Rational const& r, int32_t precision) const;
        Rational Xor(Rational const& r, int32_t precision) const;

        bool IsZero() const;
        bool IsLess(Rational const& r, int32_t precision) const;
        bool IsLessEq(Rational const& r, int32_t precision) const;
        bool IsGreaterEq(Rational const& r, int32_t precision) const;
        bool IsEq(Rational const& r, int32_t precision) const;

        std::wstring ToString(uint32_t radix, NUMOBJ_FMT format, int32_t precision) const;
        uint64_t ToUInt64_t(int32_t precision) const;

    private:
        Number m_p;
        Number m_q;
    };
}
