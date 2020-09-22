// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include "Header Files/Rational.h"
#include "Header Files/RationalMath.h"

using namespace CalcEngine;
using namespace CalcEngine::RationalMath;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorEngineTests
{
    TEST_CLASS(RationalTest){ public: TEST_CLASS_INITIALIZE(CommonSetup){ ChangeConstants(10, 128);
}

TEST_METHOD(TestModuloOperandsNotModified)
{
    // Verify results but also check that operands are not modified
    Rational rat25(25);
    Rational ratminus25(-25);
    Rational rat4(4);
    Rational ratminus4(-4);
    Rational res = Mod(rat25, rat4);
    VERIFY_ARE_EQUAL(res, 1);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(rat4, 4);
    res = Mod(rat25, ratminus4);
    VERIFY_ARE_EQUAL(res, -3);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = Mod(ratminus25, ratminus4);
    VERIFY_ARE_EQUAL(res, -1);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = Mod(ratminus25, rat4);
    VERIFY_ARE_EQUAL(res, 3);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(rat4, 4);
}

TEST_METHOD(TestModuloInteger)
{
    // Check with integers
    auto res = Mod(Rational(426), Rational(56478));
    VERIFY_ARE_EQUAL(res, 426);
    res = Mod(Rational(56478), Rational(426));
    VERIFY_ARE_EQUAL(res, 246);
    res = Mod(Rational(-643), Rational(8756));
    VERIFY_ARE_EQUAL(res, 8113);
    res = Mod(Rational(643), Rational(-8756));
    VERIFY_ARE_EQUAL(res, -8113);
    res = Mod(Rational(-643), Rational(-8756));
    VERIFY_ARE_EQUAL(res, -643);
    res = Mod(Rational(1000), Rational(250));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(1000), Rational(-250));
    VERIFY_ARE_EQUAL(res, 0);
}

TEST_METHOD(TestModuloZero)
{
    // Test with Zero
    auto res = Mod(Rational(343654332), Rational(0));
    VERIFY_ARE_EQUAL(res, 343654332);
    res = Mod(Rational(0), Rational(8756));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(0), Rational(-242));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(0), Rational(0));
    VERIFY_ARE_EQUAL(res, 0);
    res = Mod(Rational(Number(1, 0, { 23242 }), Number(1, 0, { 2 })), Rational(Number(1, 0, { 0 }), Number(1, 0, { 23 })));
    VERIFY_ARE_EQUAL(res, 11621);
}

TEST_METHOD(TestModuloRational)
{
    // Test with rational numbers
    auto res = Mod(Rational(Number(1, 0, { 250 }), Number(1, 0, { 100 })), Rational(89));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Mod(Rational(Number(1, 0, { 3330 }), Number(1, 0, { 1332 })), Rational(1));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.5");
    res = Mod(Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Mod(Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"7.5");
    res = Mod(Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(-10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-2.5");
    res = Mod(Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })), Rational(-10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-7.5");
    res = Mod(Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })), Rational(1));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.33333333");
    res = Mod(Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })), Rational(-10));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-6.6666667");
    res = Mod(Rational(834345), Rational(Number(1, 0, { 103 }), Number(1, 0, { 100 })));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.71");
    res = Mod(Rational(834345), Rational(Number(-1, 0, { 103 }), Number(1, 0, { 100 })));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-0.32");
}

TEST_METHOD(TestRemainderOperandsNotModified)
{
    // Verify results but also check that operands are not modified
    Rational rat25(25);
    Rational ratminus25(-25);
    Rational rat4(4);
    Rational ratminus4(-4);
    Rational res = rat25 % rat4;
    VERIFY_ARE_EQUAL(res, 1);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(rat4, 4);
    res = rat25 % ratminus4;
    VERIFY_ARE_EQUAL(res, 1);
    VERIFY_ARE_EQUAL(rat25, 25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = ratminus25 % ratminus4;
    VERIFY_ARE_EQUAL(res, -1);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(ratminus4, -4);
    res = ratminus25 % rat4;
    VERIFY_ARE_EQUAL(res, -1);
    VERIFY_ARE_EQUAL(ratminus25, -25);
    VERIFY_ARE_EQUAL(rat4, 4);
}

TEST_METHOD(TestRemainderInteger)
{
    // Check with integers
    auto res = Rational(426) % Rational(56478);
    VERIFY_ARE_EQUAL(res, 426);
    res = Rational(56478) % Rational(426);
    VERIFY_ARE_EQUAL(res, 246);
    res = Rational(-643) % Rational(8756);
    VERIFY_ARE_EQUAL(res, -643);
    res = Rational(643) % Rational(-8756);
    VERIFY_ARE_EQUAL(res, 643);
    res = Rational(-643) % Rational(-8756);
    VERIFY_ARE_EQUAL(res, -643);
    res = Rational(-124) % Rational(-124);
    VERIFY_ARE_EQUAL(res, 0);
    res = Rational(24) % Rational(24);
    VERIFY_ARE_EQUAL(res, 0);
}

TEST_METHOD(TestRemainderZero)
{
    // Test with Zero
    auto res = Rational(0) % Rational(3654);
    VERIFY_ARE_EQUAL(res, 0);
    res = Rational(0) % Rational(-242);
    VERIFY_ARE_EQUAL(res, 0);
    for (auto number : { 343654332, 0, -23423 })
    {
        try
        {
            res = Rational(number) % Rational(0);
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

        try
        {
            res = Rational(Number(1, number, { 0 }), Number(1, 0, { 2 })) % Rational(Number(1, 0, { 0 }), Number(1, 0, { 23 }));
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
}

TEST_METHOD(TestRemainderRational)
{
    // Test with rational numbers
    auto res = Rational(Number(1, 0, { 250 }), Number(1, 0, { 100 })) % Rational(89);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Rational(Number(1, 0, { 3330 }), Number(1, 0, { 1332 })) % Rational(1);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.5");
    res = Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-2.5");
    res = Rational(Number(-1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-2.5");
    res = Rational(Number(1, 0, { 12250 }), Number(1, 0, { 100 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"2.5");
    res = Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })) % Rational(1);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.33333333");
    res = Rational(Number(1, 0, { 1000 }), Number(1, 0, { 3 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"3.3333333");
    res = Rational(Number(-1, 0, { 1000 }), Number(1, 0, { 3 })) % Rational(-10);
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-3.3333333");
    res = Rational(834345) % Rational(Number(1, 0, { 103 }), Number(1, 0, { 100 }));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.71");
    res = Rational(834345) % Rational(Number(-1, 0, { 103 }), Number(1, 0, { 100 }));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"0.71");
    res = Rational(-834345) % Rational(Number(1, 0, { 103 }), Number(1, 0, { 100 }));
    VERIFY_ARE_EQUAL(res.ToString(10, NumberFormat::Float, 8), L"-0.71");
}
}
;
}
