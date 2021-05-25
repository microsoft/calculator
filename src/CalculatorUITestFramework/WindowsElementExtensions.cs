// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VisualStudio.TestTools.UnitTesting.Logging;
using OpenQA.Selenium.Appium.Windows;
using System;
using System.Diagnostics;
using System.Threading;

namespace CalculatorUITestFramework
{
    public static class WindowsElementExtensions
    {
        /// <summary>
        /// Waits for an element to be displayed until the timeout is reached.
        /// </summary>
        /// <param name="element">WindowsElement in the Calculator application.</param>
        /// <param name="timeout">Timeout in ms.</param>
        public static void WaitForDisplayed(this WindowsElement element, int timeout = 2000)
        {
            Stopwatch timer = new Stopwatch();
            timer.Reset();
            timer.Start();
            while (timer.ElapsedMilliseconds < timeout)
            {
                if (element.Displayed)
                {
                    timer.Stop();
                    return;
                }
                Logger.LogMessage("Waiting for 10ms in WaitForDisplayed");
                Thread.Sleep(10);
            }
            timer.Stop();
            Assert.Fail($"{element} was not displayed in {timeout} ms");
        }
    }
}
