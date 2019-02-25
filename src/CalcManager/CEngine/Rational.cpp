// Copyright (c) Microsoft Corporation. All rights reserved.

#include "pch.h"
#include "Header Files/Rational.h"

using namespace std;

namespace CalcEngine
{
    Rational::Rational() noexcept :
        m_p{},
        m_q{ 1, 0, { 1 } }
    {}

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

    Rational::Rational(Number const& p, Number const& q) noexcept :
        m_p{ p },
        m_q{ q }
    {}

    Rational::Rational(int32_t i)
    {
        PRAT pr = longtorat(static_cast<long>(i));

        m_p = Number{ pr->pp };
        m_q = Number{ pr->pq };

        destroyrat(pr);
    }

    Rational::Rational(uint32_t ui)
    {
        PRAT pr = Ulongtorat(static_cast<unsigned long>(ui));

        m_p = Number{ pr->pp };
        m_q = Number{ pr->pq };

        destroyrat(pr);
    }

    Rational::Rational(uint64_t ui)
    {
        uint32_t hi = HIDWORD(ui);
        uint32_t lo = LODWORD(ui);

        Rational temp = (Rational{ hi } << 32) | lo;

        m_p = Number{ temp.P() };
        m_q = Number{ temp.Q() };
    }

    Rational::Rational(PRAT prat) noexcept :
        m_p{ Number{prat->pp} },
        m_q{ Number{prat->pq} }
    {}

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
        catch (DWORD error)
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
        catch (DWORD error)
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
        catch (DWORD error)
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
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        *this = Rational{ lhsRat };
        destroyrat(lhsRat);

        return *this;
    }

    Rational& Rational::operator%=(Rational const& rhs)
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            modrat(&lhsRat, rhsRat);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
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
        catch (DWORD error)
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
        catch (DWORD error)
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
        catch (DWORD error)
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
        catch (DWORD error)
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
        catch (DWORD error)
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
        catch (DWORD error)
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
        catch (DWORD error)
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

    wstring Rational::ToString(uint32_t radix, NUMOBJ_FMT fmt, int32_t precision) const
    {
        PRAT rat = this->ToPRAT();
        wstring result{};

        try
        {
            result = RatToString(rat, fmt, radix, precision);
        }
        catch (DWORD error)
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
            result = rattoUlonglong(rat, RATIONAL_BASE, RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            destroyrat(rat);
            throw(error);
        }

        destroyrat(rat);

        return result;
    }
}
