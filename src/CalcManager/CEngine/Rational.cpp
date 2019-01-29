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
}
