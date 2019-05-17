using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VisualStudio.TestTools.UnitTesting.Logging;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium.Windows;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
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
            Assert.Fail(String.Format("{0} was not displayed in {1} ms", element, timeout));
        }

        /// <summary>
        /// Sends keyboard key presses to the calculator app.
        /// </summary>
        /// <param name="keysToSend"></param>
        public static void SendKeys(this WindowsElement element, string keysToSend)
        {
            element.SendKeys(keysToSend);
        }
    }
}
