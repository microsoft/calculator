// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        private const int RATIO_FOR_DECIMAL = 9;
        private const int DECIMAL = 10;
        private const int CALC_DECIMAL_DIGITS_DEFAULT = 32;

        private static int cbitsofprecision = RATIO_FOR_DECIMAL * DECIMAL * CALC_DECIMAL_DIGITS_DEFAULT;

        public static Number num_one;
        public static Number num_two;
        public static Number num_five;
        public static Number num_six;
        public static Number num_ten;

        public static Rat ln_ten;
        public static Rat ln_two;
        public static Rat rat_zero;
        public static Rat rat_one;
        public static Rat rat_neg_one;
        public static Rat rat_two;
        public static Rat rat_six;
        public static Rat rat_half;
        public static Rat rat_ten;
        public static Rat pt_eight_five;
        public static Rat pi;
        public static Rat pi_over_two;
        public static Rat two_pi;
        public static Rat one_pt_five_pi;
        public static Rat e_to_one_half;
        public static Rat rat_exp;
        public static Rat rad_to_deg;
        public static Rat rad_to_grad;
        public static Rat rat_qword;
        public static Rat rat_dword; // unsigned max ui32
        public static Rat rat_word;
        public static Rat rat_byte;
        public static Rat rat_360;
        public static Rat rat_400;
        public static Rat rat_180;
        public static Rat rat_200;
        public static Rat rat_nRadix;
        public static Rat rat_smallest;
        public static Rat rat_negsmallest;
        public static Rat rat_max_exp;
        public static Rat rat_min_exp;
        public static Rat rat_max_fact;
        public static Rat rat_min_fact;
        public static Rat rat_min_i32; // min signed i32
        public static Rat rat_max_i32; // max signed i32

        static Ratpak()
        {
            _readconstants();
        }

        public static void _readconstants()
        {
            num_one = RatConst.init_num_one.Clone();
            num_two = RatConst.init_num_two.Clone();
            num_five = RatConst.init_num_five.Clone();
            num_six = RatConst.init_num_six.Clone();
            num_ten = RatConst.init_num_ten.Clone();

            pt_eight_five = new Rat(RatConst.init_p_pt_eight_five.Clone(), RatConst.init_q_pt_eight_five.Clone());
            rat_six = new Rat(RatConst.init_p_rat_six.Clone(), RatConst.init_q_rat_six.Clone());
            rat_two = new Rat(RatConst.init_p_rat_two.Clone(), RatConst.init_q_rat_two.Clone());
            rat_zero = new Rat(RatConst.init_p_rat_zero.Clone(), RatConst.init_q_rat_zero.Clone());
            rat_one = new Rat(RatConst.init_p_rat_one.Clone(), RatConst.init_q_rat_one.Clone());
            rat_neg_one = new Rat(RatConst.init_p_rat_neg_one.Clone(), RatConst.init_q_rat_neg_one.Clone());
            rat_half = new Rat(RatConst.init_p_rat_half.Clone(), RatConst.init_q_rat_half.Clone());
            rat_ten = new Rat(RatConst.init_p_rat_ten.Clone(), RatConst.init_q_rat_ten.Clone());
            pi = new Rat(RatConst.init_p_pi.Clone(), RatConst.init_q_pi.Clone());
            two_pi = new Rat(RatConst.init_p_two_pi.Clone(), RatConst.init_q_two_pi.Clone());
            pi_over_two = new Rat(RatConst.init_p_pi_over_two.Clone(), RatConst.init_q_pi_over_two.Clone());
            one_pt_five_pi = new Rat(RatConst.init_p_one_pt_five_pi.Clone(), RatConst.init_q_one_pt_five_pi.Clone());
            e_to_one_half = new Rat(RatConst.init_p_e_to_one_half.Clone(), RatConst.init_q_e_to_one_half.Clone());
            rat_exp = new Rat(RatConst.init_p_rat_exp.Clone(), RatConst.init_q_rat_exp.Clone());
            ln_ten = new Rat(RatConst.init_p_ln_ten.Clone(), RatConst.init_q_ln_ten.Clone());
            ln_two = new Rat(RatConst.init_p_ln_two.Clone(), RatConst.init_q_ln_two.Clone());
            rad_to_deg = new Rat(RatConst.init_p_rad_to_deg.Clone(), RatConst.init_q_rad_to_deg.Clone());
            rad_to_grad = new Rat(RatConst.init_p_rad_to_grad.Clone(), RatConst.init_q_rad_to_grad.Clone());
            rat_qword = new Rat(RatConst.init_p_rat_qword.Clone(), RatConst.init_q_rat_qword.Clone());
            rat_dword = new Rat(RatConst.init_p_rat_dword.Clone(), RatConst.init_q_rat_dword.Clone());
            rat_word = new Rat(RatConst.init_p_rat_word.Clone(), RatConst.init_q_rat_word.Clone());
            rat_byte = new Rat(RatConst.init_p_rat_byte.Clone(), RatConst.init_q_rat_byte.Clone());
            rat_360 = new Rat(RatConst.init_p_rat_360.Clone(), RatConst.init_q_rat_360.Clone());
            rat_400 = new Rat(RatConst.init_p_rat_400.Clone(), RatConst.init_q_rat_400.Clone());
            rat_180 = new Rat(RatConst.init_p_rat_180.Clone(), RatConst.init_q_rat_180.Clone());
            rat_200 = new Rat(RatConst.init_p_rat_200.Clone(), RatConst.init_q_rat_200.Clone());
            rat_smallest = new Rat(RatConst.init_p_rat_smallest.Clone(), RatConst.init_q_rat_smallest.Clone());
            rat_negsmallest = new Rat(RatConst.init_p_rat_negsmallest.Clone(), RatConst.init_q_rat_negsmallest.Clone());
            rat_max_exp = new Rat(RatConst.init_p_rat_max_exp.Clone(), RatConst.init_q_rat_max_exp.Clone());
            rat_min_exp = new Rat(RatConst.init_p_rat_min_exp.Clone(), RatConst.init_q_rat_min_exp.Clone());
            rat_max_fact = new Rat(RatConst.init_p_rat_max_fact.Clone(), RatConst.init_q_rat_max_fact.Clone());
            rat_min_fact = new Rat(RatConst.init_p_rat_min_fact.Clone(), RatConst.init_q_rat_min_fact.Clone());
            rat_min_i32 = new Rat(RatConst.init_p_rat_min_i32.Clone(), RatConst.init_q_rat_min_i32.Clone());
            rat_max_i32 = new Rat(RatConst.init_p_rat_max_i32.Clone(), RatConst.init_q_rat_max_i32.Clone());
        }

        public static void ChangeConstants(uint radix, int precision)
        {
            g_ratio = (int)Math.Ceiling(BASEXPWR / Math.Log(radix, 2)) - 1;
            rat_nRadix = i32torat((int)radix);

            if (cbitsofprecision < (g_ratio * (int)radix * precision))
            {
                g_ftrueinfinite = false;

                num_one = i32tonum(1, BASEX);
                num_two = i32tonum(2, BASEX);
                num_five = i32tonum(5, BASEX);
                num_six = i32tonum(6, BASEX);
                num_ten = i32tonum(10, BASEX);

                rat_six = i32torat(6);
                rat_two = i32torat(2);
                rat_zero = i32torat(0);
                rat_one = i32torat(1);
                rat_neg_one = i32torat(-1);
                rat_ten = i32torat(10);
                rat_word = i32torat(0xffff);
                rat_byte = i32torat(0xff);
                rat_400 = i32torat(400);
                rat_360 = i32torat(360);
                rat_200 = i32torat(200);
                rat_180 = i32torat(180);
                rat_max_exp = i32torat(100000);
                rat_max_fact = i32torat(3249);
                rat_min_fact = i32torat(-1000);

                rat_smallest = rat_nRadix.Clone();
                ratpowi32(ref rat_smallest, -precision, precision);
                rat_negsmallest = rat_smallest.Clone();
                rat_negsmallest.P.Sign = -1;

                rat_half = new Rat(num_one.Clone(), num_two.Clone());
                pt_eight_five = new Rat(i32tonum(85, BASEX), i32tonum(100, BASEX));

                rat_qword = rat_two.Clone();
                numpowi32(ref rat_qword.P, 64, BASEX, precision);
                _subrat(ref rat_qword, rat_one, precision);

                rat_dword = rat_two.Clone();
                numpowi32(ref rat_dword.P, 32, BASEX, precision);
                _subrat(ref rat_dword, rat_one, precision);

                rat_max_i32 = rat_two.Clone();
                numpowi32(ref rat_max_i32.P, 31, BASEX, precision);
                rat_min_i32 = rat_max_i32.Clone();
                _subrat(ref rat_max_i32, rat_one, precision);

                rat_min_i32.P.Sign *= -1;

                rat_min_exp = rat_max_exp.Clone();
                rat_min_exp.P.Sign *= -1;

                cbitsofprecision = g_ratio * (int)radix * precision;

                int extraPrecision = precision + g_ratio;
                pi = rat_half.Clone();
                asinrat(ref pi, radix, extraPrecision);
                mulrat(ref pi, rat_six, extraPrecision);

                two_pi = pi.Clone();
                pi_over_two = pi.Clone();
                one_pt_five_pi = pi.Clone();
                _addrat(ref two_pi, pi, extraPrecision);
                divrat(ref pi_over_two, rat_two, extraPrecision);
                _addrat(ref one_pt_five_pi, pi_over_two, extraPrecision);

                e_to_one_half = rat_half.Clone();
                _exprat(ref e_to_one_half, extraPrecision);

                rat_exp = rat_one.Clone();
                _exprat(ref rat_exp, extraPrecision);

                ln_ten = rat_ten.Clone();
                _lograt(ref ln_ten, extraPrecision);

                ln_two = rat_two.Clone();
                _lograt(ref ln_two, extraPrecision);

                rad_to_deg = i32torat(180);
                divrat(ref rad_to_deg, pi, extraPrecision);

                rad_to_grad = i32torat(200);
                divrat(ref rad_to_grad, pi, extraPrecision);
            }
            else
            {
                _readconstants();

                rat_smallest = rat_nRadix.Clone();
                ratpowi32(ref rat_smallest, -precision, precision);
                rat_negsmallest = rat_smallest.Clone();
                rat_negsmallest.P.Sign = -1;
            }
        }

        public static void intrat(ref Rat px, uint radix, int precision)
        {
            if (!zernum(px.P) && !equnum(px.Q, num_one))
            {
                flatrat(ref px, radix, precision);

                Rat pret = px.Clone();
                remrat(ref pret, rat_one);

                if (!equnum(px.Q, pret.Q))
                {
                    flatrat(ref pret, radix, precision);
                }

                _subrat(ref px, pret, precision);
                flatrat(ref px, radix, precision);
            }
        }

        public static bool rat_equ(Rat a, Rat b, int precision)
        {
            Rat rattmp = a.Clone();
            rattmp.P.Sign *= -1;
            _addrat(ref rattmp, b, precision);
            return zernum(rattmp.P);
        }

        public static bool rat_ge(Rat a, Rat b, int precision)
        {
            Rat rattmp = a.Clone();
            b.P.Sign *= -1;
            _addrat(ref rattmp, b, precision);
            b.P.Sign *= -1;
            return zernum(rattmp.P) || SIGN(rattmp) == 1;
        }

        public static bool rat_gt(Rat a, Rat b, int precision)
        {
            Rat rattmp = a.Clone();
            b.P.Sign *= -1;
            _addrat(ref rattmp, b, precision);
            b.P.Sign *= -1;
            return !zernum(rattmp.P) && SIGN(rattmp) == 1;
        }

        public static bool rat_le(Rat a, Rat b, int precision)
        {
            Rat rattmp = a.Clone();
            b.P.Sign *= -1;
            _addrat(ref rattmp, b, precision);
            b.P.Sign *= -1;
            return zernum(rattmp.P) || SIGN(rattmp) == -1;
        }

        public static bool rat_lt(Rat a, Rat b, int precision)
        {
            Rat rattmp = a.Clone();
            b.P.Sign *= -1;
            _addrat(ref rattmp, b, precision);
            b.P.Sign *= -1;
            return !zernum(rattmp.P) && SIGN(rattmp) == -1;
        }

        public static bool rat_neq(Rat a, Rat b, int precision)
        {
            Rat rattmp = a.Clone();
            rattmp.P.Sign *= -1;
            _addrat(ref rattmp, b, precision);
            return !zernum(rattmp.P);
        }

        public static void scale(ref Rat px, Rat scalefact, uint radix, int precision)
        {
            Rat pret = px.Clone();
            int logscale = g_ratio * ((pret.P.CDigit + pret.P.Exp) - (pret.Q.CDigit + pret.Q.Exp));
            if (logscale > 0)
            {
                precision += logscale;
            }

            divrat(ref pret, scalefact, precision);
            intrat(ref pret, radix, precision);
            mulrat(ref pret, scalefact, precision);
            pret.P.Sign *= -1;
            _addrat(ref px, pret, precision);
        }

        public static void scale2pi(ref Rat px, uint radix, int precision)
        {
            Rat pret = px.Clone();
            Rat my_two_pi;
            int logscale = g_ratio * ((pret.P.CDigit + pret.P.Exp) - (pret.Q.CDigit + pret.Q.Exp));
            if (logscale > 0)
            {
                precision += logscale;
                my_two_pi = rat_half.Clone();
                asinrat(ref my_two_pi, radix, precision);
                mulrat(ref my_two_pi, rat_six, precision);
                mulrat(ref my_two_pi, rat_two, precision);
            }
            else
            {
                my_two_pi = two_pi.Clone();
            }

            divrat(ref pret, my_two_pi, precision);
            intrat(ref pret, radix, precision);
            mulrat(ref pret, my_two_pi, precision);
            pret.P.Sign *= -1;
            _addrat(ref px, pret, precision);
        }

        public static void inbetween(ref Rat px, Rat range, int precision)
        {
            if (rat_gt(px, range, precision))
            {
                px = range.Clone();
            }
            else
            {
                range.P.Sign *= -1;
                if (rat_lt(px, range, precision))
                {
                    px = range.Clone();
                }
                range.P.Sign *= -1;
            }
        }

        public static void trimit(ref Rat px, int precision)
        {
            if (!g_ftrueinfinite)
            {
                Number pp = px.P;
                Number pq = px.Q;
                int trim = g_ratio * (Math.Min(pp.CDigit + pp.Exp, pq.CDigit + pq.Exp) - 1) - precision;
                if (trim > g_ratio)
                {
                    trim /= g_ratio;

                    if (trim <= pp.Exp)
                    {
                        pp.Exp -= trim;
                    }
                    else
                    {
                        int shift = trim - pp.Exp;
                        int newCount = pp.CDigit - shift;
                        if (newCount > 0)
                        {
                            Array.Copy(pp.Mant, shift, pp.Mant, 0, newCount);
                            pp.CDigit = newCount;
                        }
                        pp.Exp = 0;
                    }

                    if (trim <= pq.Exp)
                    {
                        pq.Exp -= trim;
                    }
                    else
                    {
                        int shift = trim - pq.Exp;
                        int newCount = pq.CDigit - shift;
                        if (newCount > 0)
                        {
                            Array.Copy(pq.Mant, shift, pq.Mant, 0, newCount);
                            pq.CDigit = newCount;
                        }
                        pq.Exp = 0;
                    }
                }
                int expTrim = Math.Min(pp.Exp, pq.Exp);
                pp.Exp -= expTrim;
                pq.Exp -= expTrim;
            }
        }
    }
}
