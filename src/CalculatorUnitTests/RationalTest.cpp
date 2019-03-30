// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include "Header Files/Rational.h"
#include "Header Files/RationalMath.h"

using namespace CalcEngine;
using namespace CalcEngine::RationalMath;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorManagerTest
{
    TEST_CLASS(RationalTest)
    {
    public:
        TEST_CLASS_INITIALIZE(CommonSetup)
        {
            ChangeConstants(10, 128);
        }
        TEST_METHOD(ModuloTest)
        {
            Rational rat25(25);
            Rational ratminus25(-25);
            Rational rat4(4);
            Rational ratminus4(-4);
            Rational res = Mod(rat25, rat4);
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"1");
            res = Mod(rat25, ratminus4);
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"-3");
            res = Mod(ratminus25, ratminus4);
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"-1");
            res = Mod(ratminus25, rat4);
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"3");

            res = Mod(Rational(426), Rational(56478));
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"426");
            res = Mod(Rational(56478), Rational(426));
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"246");
            res = Mod(Rational(-643), Rational(8756));
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"8113");
            res = Mod(Rational(643), Rational(-8756));
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"-8113");
            res = Mod(Rational(-643), Rational(-8756));
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"-643");

            //Test with Zero
            res = Mod(Rational(343654332), Rational(0));
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"343654332");
        }

        TEST_METHOD(RemainderTest)
        {
            Rational rat25(25);
            Rational ratminus25(-25);
            Rational rat4(4);
            Rational ratminus4(-4);
            Rational res = rat25 % rat4;
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"1");
            res = rat25 % ratminus4;
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"1");
            res = ratminus25 % ratminus4;
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"-1");
            res = ratminus25 % rat4;
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"-1");

            res = Rational(426) % Rational(56478);
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"426");
            res = Rational(56478) % Rational(426);
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"246");
            res = Rational(-643) % Rational(8756);
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"-643");
            res = Rational(643) % Rational(-8756);
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"643");
            res = Rational(-643) % Rational(-8756);
            VERIFY_ARE_EQUAL(res.ToString(10, FMT_FLOAT, 128), L"-643");

            //Test with Zero
            try
            {
                res = Rational(343654332) % Rational(0);
                Assert::Fail();
            }
            catch (uint32_t t)
            {
                if (t != CALC_E_INDEFINITE)
                {
                    Assert::Fail();
                }
            }
            catch (...)
            {
                Assert::Fail();
            }
        }
    };
}
