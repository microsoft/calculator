// Copyright (c) Microsoft Corporation. All rights reserved.

#include "Header Files/Rational.h"

using namespace std;

namespace CalcEngine
{
    Rational::Rational() noexcept
        : m_p{}
        , m_q{ 1, 0, { 1 } }
    {
    }

    Rational::Rational(Number const& n) noexcept
    {
        int32_t qExp = 0;
        if (n.Exp() < 0)
        {
            qExp -= n.Exp();
        }

        m_p = Number(n.Sign(), 0, n.Mantissa());
        m_q = Number(1, qExp, { 1 });
    }

    Rational::Rational(Number const& p, Number const& q) noexcept
        : m_p{ p }
        , m_q{ q }
    {
    }

    Rational::Rational(int32_t i)
    {
        PRAT pr = i32torat(static_cast<int32_t>(i));

        m_p = Number{ pr->pp };
        m_q = Number{ pr->pq };

        destroyrat(pr);
    }

    Rational::Rational(uint32_t ui)
    {
        PRAT pr = Ui32torat(static_cast<uint32_t>(ui));

        m_p = Number{ pr->pp };
        m_q = Number{ pr->pq };

        destroyrat(pr);
    }

    Rational::Rational(uint64_t ui)
    {
        uint32_t hi = (uint32_t)(((ui) >> 32) & 0xffffffff);
        uint32_t lo = (uint32_t)ui;

        Rational temp = (Rational{ hi } << 32) | lo;

        m_p = Number{ temp.P() };
        m_q = Number{ temp.Q() };
    }

    Rational::Rational(PRAT prat) noexcept
        : m_p{ Number{ prat->pp } }
        , m_q{ Number{ prat->pq } }
    {
    }

    PRAT Rational::ToPRAT() const
    {
        PRAT ret = _createrat();

        ret->pp = this->P().ToPNUMBER();
        ret->pq = this->Q().ToPNUMBER();

        return ret;
    }

    Number const& Rational::P() const
    {
        return m_p;
    }

    Number const& Rational::Q() const
    {
        return m_q;
    }

    bool Rational::IsZero() const
    {
        return m_p.IsZero();
    }

    Rational Rational::operator-() const
    {
        return Rational{ Number{ -1 * m_p.Sign(), m_p.Exp(), m_p.Mantissa() }, m_q };
    }

    Rational& Rational::operator+=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            addrat(&lhsRat, rhsRat, RATIONAL_PRECISION);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational& Rational::operator-=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            subrat(&lhsRat, rhsRat, RATIONAL_PRECISION);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational& Rational::operator*=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            mulrat(&lhsRat, rhsRat, RATIONAL_PRECISION);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational& Rational::operator/=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            divrat(&lhsRat, rhsRat, RATIONAL_PRECISION);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    /// <summary>
    /// Calculate the remainder after division, the sign of a result will match the sign of the current object.
    /// </summary>
    /// <remarks>
    /// This function has the same behavior as the standard C/C++ operator '%'
    /// to calculate the modulus after division instead, use <see cref="RationalMath::Mod"/> instead.
    /// </remarks>
    Rational& Rational::operator%=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            remrat(&lhsRat, rhsRat);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational& Rational::operator<<=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            lshrat(&lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational& Rational::operator>>=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            rshrat(&lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational& Rational::operator&=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            andrat(&lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational& Rational::operator|=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();
        try
        {
            orrat(&lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational& Rational::operator^=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();
        try
        {
            xorrat(&lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            destroyrat(rhsRat);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational operator+(Rational lhs, Rational const& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    Rational operator-(Rational lhs, Rational const& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    Rational operator*(Rational lhs, Rational const& rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    Rational operator/(Rational lhs, Rational const& rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    /// <summary>
    /// Calculate the remainder after division, the sign of a result will match the sign of lhs.
    /// </summary>
    /// <remarks>
    /// This function has the same behavior as the standard C/C++ operator '%', to calculate the modulus after division instead, use <see
    /// cref="Rational::operator%"/> instead.
    /// </remarks>
    Rational operator%(Rational lhs, Rational const& rhs)
    {
        lhs %= rhs;
        return lhs;
    }

    Rational operator<<(Rational lhs, Rational const& rhs)
    {
        lhs <<= rhs;
        return lhs;
    }

    Rational operator>>(Rational lhs, Rational const& rhs)
    {
        lhs >>= rhs;
        return lhs;
    }

    Rational operator&(Rational lhs, Rational const& rhs)
    {
        lhs &= rhs;
        return lhs;
    }

    Rational operator|(Rational lhs, Rational const& rhs)
    {
        lhs |= rhs;
        return lhs;
    }

    Rational operator^(Rational lhs, Rational const& rhs)
    {
        lhs ^= rhs;
        return lhs;
    }

    bool operator==(Rational const& lhs, Rational const& rhs)
    {
        PRAT lhsRat = lhs.ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        bool result = false;
        try
        {
            result = rat_equ(lhsRat, rhsRat, RATIONAL_PRECISION);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        destroyrat(lhsRat);
        destroyrat(rhsRat);

        return result;
    }

    bool operator!=(Rational const& lhs, Rational const& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator<(Rational const& lhs, Rational const& rhs)
    {
        PRAT lhsRat = lhs.ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        bool result = false;
        try
        {
            result = rat_lt(lhsRat, rhsRat, RATIONAL_PRECISION);
        }
        catch (uint32_t error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        destroyrat(lhsRat);
        destroyrat(rhsRat);

        return result;
    }

    bool operator>(Rational const& lhs, Rational const& rhs)
    {
        return rhs < lhs;
    }

    bool operator<=(Rational const& lhs, Rational const& rhs)
    {
        return !(lhs > rhs);
    }

    bool operator>=(Rational const& lhs, Rational const& rhs)
    {
        return !(lhs < rhs);
    }

    wstring Rational::ToString(uint32_t radix, NumberFormat fmt, int32_t precision) const
    {
        PRAT rat = this->ToPRAT();
        wstring result{};

        try
        {
            result = RatToString(rat, fmt, radix, precision);
        }
        catch (uint32_t error)
        {
            destroyrat(rat);
            throw(error);
        }

        destroyrat(rat);

        return result;
    }

    uint64_t Rational::ToUInt64_t() const
    {
        PRAT rat = this->ToPRAT();
        uint64_t result;
        try
        {
            result = rattoUi64(rat, RATIONAL_BASE, RATIONAL_PRECISION);
        }
        catch (uint32_t error)
        {
            destroyrat(rat);
            throw(error);
        }

        destroyrat(rat);

        return result;
    }
}
