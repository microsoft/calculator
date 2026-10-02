// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        public static void scalerat(ref Rat pa, AngleType angletype, uint radix, int precision)
        {
            switch (angletype)
            {
                case AngleType.Radians:
                    scale2pi(ref pa, radix, precision);
                    break;
                case AngleType.Degrees:
                    scale(ref pa, rat_360, radix, precision);
                    break;
                case AngleType.Gradians:
                    scale(ref pa, rat_400, radix, precision);
                    break;
            }
        }

        public static void _sinrat(ref Rat px, int precision)
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
                INC(ref n2);
                DIVNUM(ref thisterm, n2);
                INC(ref n2);
                DIVNUM(ref thisterm, n2);
                _addrat(ref pret, thisterm, precision);
            } while (!SMALL_ENOUGH_RAT(thisterm, precision));

            trimit(ref pret, precision);
            px = pret;

            inbetween(ref px, rat_one, precision);

            if (rat_le(px, rat_smallest, precision) && rat_ge(px, rat_negsmallest, precision))
            {
                px = rat_zero.Clone();
            }
        }

        public static void sinrat(ref Rat px, uint radix, int precision)
        {
            scale2pi(ref px, radix, precision);
            _sinrat(ref px, precision);
        }

        public static void sinanglerat(ref Rat pa, AngleType angletype, uint radix, int precision)
        {
            scalerat(ref pa, angletype, radix, precision);
            switch (angletype)
            {
                case AngleType.Degrees:
                    if (rat_gt(pa, rat_180, precision))
                    {
                        _subrat(ref pa, rat_360, precision);
                    }
                    divrat(ref pa, rat_180, precision);
                    mulrat(ref pa, pi, precision);
                    break;
                case AngleType.Gradians:
                    if (rat_gt(pa, rat_200, precision))
                    {
                        _subrat(ref pa, rat_400, precision);
                    }
                    divrat(ref pa, rat_200, precision);
                    mulrat(ref pa, pi, precision);
                    break;
            }
            _sinrat(ref pa, precision);
        }

        public static void _cosrat(ref Rat px, uint radix, int precision)
        {
            Rat xx = px.Clone();
            mulrat(ref xx, px, precision);

            Rat pret = new Rat(i32tonum(1, radix), i32tonum(1, radix));
            Rat thisterm = pret.Clone();

            Number n2 = i32tonum(0, radix);
            xx.P.Sign *= -1;

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

            inbetween(ref px, rat_one, precision);

            if (rat_le(px, rat_smallest, precision) && rat_ge(px, rat_negsmallest, precision))
            {
                px = rat_zero.Clone();
            }
        }

        public static void cosrat(ref Rat px, uint radix, int precision)
        {
            scale2pi(ref px, radix, precision);
            _cosrat(ref px, radix, precision);
        }

        public static void cosanglerat(ref Rat pa, AngleType angletype, uint radix, int precision)
        {
            scalerat(ref pa, angletype, radix, precision);
            switch (angletype)
            {
                case AngleType.Degrees:
                    if (rat_gt(pa, rat_180, precision))
                    {
                        Rat ptmp = rat_360.Clone();
                        _subrat(ref ptmp, pa, precision);
                        pa = ptmp;
                    }
                    divrat(ref pa, rat_180, precision);
                    mulrat(ref pa, pi, precision);
                    break;
                case AngleType.Gradians:
                    if (rat_gt(pa, rat_200, precision))
                    {
                        Rat ptmp = rat_400.Clone();
                        _subrat(ref ptmp, pa, precision);
                        pa = ptmp;
                    }
                    divrat(ref pa, rat_200, precision);
                    mulrat(ref pa, pi, precision);
                    break;
            }
            _cosrat(ref pa, radix, precision);
        }

        public static void _tanrat(ref Rat px, uint radix, int precision)
        {
            Rat ptmp = px.Clone();
            _sinrat(ref px, precision);
            _cosrat(ref ptmp, radix, precision);
            if (zerrat(ptmp))
            {
                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
            }
            divrat(ref px, ptmp, precision);
        }

        public static void tanrat(ref Rat px, uint radix, int precision)
        {
            scale2pi(ref px, radix, precision);
            _tanrat(ref px, radix, precision);
        }

        public static void tananglerat(ref Rat pa, AngleType angletype, uint radix, int precision)
        {
            scalerat(ref pa, angletype, radix, precision);
            switch (angletype)
            {
                case AngleType.Degrees:
                    if (rat_gt(pa, rat_180, precision))
                    {
                        _subrat(ref pa, rat_180, precision);
                    }
                    divrat(ref pa, rat_180, precision);
                    mulrat(ref pa, pi, precision);
                    break;
                case AngleType.Gradians:
                    if (rat_gt(pa, rat_200, precision))
                    {
                        _subrat(ref pa, rat_200, precision);
                    }
                    divrat(ref pa, rat_200, precision);
                    mulrat(ref pa, pi, precision);
                    break;
            }
            _tanrat(ref pa, radix, precision);
        }
    }
}
