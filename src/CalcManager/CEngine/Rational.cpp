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

    Rational::Rational(uint64_t ui, uint32_t radix, int32_t precision)
    {
        uint32_t hi = HIDWORD(ui);
        uint32_t lo = LODWORD(ui);

        Rational temp = Rational{ hi }.Lsh(32, radix, precision).Or(lo, radix, precision);

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

    Rational Rational::Negate() const
    {
        return Rational{ Number{ -1 * m_p.Sign(), m_p.Exp(), m_p.Mantissa() }, m_q};
    }
    Rational Rational::Add(Rational const& rhs, int32_t precision) const
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            addrat(&lhsRat, rhsRat, precision);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
    }
    Rational Rational::Sub(Rational const& rhs, int32_t precision) const
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            subrat(&lhsRat, rhsRat, precision);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
    }
    Rational Rational::Mul(Rational const& rhs, int32_t precision) const
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            mulrat(&lhsRat, rhsRat, precision);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
    }
    Rational Rational::Div(Rational const& rhs, int32_t precision) const
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            divrat(&lhsRat, rhsRat, precision);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
    }
    Rational Rational::Mod(Rational const& rhs) const
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

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
    }

    Rational Rational::Lsh(Rational const& rhs, uint32_t radix, int32_t precision) const
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            lshrat(&lhsRat, rhsRat, radix, precision);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
    }
    Rational Rational::Rsh(Rational const& rhs, uint32_t radix, int32_t precision) const
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            rshrat(&lhsRat, rhsRat, radix, precision);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
    }

    Rational Rational::Not(bool isIntegerMode, Rational const& chopNum, uint32_t radix, int32_t precision) const
    {
        Rational result{};

        if (radix == 10 && !isIntegerMode)
        {
            result = RationalMath::Integer(*this, radix, precision);
            result = result.Add(1, precision);
            result = result.Negate();
        }
        else
        {
            result = this->Xor(chopNum, radix, precision);
        }

        return result;
    }
    Rational Rational::And(Rational const& rhs, uint32_t radix, int32_t precision) const
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();

        try
        {
            andrat(&lhsRat, rhsRat, radix, precision);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
    }
    Rational Rational::Or(Rational const& rhs, uint32_t radix, int32_t precision) const
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();
        try
        {
            orrat(&lhsRat, rhsRat, radix, precision);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
    }
    Rational Rational::Xor(Rational const& rhs, uint32_t radix, int32_t precision) const
    {
        PRAT lhsRat = this->ToPRAT();
        PRAT rhsRat = rhs.ToPRAT();
        try
        {
            xorrat(&lhsRat, rhsRat, radix, precision);
            destroyrat(rhsRat);
        }
        catch (DWORD error)
        {
            destroyrat(lhsRat);
            destroyrat(rhsRat);
            throw(error);
        }

        Rational result = Rational{ lhsRat };
        destroyrat(lhsRat);

        return result;
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
