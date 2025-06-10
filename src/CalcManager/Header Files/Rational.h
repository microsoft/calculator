// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Number.h"

namespace CalcEngine
{
    // Default Base/Radix to use for Rational calculations
    // RatPack calculations currently support up to Base64.
    inline constexpr uint32_t RATIONAL_BASE = 10;

    // Default Precision to use for Rational calculations
    inline constexpr int32_t RATIONAL_PRECISION = 128;

    class Rational
    {
    public:
        Rational() noexcept;
        Rational(Number const& n) noexcept;
        Rational(Number const& p, Number const& q) noexcept;
        Rational(int32_t i);
        Rational(uint32_t ui);
        Rational(uint64_t ui);

        explicit Rational(PRAT prat) noexcept;
        PRAT ToPRAT() const;

        Number const& P() const;
        Number const& Q() const;

        bool IsZero() const;

        Rational operator-() const;
        Rational& operator+=(Rational const& rhs);
        Rational& operator-=(Rational const& rhs);
        Rational& operator*=(Rational const& rhs);
        Rational& operator/=(Rational const& rhs);
        Rational& operator%=(Rational const& rhs);

        Rational& operator<<=(Rational const& rhs);
        Rational& operator>>=(Rational const& rhs);

        Rational& operator&=(Rational const& rhs);
        Rational& operator|=(Rational const& rhs);
        Rational& operator^=(Rational const& rhs);

        friend Rational operator+(Rational lhs, Rational const& rhs);
        friend Rational operator-(Rational lhs, Rational const& rhs);
        friend Rational operator*(Rational lhs, Rational const& rhs);
        friend Rational operator/(Rational lhs, Rational const& rhs);
        friend Rational operator%(Rational lhs, Rational const& rhs);

        friend Rational operator<<(Rational lhs, Rational const& rhs);
        friend Rational operator>>(Rational lhs, Rational const& rhs);

        friend Rational operator&(Rational lhs, Rational const& rhs);
        friend Rational operator|(Rational lhs, Rational const& rhs);
        friend Rational operator^(Rational lhs, Rational const& rhs);

        friend bool operator==(Rational const& lhs, Rational const& rhs);
        friend bool operator!=(Rational const& lhs, Rational const& rhs);
        friend bool operator<(Rational const& lhs, Rational const& rhs);
        friend bool operator>(Rational const& lhs, Rational const& rhs);
        friend bool operator<=(Rational const& lhs, Rational const& rhs);
        friend bool operator>=(Rational const& lhs, Rational const& rhs);

        std::wstring ToString(uint32_t radix, NumberFormat format, int32_t precision) const;
        uint64_t ToUInt64_t() const;

    private:
        Number m_p;
        Number m_q;
    };
}
