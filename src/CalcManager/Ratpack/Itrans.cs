// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        public static void ascalerat(ref Rat pa, AngleType angletype, int precision)
        {
            switch (angletype)
            {
                case AngleType.Radians:
                    break;
                case AngleType.Degrees:
                    divrat(ref pa, two_pi, precision);
                    mulrat(ref pa, rat_360, precision);
                    break;
                case AngleType.Gradians:
                    divrat(ref pa, two_pi, precision);
                    mulrat(ref pa, rat_400, precision);
                    break;
            }
        }

        public static void _asinrat(ref Rat px, int precision)
        {
            Rat xx = px.Clone();
            mulrat(ref xx, px, precision);

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

        public static void asinanglerat(ref Rat pa, AngleType angletype, uint radix, int precision)
        {
            asinrat(ref pa, radix, precision);
            ascalerat(ref pa, angletype, precision);
        }

        public static void asinrat(ref Rat px, uint radix, int precision)
        {
            int sgn = SIGN(px);

            px.P.Sign = 1;
            px.Q.Sign = 1;

            Rat phack = px.Clone();
            _subrat(ref phack, rat_one, precision);

            if (rat_le(phack, rat_smallest, precision) && rat_ge(phack, rat_negsmallest, precision))
            {
                px = pi_over_two.Clone();
            }
            else
            {
                if (rat_gt(px, pt_eight_five, precision))
                {
                    if (rat_gt(px, rat_one, precision))
                    {
                        _subrat(ref px, rat_one, precision);
                        if (rat_gt(px, rat_smallest, precision))
                        {
                            throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
                        }
                        else
                        {
                            px = rat_one.Clone();
                        }
                    }

                    Rat pret = px.Clone();
                    mulrat(ref px, pret, precision);
                    px.P.Sign *= -1;
                    _addrat(ref px, rat_one, precision);
                    rootrat(ref px, rat_two, radix, precision);
                    _asinrat(ref px, precision);
                    px.P.Sign *= -1;
                    _addrat(ref px, pi_over_two, precision);
                }
                else
                {
                    _asinrat(ref px, precision);
                }
            }

            px.P.Sign = sgn;
            px.Q.Sign = 1;
        }

        public static void acosanglerat(ref Rat pa, AngleType angletype, uint radix, int precision)
        {
            acosrat(ref pa, radix, precision);
            ascalerat(ref pa, angletype, precision);
        }

        public static void _acosrat(ref Rat px, int precision)
        {
            Rat xx = px.Clone();
            mulrat(ref xx, px, precision);

            Rat pret = new Rat(i32tonum(0, BASEX), i32tonum(0, BASEX));
            Rat thisterm = new Rat(i32tonum(1, BASEX), i32tonum(1, BASEX));

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

        public static void acosrat(ref Rat px, uint radix, int precision)
        {
            int sgn = SIGN(px);

            px.P.Sign = 1;
            px.Q.Sign = 1;

            if (rat_equ(px, rat_one, precision))
            {
                if (sgn == -1)
                {
                    px = pi.Clone();
                }
                else
                {
                    px = rat_zero.Clone();
                }
            }
            else
            {
                px.P.Sign = sgn;
                asinrat(ref px, radix, precision);
                px.P.Sign *= -1;
                _addrat(ref px, pi_over_two, precision);
            }
        }

        public static void atananglerat(ref Rat pa, AngleType angletype, uint radix, int precision)
        {
            atanrat(ref pa, radix, precision);
            ascalerat(ref pa, angletype, precision);
        }

        public static void _atanrat(ref Rat px, int precision)
        {
            Rat xx = px.Clone();
            mulrat(ref xx, px, precision);

            Rat pret = px.Clone();
            Rat thisterm = px.Clone();

            Number n2 = num_one.Clone();
            xx.P.Sign *= -1;

            do
            {
                mulrat(ref thisterm, xx, precision);
                MULNUM(ref thisterm, n2);
                INC(ref n2);
                INC(ref n2);
                DIVNUM(ref thisterm, n2);
                _addrat(ref pret, thisterm, precision);
            } while (!SMALL_ENOUGH_RAT(thisterm, precision));

            trimit(ref pret, precision);
            px = pret;
        }

        public static void atanrat(ref Rat px, uint radix, int precision)
        {
            int sgn = SIGN(px);

            px.P.Sign = 1;
            px.Q.Sign = 1;

            if (rat_gt(px, pt_eight_five, precision))
            {
                if (rat_gt(px, rat_two, precision))
                {
                    px.P.Sign = sgn;
                    px.Q.Sign = 1;
                    Rat tmpx = rat_one.Clone();
                    divrat(ref tmpx, px, precision);
                    _atanrat(ref tmpx, precision);
                    tmpx.P.Sign = sgn;
                    tmpx.Q.Sign = 1;
                    px = pi_over_two.Clone();
                    _subrat(ref px, tmpx, precision);
                }
                else
                {
                    px.P.Sign = sgn;
                    Rat tmpx = px.Clone();
                    mulrat(ref tmpx, px, precision);
                    _addrat(ref tmpx, rat_one, precision);
                    rootrat(ref tmpx, rat_two, radix, precision);
                    divrat(ref px, tmpx, precision);
                    asinrat(ref px, radix, precision);
                    px.P.Sign = sgn;
                    px.Q.Sign = 1;
                }
            }
            else
            {
                px.P.Sign = sgn;
                px.Q.Sign = 1;
                _atanrat(ref px, precision);
            }

            if (rat_gt(px, pi_over_two, precision))
            {
                _subrat(ref px, pi, precision);
            }
        }
    }
}
