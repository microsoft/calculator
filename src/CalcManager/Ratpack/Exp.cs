// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        public static void _exprat(ref Rat px, int precision)
        {
            Rat xx = px.Clone();
            mulrat(ref xx, px, precision);

            Rat pret = new Rat(i32tonum(0, BASEX), i32tonum(0, BASEX));
            addnum(ref pret.P, num_one, BASEX);
            addnum(ref pret.Q, num_one, BASEX);

            Rat thisterm = pret.Clone();
            Number n2 = i32tonum(0, BASEX);

            do
            {
                mulrat(ref thisterm, px, precision);
                INC(ref n2);
                DIVNUM(ref thisterm, n2);
                _addrat(ref pret, thisterm, precision);
            } while (!SMALL_ENOUGH_RAT(thisterm, precision));

            trimit(ref pret, precision);
            px = pret;
        }

        private static void INC(ref Number a)
        {
            if (a.Mant[0] < BASEX - 1)
            {
                a.Mant[0]++;
            }
            else
            {
                addnum(ref a, num_one, BASEX);
            }
        }

        private static void MULNUM(ref Rat thisterm, Number b)
        {
            mulnumx(ref thisterm.P, b);
        }

        private static void DIVNUM(ref Rat thisterm, Number b)
        {
            mulnumx(ref thisterm.Q, b);
        }

        public static void exprat(ref Rat px, uint radix, int precision)
        {
            if (rat_gt(px, rat_max_exp, precision) || rat_lt(px, rat_min_exp, precision))
            {
                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
            }

            Rat pwr = rat_exp.Clone();
            Rat pint = px.Clone();

            intrat(ref pint, radix, precision);

            int intpwr = rattoi32(pint, radix, precision);
            ratpowi32(ref pwr, intpwr, precision);

            _subrat(ref px, pint, precision);

            if (rat_gt(px, rat_negsmallest, precision) && rat_lt(px, rat_smallest, precision))
            {
                px = pwr.Clone();
            }
            else
            {
                _exprat(ref px, precision);
                mulrat(ref px, pwr, precision);
            }
        }

        public static void __lograt(ref Rat px, int precision)
        {
            Rat xx = px.Clone();
            mulrat(ref xx, px, precision);

            px.Q.Sign *= -1;
            addnum(ref px.P, px.Q, BASEX);
            px.Q.Sign *= -1;

            Rat pret = px.Clone();
            Rat thisterm = px.Clone();

            Number n2 = i32tonum(1, BASEX);
            px.P.Sign *= -1;

            do
            {
                mulrat(ref thisterm, px, precision);
                MULNUM(ref thisterm, n2);
                INC(ref n2);
                DIVNUM(ref thisterm, n2);
                _addrat(ref pret, thisterm, precision);
                TRIMTOP(px, precision);
            } while (!SMALL_ENOUGH_RAT(thisterm, precision));

            trimit(ref pret, precision);
            px = pret;
        }

        public static void _lograt(ref Rat px, int precision)
        {
            if (rat_le(px, rat_zero, precision))
            {
                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
            }

            bool fneglog = rat_lt(px, rat_one, precision);
            if (fneglog)
            {
                Number pnumtemp = px.P;
                px.P = px.Q;
                px.Q = pnumtemp;
            }

            Rat pwr;
            if (LOGRAT2(px) > 1)
            {
                int intpwr = LOGRAT2(px) - 1;
                px.Q.Exp += intpwr;
                pwr = i32torat(intpwr * (int)BASEXPWR);
                mulrat(ref pwr, ln_two, precision);
                TRIMTOP(px, precision);
            }
            else
            {
                pwr = rat_zero.Clone();
            }

            Rat offset = rat_zero.Clone();
            while (rat_gt(px, e_to_one_half, precision))
            {
                divrat(ref px, e_to_one_half, precision);
                _addrat(ref offset, rat_one, precision);
            }

            __lograt(ref px, precision);

            divrat(ref offset, rat_two, precision);
            _addrat(ref pwr, offset, precision);
            _addrat(ref px, pwr, precision);

            trimit(ref px, precision);

            if (fneglog)
            {
                px.P.Sign *= -1;
            }
        }

        public static void lograt(ref Rat px, int precision)
        {
            Rat a = px.Clone();
            _lograt(ref px, precision);
            _snaprat(ref px, a, null, precision);
        }

        public static void log10rat(ref Rat px, int precision)
        {
            lograt(ref px, precision);
            divrat(ref px, ln_ten, precision);
        }

        public static bool IsEven(Rat x, uint radix, int precision)
        {
            Rat tmp = x.Clone();
            divrat(ref tmp, rat_two, precision);
            fracrat(ref tmp, radix, precision);
            _addrat(ref tmp, tmp, precision);
            _subrat(ref tmp, rat_one, precision);
            return rat_lt(tmp, rat_zero, precision);
        }

        public static void powrat(ref Rat px, Rat y, uint radix, int precision)
        {
            if (zerrat(px) || zerrat(y))
            {
                powratcomp(ref px, y, radix, precision);
                return;
            }
            if (rat_equ(y, rat_one, precision))
            {
                return;
            }

            try
            {
                powratNumeratorDenominator(ref px, y, radix, precision);
            }
            catch
            {
                powratcomp(ref px, y, radix, precision);
            }
        }

        public static void powratNumeratorDenominator(ref Rat px, Rat y, uint radix, int precision)
        {
            Rat yNumerator = new Rat(y.P.Clone(), i32tonum(1, BASEX));
            Rat yDenominator = new Rat(y.Q.Clone(), i32tonum(1, BASEX));

            Rat pxPow = px.Clone();

            if (!rat_equ(yNumerator, rat_one, precision))
            {
                powratcomp(ref pxPow, yNumerator, radix, precision);
            }

            if (!rat_equ(yDenominator, rat_one, precision))
            {
                Rat oneoveryDenom = rat_one.Clone();
                divrat(ref oneoveryDenom, yDenominator, precision);

                Rat originalResult = pxPow.Clone();
                powratcomp(ref originalResult, oneoveryDenom, radix, precision);

                Rat roundedResult = originalResult.Clone();
                if (roundedResult.P.Sign == -1)
                {
                    _subrat(ref roundedResult, rat_half, precision);
                }
                else
                {
                    _addrat(ref roundedResult, rat_half, precision);
                }
                intrat(ref roundedResult, radix, precision);

                Rat roundedPower = roundedResult.Clone();
                powratcomp(ref roundedPower, yDenominator, radix, precision);

                if (rat_equ(roundedPower, pxPow, precision))
                {
                    px = roundedResult.Clone();
                }
                else
                {
                    px = originalResult.Clone();
                }
            }
            else
            {
                px = pxPow.Clone();
            }
        }

        public static void powratcomp(ref Rat px, Rat y, uint radix, int precision)
        {
            int sign = SIGN(px);
            px.P.Sign = 1;
            px.Q.Sign = 1;

            if (zerrat(px))
            {
                if (rat_lt(y, rat_zero, precision))
                {
                    throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
                }
                else if (zerrat(y))
                {
                    px = rat_one.Clone();
                    sign = 1;
                }
            }
            else
            {
                Rat pxint = px.Clone();
                _subrat(ref pxint, rat_one, precision);
                if (rat_gt(pxint, rat_negsmallest, precision) && rat_lt(pxint, rat_smallest, precision) && (sign == 1))
                {
                    px = rat_one.Clone();
                    sign = 1;
                }
                else
                {
                    Rat podd = y.Clone();
                    fracrat(ref podd, radix, precision);
                    if (rat_gt(podd, rat_negsmallest, precision) && rat_lt(podd, rat_smallest, precision))
                    {
                        Rat iy = y.Clone();
                        _subrat(ref iy, podd, precision);
                        int inty = rattoi32(iy, radix, precision);

                        Rat plnx = px.Clone();
                        _lograt(ref plnx, precision);
                        mulrat(ref plnx, iy, precision);
                        if (rat_gt(plnx, rat_max_exp, precision) || rat_lt(plnx, rat_min_exp, precision))
                        {
                            throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
                        }

                        ratpowi32(ref px, inty, precision);
                        if ((inty & 1) == 0)
                        {
                            sign = 1;
                        }
                    }
                    else
                    {
                        if (sign == -1)
                        {
                            Rat pNumerator = new Rat(y.P.Clone(), i32tonum(1, BASEX));
                            Rat pDenominator = new Rat(y.Q.Clone(), i32tonum(1, BASEX));
                            pNumerator.P.Sign = 1;
                            pDenominator.P.Sign = 1;

                            bool fBadExponent = false;

                            while (IsEven(pNumerator, radix, precision) && IsEven(pDenominator, radix, precision))
                            {
                                divrat(ref pNumerator, rat_two, precision);
                                divrat(ref pDenominator, rat_two, precision);
                            }
                            if (IsEven(pDenominator, radix, precision))
                            {
                                fBadExponent = true;
                            }
                            if (IsEven(pNumerator, radix, precision))
                            {
                                sign = 1;
                            }

                            if (fBadExponent)
                            {
                                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
                            }
                        }
                        else
                        {
                            sign = 1;
                        }

                        _lograt(ref px, precision);
                        mulrat(ref px, y, precision);
                        exprat(ref px, radix, precision);
                    }
                }
            }

            px.P.Sign *= sign;
        }
    }
}
