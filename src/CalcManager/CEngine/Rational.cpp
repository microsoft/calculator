// Copyright (c) Microsoft Corporation. All rights reserved.

#include "pch.h"
#include "Header Files/Rational.h"
#include "Header Files/scimath.h"

using namespace std;
using namespace CalcEngine::RationalMath;

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

    Rational::Rational(int32_t i) noexcept
    {
        PRAT pr = longtorat(static_cast<long>(i));

        m_p = Number{ pr->pp };
        m_q = Number{ pr->pq };

        destroyrat(pr);
    }

    Rational::Rational(uint32_t ui) noexcept
    {
        PRAT pr = Ulongtorat(static_cast<unsigned long>(ui));

        m_p = Number{ pr->pp };
        m_q = Number{ pr->pq };

        destroyrat(pr);
    }

    Rational::Rational(uint64_t ui, uint32_t radix, int32_t precision) noexcept
    {
        uint32_t hi = HIDWORD(ui);
        uint32_t lo = LODWORD(ui);

        Rational temp = Lsh(hi, 32, radix, precision);
        temp = Or(temp, lo, radix, precision);

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

    bool Rational::IsZero() const
    {
        return this->P().IsZero();
    }

    bool Rational::IsLess(Rational const& r, int32_t precision) const
    {
        PRAT thisRat = this->ToPRAT();
        PRAT rRat = r.ToPRAT();

        bool result = false;
        try
        {
            result = rat_lt(thisRat, rRat, precision);
        }
        catch (DWORD error)
        {
            destroyrat(thisRat);
            destroyrat(rRat);
            throw(error);
        }

        destroyrat(thisRat);
        destroyrat(rRat);

        return result;
    }

    bool Rational::IsLessEq(Rational const& r, int32_t precision) const
    {
        PRAT thisRat = this->ToPRAT();
        PRAT rRat = r.ToPRAT();

        bool result = false;
        try
        {
            result = rat_le(thisRat, rRat, precision);
        }
        catch (DWORD error)
        {
            destroyrat(thisRat);
            destroyrat(rRat);
            throw(error);
        }

        destroyrat(thisRat);
        destroyrat(rRat);

        return result;
    }

    bool Rational::IsGreaterEq(Rational const& r, int32_t precision) const
    {
        PRAT thisRat = this->ToPRAT();
        PRAT rRat = r.ToPRAT();

        bool result = false;
        try
        {
            result = rat_ge(thisRat, rRat, precision);
        }
        catch (DWORD error)
        {
            destroyrat(thisRat);
            destroyrat(rRat);
            throw(error);
        }

        destroyrat(thisRat);
        destroyrat(rRat);

        return result;
    }

    bool Rational::IsEq(Rational const& r, int32_t precision) const
    {
        PRAT thisRat = this->ToPRAT();
        PRAT rRat = r.ToPRAT();

        bool result = false;
        try
        {
            result = rat_equ(thisRat, rRat, precision);
        }
        catch (DWORD error)
        {
            destroyrat(thisRat);
            destroyrat(rRat);
            throw(error);
        }

        destroyrat(thisRat);
        destroyrat(rRat);

        return result;
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

    uint64_t Rational::ToUInt64_t(uint32_t radix, int32_t precision) const
    {
        PRAT rat = this->ToPRAT();
        uint64_t result{};
        try
        {
            result = rattoUlonglong(rat, radix, precision);
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
