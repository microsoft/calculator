// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using CalcManager.CEngine;
using CalcManager.Ratpack;
using Number = CalcManager.CEngine.Number;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CalculatorEngineTests
{
    [TestClass]
    public class RationalTest
    {
        [ClassInitialize]
        public static void CommonSetup(TestContext context)
        {
            Ratpak.ChangeConstants(10, 128);
        }

        [TestMethod]
        public void TestModuloOperandsNotModified()
        {
            Rational rat25 = new Rational(25);
            Rational ratminus25 = new Rational(-25);
            Rational rat4 = new Rational(4);
            Rational ratminus4 = new Rational(-4);
            Rational res = RationalMath.Mod(rat25, rat4);
            Assert.AreEqual(new Rational(1), res);
            Assert.AreEqual(new Rational(25), rat25);
            Assert.AreEqual(new Rational(4), rat4);
            res = RationalMath.Mod(rat25, ratminus4);
            Assert.AreEqual(new Rational(-3), res);
            Assert.AreEqual(new Rational(25), rat25);
            Assert.AreEqual(new Rational(-4), ratminus4);
            res = RationalMath.Mod(ratminus25, ratminus4);
            Assert.AreEqual(new Rational(-1), res);
            Assert.AreEqual(new Rational(-25), ratminus25);
            Assert.AreEqual(new Rational(-4), ratminus4);
            res = RationalMath.Mod(ratminus25, rat4);
            Assert.AreEqual(new Rational(3), res);
            Assert.AreEqual(new Rational(-25), ratminus25);
            Assert.AreEqual(new Rational(4), rat4);
        }

        [TestMethod]
        public void TestModuloInteger()
        {
            var res = RationalMath.Mod(new Rational(426), new Rational(56478));
            Assert.AreEqual(new Rational(426), res);
            res = RationalMath.Mod(new Rational(56478), new Rational(426));
            Assert.AreEqual(new Rational(246), res);
            res = RationalMath.Mod(new Rational(-643), new Rational(8756));
            Assert.AreEqual(new Rational(8113), res);
            res = RationalMath.Mod(new Rational(643), new Rational(-8756));
            Assert.AreEqual(new Rational(-8113), res);
            res = RationalMath.Mod(new Rational(-643), new Rational(-8756));
            Assert.AreEqual(new Rational(-643), res);
            res = RationalMath.Mod(new Rational(1000), new Rational(250));
            Assert.AreEqual(new Rational(0), res);
            res = RationalMath.Mod(new Rational(1000), new Rational(-250));
            Assert.AreEqual(new Rational(0), res);
        }

        [TestMethod]
        public void TestModuloZero()
        {
            var res = RationalMath.Mod(new Rational(343654332), new Rational(0));
            Assert.AreEqual(new Rational(343654332), res);
            res = RationalMath.Mod(new Rational(0), new Rational(8756));
            Assert.AreEqual(new Rational(0), res);
            res = RationalMath.Mod(new Rational(0), new Rational(-242));
            Assert.AreEqual(new Rational(0), res);
            res = RationalMath.Mod(new Rational(0), new Rational(0));
            Assert.AreEqual(new Rational(0), res);
            res = RationalMath.Mod(new Rational(new Number(1, 0, new uint[] { 23242 }), new Number(1, 0, new uint[] { 2 })), new Rational(new Number(1, 0, new uint[] { 0 }), new Number(1, 0, new uint[] { 23 })));
            Assert.AreEqual(new Rational(11621), res);
        }

        [TestMethod]
        public void TestModuloRational()
        {
            var res = RationalMath.Mod(new Rational(new Number(1, 0, new uint[] { 250 }), new Number(1, 0, new uint[] { 100 })), new Rational(89));
            Assert.AreEqual("2.5", res.ToString(10, NumberFormat.Float, 8));
            res = RationalMath.Mod(new Rational(new Number(1, 0, new uint[] { 3330 }), new Number(1, 0, new uint[] { 1332 })), new Rational(1));
            Assert.AreEqual("0.5", res.ToString(10, NumberFormat.Float, 8));
            res = RationalMath.Mod(new Rational(new Number(1, 0, new uint[] { 12250 }), new Number(1, 0, new uint[] { 100 })), new Rational(10));
            Assert.AreEqual("2.5", res.ToString(10, NumberFormat.Float, 8));
            res = RationalMath.Mod(new Rational(new Number(-1, 0, new uint[] { 12250 }), new Number(1, 0, new uint[] { 100 })), new Rational(10));
            Assert.AreEqual("7.5", res.ToString(10, NumberFormat.Float, 8));
            res = RationalMath.Mod(new Rational(new Number(-1, 0, new uint[] { 12250 }), new Number(1, 0, new uint[] { 100 })), new Rational(-10));
            Assert.AreEqual("-2.5", res.ToString(10, NumberFormat.Float, 8));
            res = RationalMath.Mod(new Rational(new Number(1, 0, new uint[] { 12250 }), new Number(1, 0, new uint[] { 100 })), new Rational(-10));
            Assert.AreEqual("-7.5", res.ToString(10, NumberFormat.Float, 8));
            res = RationalMath.Mod(new Rational(new Number(1, 0, new uint[] { 1000 }), new Number(1, 0, new uint[] { 3 })), new Rational(1));
            Assert.AreEqual("0.33333333", res.ToString(10, NumberFormat.Float, 8));
            res = RationalMath.Mod(new Rational(new Number(1, 0, new uint[] { 1000 }), new Number(1, 0, new uint[] { 3 })), new Rational(-10));
            Assert.AreEqual("-6.6666667", res.ToString(10, NumberFormat.Float, 8));
            res = RationalMath.Mod(new Rational(834345), new Rational(new Number(1, 0, new uint[] { 103 }), new Number(1, 0, new uint[] { 100 })));
            Assert.AreEqual("0.71", res.ToString(10, NumberFormat.Float, 8));
            res = RationalMath.Mod(new Rational(834345), new Rational(new Number(-1, 0, new uint[] { 103 }), new Number(1, 0, new uint[] { 100 })));
            Assert.AreEqual("-0.32", res.ToString(10, NumberFormat.Float, 8));
        }

        [TestMethod]
        public void TestRemainderOperandsNotModified()
        {
            Rational rat25 = new Rational(25);
            Rational ratminus25 = new Rational(-25);
            Rational rat4 = new Rational(4);
            Rational ratminus4 = new Rational(-4);
            Rational res = rat25 % rat4;
            Assert.AreEqual(new Rational(1), res);
            Assert.AreEqual(new Rational(25), rat25);
            Assert.AreEqual(new Rational(4), rat4);
            res = rat25 % ratminus4;
            Assert.AreEqual(new Rational(1), res);
            Assert.AreEqual(new Rational(25), rat25);
            Assert.AreEqual(new Rational(-4), ratminus4);
            res = ratminus25 % ratminus4;
            Assert.AreEqual(new Rational(-1), res);
            Assert.AreEqual(new Rational(-25), ratminus25);
            Assert.AreEqual(new Rational(-4), ratminus4);
            res = ratminus25 % rat4;
            Assert.AreEqual(new Rational(-1), res);
            Assert.AreEqual(new Rational(-25), ratminus25);
            Assert.AreEqual(new Rational(4), rat4);
        }

        [TestMethod]
        public void TestRemainderInteger()
        {
            var res = new Rational(426) % new Rational(56478);
            Assert.AreEqual(new Rational(426), res);
            res = new Rational(56478) % new Rational(426);
            Assert.AreEqual(new Rational(246), res);
            res = new Rational(-643) % new Rational(8756);
            Assert.AreEqual(new Rational(-643), res);
            res = new Rational(643) % new Rational(-8756);
            Assert.AreEqual(new Rational(643), res);
            res = new Rational(-643) % new Rational(-8756);
            Assert.AreEqual(new Rational(-643), res);
            res = new Rational(-124) % new Rational(-124);
            Assert.AreEqual(new Rational(0), res);
            res = new Rational(24) % new Rational(24);
            Assert.AreEqual(new Rational(0), res);
        }

        [TestMethod]
        public void TestRemainderZero()
        {
            var res = new Rational(0) % new Rational(3654);
            Assert.AreEqual(new Rational(0), res);
            res = new Rational(0) % new Rational(-242);
            Assert.AreEqual(new Rational(0), res);
            foreach (var number in new int[] { 343654332, 0, -23423 })
            {
                try
                {
                    res = new Rational(number) % new Rational(0);
                    Assert.Fail();
                }
                catch (Exception ex)
                {
                    Assert.IsTrue(ex.Message.Contains(CalcErr.CALC_E_INDEFINITE.ToString()) || ex.Message.Contains("undefined"));
                }

                try
                {
                    res = new Rational(new Number(1, number, new uint[] { 0 }), new Number(1, 0, new uint[] { 2 })) % new Rational(new Number(1, 0, new uint[] { 0 }), new Number(1, 0, new uint[] { 23 }));
                    Assert.Fail();
                }
                catch (Exception ex)
                {
                    Assert.IsTrue(ex.Message.Contains(CalcErr.CALC_E_INDEFINITE.ToString()) || ex.Message.Contains("undefined"));
                }
            }
        }

        [TestMethod]
        public void TestRemainderRational()
        {
            var res = new Rational(new Number(1, 0, new uint[] { 250 }), new Number(1, 0, new uint[] { 100 })) % new Rational(89);
            Assert.AreEqual("2.5", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(new Number(1, 0, new uint[] { 3330 }), new Number(1, 0, new uint[] { 1332 })) % new Rational(1);
            Assert.AreEqual("0.5", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(new Number(1, 0, new uint[] { 12250 }), new Number(1, 0, new uint[] { 100 })) % new Rational(10);
            Assert.AreEqual("2.5", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(new Number(-1, 0, new uint[] { 12250 }), new Number(1, 0, new uint[] { 100 })) % new Rational(10);
            Assert.AreEqual("-2.5", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(new Number(-1, 0, new uint[] { 12250 }), new Number(1, 0, new uint[] { 100 })) % new Rational(-10);
            Assert.AreEqual("-2.5", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(new Number(1, 0, new uint[] { 12250 }), new Number(1, 0, new uint[] { 100 })) % new Rational(-10);
            Assert.AreEqual("2.5", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(new Number(1, 0, new uint[] { 1000 }), new Number(1, 0, new uint[] { 3 })) % new Rational(1);
            Assert.AreEqual("0.33333333", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(new Number(1, 0, new uint[] { 1000 }), new Number(1, 0, new uint[] { 3 })) % new Rational(-10);
            Assert.AreEqual("3.3333333", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(new Number(-1, 0, new uint[] { 1000 }), new Number(1, 0, new uint[] { 3 })) % new Rational(-10);
            Assert.AreEqual("-3.3333333", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(834345) % new Rational(new Number(1, 0, new uint[] { 103 }), new Number(1, 0, new uint[] { 100 }));
            Assert.AreEqual("0.71", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(834345) % new Rational(new Number(-1, 0, new uint[] { 103 }), new Number(1, 0, new uint[] { 100 }));
            Assert.AreEqual("0.71", res.ToString(10, NumberFormat.Float, 8));
            res = new Rational(-834345) % new Rational(new Number(1, 0, new uint[] { 103 }), new Number(1, 0, new uint[] { 100 }));
            Assert.AreEqual("-0.71", res.ToString(10, NumberFormat.Float, 8));
        }
    }
}
