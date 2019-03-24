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
        unique_ptr<RAT, decltype(&_destroyrat)> pr(Ulongtorat(static_cast<unsigned long>(i)), &_destroyrat);

        m_p = Number{ pr->pp };
        m_q = Number{ pr->pq };
    }

    Rational::Rational(uint32_t ui)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> pr(Ulongtorat(static_cast<unsigned long>(ui)), &_destroyrat);

        m_p = Number{ pr->pp };
        m_q = Number{ pr->pq };
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
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        try
        {
            addrat(&lhsPrat, rhsSmartRat.get(), RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

        return *this;
    }

    Rational& Rational::operator-=(Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        try
        {
            subrat(&lhsPrat, rhsSmartRat.get(), RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

        return *this;
    }

    Rational& Rational::operator*=(Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        try
        {
            mulrat(&lhsPrat, rhsSmartRat.get(), RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

        return *this;
    }

    Rational& Rational::operator/=(Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        try
        {
            divrat(&lhsPrat, rhsSmartRat.get(), RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

        return *this;
    }

    Rational& Rational::operator%=(Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        try
        {
            modrat(&lhsPrat, rhsSmartRat.get());
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

        return *this;
    }

    Rational& Rational::operator<<=(Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        try
        {
            lshrat(&lhsPrat, rhsSmartRat.get(), RATIONAL_BASE, RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

        return *this;
    }

    Rational& Rational::operator>>=(Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        try
        {
            rshrat(&lhsPrat, rhsSmartRat.get(), RATIONAL_BASE, RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

        return *this;
    }

    Rational& Rational::operator&=(Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        try
        {
            andrat(&lhsPrat, rhsSmartRat.get(), RATIONAL_BASE, RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

        return *this;
    }

    Rational& Rational::operator|=(Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);
        try
        {
            orrat(&lhsPrat, rhsSmartRat.get(), RATIONAL_BASE, RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

        return *this;
    }

    Rational& Rational::operator^=(Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(this->ToPRAT(), &_destroyrat);
        auto lhsPrat = lhsSmartRat.get();
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);
        try
        {
            xorrat(&lhsPrat, rhsSmartRat.get(), RATIONAL_BASE, RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        *this = Rational{ lhsSmartRat.get() };

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
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(lhs.ToPRAT(), &_destroyrat);
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        bool result = false;
        try
        {
            result = rat_equ(lhsSmartRat.get(), rhsSmartRat.get(), RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        return result;
    }

    bool operator!=(Rational const& lhs, Rational const& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator<(Rational const& lhs, Rational const& rhs)
    {
        unique_ptr<RAT, decltype(&_destroyrat)> lhsSmartRat(lhs.ToPRAT(), &_destroyrat);
        unique_ptr<RAT, decltype(&_destroyrat)> rhsSmartRat(rhs.ToPRAT(), &_destroyrat);

        bool result = false;
        try
        {
            result = rat_lt(lhsSmartRat.get(), rhsSmartRat.get(), RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

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
        unique_ptr<RAT, decltype(&_destroyrat)> SmartRat(this->ToPRAT(), &_destroyrat);
        auto PRAT = SmartRat.get();
        wstring result{};

        try
        {
            result = RatToString(PRAT, fmt, radix, precision);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        return result;
    }

    uint64_t Rational::ToUInt64_t() const
    {
        unique_ptr<RAT, decltype(&_destroyrat)> smartRat(this->ToPRAT(), &_destroyrat);
        uint64_t result;
        try
        {
            result = rattoUlonglong(smartRat.get(), RATIONAL_BASE, RATIONAL_PRECISION);
        }
        catch (DWORD error)
        {
            throw(error);
        }

        return result;
    }
}
