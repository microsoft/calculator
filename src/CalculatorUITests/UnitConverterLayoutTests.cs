// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using CalculatorUITestFramework;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using OpenQA.Selenium.Appium.Windows;

using System;
using System.Diagnostics;
using System.Drawing;
using System.Threading;

namespace CalculatorUITests
{
    [TestClass]
    public class UnitConverterLayoutTests
    {
        private static readonly UnitConverterPage page = new UnitConverterPage();

        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            CalculatorDriver.Instance.SetupCalculatorSession(context);
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            CalculatorDriver.Instance.TearDownCalculatorSession();
        }

        [TestInitialize]
        public void TestInitialize()
        {
            page.OpenConverter();
            page.UnitPicker.SelectCategoryAndFirstUnit(page.UnitConverterOperators.Units1, "Length");
            page.SelectUnits1("Centimeters");
            page.SelectUnits2("Inches");
            page.ClearAll();
        }

        [TestCleanup]
        public void TestCleanup()
        {
            page.ClearAll();
        }

        [TestMethod]
        [Priority(0)]
        public void BottomValueAtMinimumSizeDoesNotOverlapSwapButton()
        {
            var window = CalculatorDriver.Instance.CalculatorSession.Manage().Window;
            var originalSize = window.Size;
            try
            {
                window.Size = new Size(320, 500);
                WaitForStableBounds(
                    page.UnitConverterResults.CalculationResult2,
                    page.UnitConverterOperators.SwapUnitsButton);

                page.UnitConverterResults.CalculationResult2.Click();
                page.ClearAll();
                for (int digit = 0; digit < 11; digit++)
                {
                    page.UnitConverterOperators.NumberPad.Num9Button.Click();
                }

                WaitForResultText("99999999999");
                var bounds = WaitForStableBounds(
                    page.UnitConverterResults.CalculationResult2,
                    page.UnitConverterOperators.SwapUnitsButton);

                Assert.IsFalse(
                    Intersects(bounds[0], bounds[1]),
                    $"Bottom value bounds {bounds[0]} overlap swap button bounds {bounds[1]}.");
            }
            finally
            {
                window.Size = originalSize;
            }
        }

        private static Rectangle[] WaitForStableBounds(params WindowsElement[] elements)
        {
            var timer = Stopwatch.StartNew();
            Rectangle[] previous = null;
            int stableSamples = 0;

            while (timer.Elapsed < CalculatorDriver.ImplicitWait)
            {
                var current = new Rectangle[elements.Length];
                for (int index = 0; index < elements.Length; index++)
                {
                    current[index] = elements[index].Rect;
                }

                if (previous != null && BoundsEqual(previous, current))
                {
                    stableSamples++;
                    if (stableSamples == 3)
                    {
                        return current;
                    }
                }
                else
                {
                    stableSamples = 0;
                    previous = current;
                }

                Thread.Sleep(50);
            }

            Assert.Fail($"Converter bounds did not stabilize within {CalculatorDriver.ImplicitWait.TotalSeconds} seconds.");
            return null;
        }

        private static void WaitForResultText(string expected)
        {
            var timer = Stopwatch.StartNew();
            while (timer.Elapsed < CalculatorDriver.ImplicitWait)
            {
                if (page.UnitConverterResults.GetCalculationResult2Text() == expected)
                {
                    return;
                }

                Thread.Sleep(50);
            }

            Assert.Fail(
                $"Bottom converter result did not become {expected} within "
                + $"{CalculatorDriver.ImplicitWait.TotalSeconds} seconds.");
        }

        private static bool BoundsEqual(Rectangle[] left, Rectangle[] right)
        {
            for (int index = 0; index < left.Length; index++)
            {
                if (left[index] != right[index])
                {
                    return false;
                }
            }

            return true;
        }

        private static bool Intersects(Rectangle left, Rectangle right)
        {
            return left.Right > right.Left
                && right.Right > left.Left
                && left.Bottom > right.Top
                && right.Bottom > left.Top;
        }
    }
}
