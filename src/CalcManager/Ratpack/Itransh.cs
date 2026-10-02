// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        public static void asinhrat(ref Rat px, uint radix, int precision)
        {
            Rat neg_pt_eight_five = pt_eight_five.Clone();
            neg_pt_eight_five.P.Sign *= -1;

            if (rat_gt(px, pt_eight_five, precision) || rat_lt(px, neg_pt_eight_five, precision))
            {
                Rat ptmp = px.Clone();
                mulrat(ref ptmp, px, precision);
                _addrat(ref ptmp, rat_one, precision);
                rootrat(ref ptmp, rat_two, radix, precision);
                _addrat(ref px, ptmp, precision);
                _lograt(ref px, precision);
            }
            else
            {
                Rat xx = px.Clone();
                mulrat(ref xx, px, precision);
                xx.P.Sign *= -1;

                Rat pret = px.Clone();
                Rat thisterm = px.Clone();

                Number n2 = num_one.Clone();

                do
                {
                    mulrat(ref thisterm, xx, precision);
                    MULNUM(ref thisterm, n2);
                    MULNUM(ref thisterm, n2);
                    INC(ref n2);
                    DIVNUM(ref thisterm, n2);
                    INC(ref n2);
                    DIVNUM(ref thisterm, n2);
                    _addrat(ref pret, thisterm, precision);
                } while (!SMALL_ENOUGH_RAT(thisterm, precision));

                trimit(ref pret, precision);
                px = pret;
            }
        }

        public static void acoshrat(ref Rat px, uint radix, int precision)
        {
            if (rat_lt(px, rat_one, precision))
            {
                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
            }

            Rat ptmp = px.Clone();
            mulrat(ref ptmp, px, precision);
            _subrat(ref ptmp, rat_one, precision);
            rootrat(ref ptmp, rat_two, radix, precision);
            _addrat(ref px, ptmp, precision);
            _lograt(ref px, precision);
        }

        public static void atanhrat(ref Rat px, int precision)
        {
            Rat ptmp = px.Clone();
            _subrat(ref ptmp, rat_one, precision);
            _addrat(ref px, rat_one, precision);
            divrat(ref px, ptmp, precision);
            px.P.Sign *= -1;
            _lograt(ref px, precision);
            divrat(ref px, rat_two, precision);
        }
    }
}
