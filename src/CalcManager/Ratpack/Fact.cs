// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        public static void _gamma(ref Rat pn, uint radix, int precision)
        {
            Rat ratprec = i32torat(precision);

            Rat a = i32torat((int)radix);
            _lograt(ref a, precision);
            mulrat(ref a, ratprec, precision);

            _addrat(ref a, rat_two, precision);
            Rat tmp = a.Clone();
            _lograt(ref tmp, precision);
            mulrat(ref tmp, pn, precision);
            _addrat(ref a, tmp, precision);
            _addrat(ref a, rat_one, precision);

            tmp = pn.Clone();
            Rat one_pt_five = i32torat(3);
            divrat(ref one_pt_five, rat_two, precision);
            _addrat(ref tmp, one_pt_five, precision);
            Rat term = a.Clone();
            powratcomp(ref term, tmp, radix, precision);
            tmp = a.Clone();
            exprat(ref tmp, radix, precision);
            mulrat(ref term, tmp, precision);
            _lograt(ref term, precision);
            Rat ratRadix = i32torat((int)radix);
            tmp = ratRadix.Clone();
            _lograt(ref tmp, precision);
            _subrat(ref term, tmp, precision);
            precision += rattoi32(term, radix, precision);

            Rat factorial = rat_one.Clone();
            Number count = i32tonum(0, BASEX);

            Rat mpy = a.Clone();
            powratcomp(ref mpy, pn, radix, precision);
            Rat a2 = a.Clone();
            mulrat(ref a2, a, precision);

            Rat sum = rat_one.Clone();
            divrat(ref sum, pn, precision);
            tmp = pn.Clone();
            _addrat(ref tmp, rat_one, precision);
            term = a.Clone();
            divrat(ref term, tmp, precision);
            _subrat(ref sum, term, precision);

            Rat err = ratRadix.Clone();
            ratprec.P.Sign *= -1;
            powratcomp(ref err, ratprec, radix, precision);
            divrat(ref err, ratRadix, precision);

            term = rat_two.Clone();

            while (!zerrat(term) && rat_gt(term, err, precision))
            {
                _addrat(ref pn, rat_two, precision);

                INC(ref count);
                mulnumx(ref factorial.P, count);
                INC(ref count);
                mulnumx(ref factorial.P, count);

                divrat(ref factorial, a2, precision);

                tmp = pn.Clone();
                _addrat(ref tmp, rat_one, precision);

                term = new Rat(count.Clone(), num_one.Clone());
                _addrat(ref term, rat_one, precision);
                mulrat(ref term, tmp, precision);
                tmp = a.Clone();
                divrat(ref tmp, term, precision);

                term = rat_one.Clone();
                divrat(ref term, pn, precision);
                _subrat(ref term, tmp, precision);

                divrat(ref term, factorial, precision);
                _addrat(ref sum, term, precision);
                ABSRAT(term);
            }

            mulrat(ref sum, mpy, precision);
            pn = sum;
        }

        public static void factrat(ref Rat px, uint radix, int precision)
        {
            if (rat_gt(px, rat_max_fact, precision) || rat_lt(px, rat_min_fact, precision))
            {
                throw new Exception(CalcErr.CALC_E_OVERFLOW.ToString());
            }

            Rat fact = rat_one.Clone();
            Rat neg_rat_one = rat_one.Clone();
            neg_rat_one.P.Sign *= -1;

            Rat frac = px.Clone();
            fracrat(ref frac, radix, precision);

            if ((zerrat(frac) || (LOGRATRADIX(frac) <= -precision)) && (SIGN(px) == -1))
            {
                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
            }

            while (rat_gt(px, rat_zero, precision) && (LOGRATRADIX(px) > -precision))
            {
                mulrat(ref fact, px, precision);
                _subrat(ref px, rat_one, precision);
            }

            if (LOGRATRADIX(px) <= -precision)
            {
                px = rat_zero.Clone();
                intrat(ref fact, radix, precision);
            }

            while (rat_lt(px, neg_rat_one, precision))
            {
                _addrat(ref px, rat_one, precision);
                divrat(ref fact, px, precision);
            }

            if (rat_neq(px, rat_zero, precision))
            {
                _addrat(ref px, rat_one, precision);
                _gamma(ref px, radix, precision);
                mulrat(ref px, fact, precision);
            }
            else
            {
                px = fact.Clone();
            }
        }
    }
}
