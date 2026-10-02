// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        public static bool IsValidForHypFunc(Rat px, int precision)
        {
            Rat ptmp = rat_min_exp.Clone();
            divrat(ref ptmp, rat_ten, precision);
            return !rat_lt(px, ptmp, precision);
        }

        public static void _sinhrat(ref Rat px, int precision)
        {
            if (!IsValidForHypFunc(px, precision))
            {
                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
            }

            Rat xx = px.Clone();
            mulrat(ref xx, px, precision);

            Rat pret = px.Clone();
            Rat thisterm = pret.Clone();

            Number n2 = num_one.Clone();

            do
            {
                mulrat(ref thisterm, xx, precision);
                INC(ref n2);
                DIVNUM(ref thisterm, n2);
                INC(ref n2);
                DIVNUM(ref thisterm, n2);
                _addrat(ref pret, thisterm, precision);
            } while (!SMALL_ENOUGH_RAT(thisterm, precision));

            trimit(ref pret, precision);
            px = pret;
        }

        public static void sinhrat(ref Rat px, uint radix, int precision)
        {
            if (rat_ge(px, rat_one, precision))
            {
                Rat tmpx = px.Clone();
                exprat(ref px, radix, precision);
                tmpx.P.Sign *= -1;
                exprat(ref tmpx, radix, precision);
                _subrat(ref px, tmpx, precision);
                divrat(ref px, rat_two, precision);
            }
            else
            {
                _sinhrat(ref px, precision);
            }
        }

        public static void _coshrat(ref Rat px, uint radix, int precision)
        {
            if (!IsValidForHypFunc(px, precision))
            {
                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
            }

            Rat xx = px.Clone();
            mulrat(ref xx, px, precision);

            Rat pret = new Rat(i32tonum(1, radix), i32tonum(1, radix));
            Rat thisterm = pret.Clone();

            Number n2 = i32tonum(0, radix);

            do
            {
                mulrat(ref thisterm, xx, precision);
                INC(ref n2);
                DIVNUM(ref thisterm, n2);
                INC(ref n2);
                DIVNUM(ref thisterm, n2);
                _addrat(ref pret, thisterm, precision);
            } while (!SMALL_ENOUGH_RAT(thisterm, precision));

            trimit(ref pret, precision);
            px = pret;
        }

        public static void coshrat(ref Rat px, uint radix, int precision)
        {
            px.P.Sign = 1;
            px.Q.Sign = 1;
            if (rat_ge(px, rat_one, precision))
            {
                Rat tmpx = px.Clone();
                exprat(ref px, radix, precision);
                tmpx.P.Sign *= -1;
                exprat(ref tmpx, radix, precision);
                _addrat(ref px, tmpx, precision);
                divrat(ref px, rat_two, precision);
            }
            else
            {
                _coshrat(ref px, radix, precision);
            }

            if (rat_lt(px, rat_one, precision))
            {
                px = rat_one.Clone();
            }
        }

        public static void tanhrat(ref Rat px, uint radix, int precision)
        {
            Rat ptmp = px.Clone();
            sinhrat(ref px, radix, precision);
            coshrat(ref ptmp, radix, precision);
            mulnumx(ref px.P, ptmp.Q);
            mulnumx(ref px.Q, ptmp.P);
        }
    }
}
