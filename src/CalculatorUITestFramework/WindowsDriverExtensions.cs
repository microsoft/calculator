// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting.Logging;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium.Windows;
using System.Diagnostics;
using System.Threading;

namespace CalculatorUITestFramework
{
    public static class WindowsDriverExtensions
    {
        /// <summary>
        /// Wraps the WindowsDriver.FindElementByAccessibilityId and adds retry logic for when the element cannot be found due to WinAppDriver losing the window.
        /// If FindElementByAccessibilityId fails for a different reason rethrow the error.
        /// </summary>
        public static WindowsElement TryFindElementByAccessibilityId(this WindowsDriver<WindowsElement> driver, string id)
        {
            try
            {
                return driver.FindElementByAccessibilityId(id);
            }
            catch (WebDriverException ex)
            {
                if (ex.Message.Contains("Currently selected window has been closed"))
                {
                    driver.SwitchToCurrentWindowHandle();
                    return driver.FindElementByAccessibilityId(id);
                }

                throw;
            }
        }

        /// <summary>
        /// Wraps the WindowsDriver.FindElementByClassName and adds retry logic for when the element cannot be found due to WinAppDriver losing the window.
        /// If FindElementByAccessibilityId fails for a different reason rethrow the error.
        /// </summary>
        public static WindowsElement TryFindElementByClassName(this WindowsDriver<WindowsElement> driver, string name)
        {
            try
            {
                return driver.FindElementByClassName(name);
            }
            catch (WebDriverException ex)
            {
                if (ex.Message.Contains("Currently selected window has been closed"))
                {
                    driver.SwitchToCurrentWindowHandle();
                    return driver.FindElementByClassName(name);
                }

                throw;
            }
        }

        /// <summary>
        /// Gets the window handles for the current CalculatorSession and switches to the first one.
        /// </summary>
        public static void SwitchToCurrentWindowHandle(this WindowsDriver<WindowsElement> driver)
        {
            // Identify the current window handle. You can check through inspect.exe which window this is.
            var currentWindowHandle = driver.CurrentWindowHandle;
            // Return all window handles associated with this process/application.
            // At this point hopefully you have one to pick from. Otherwise you can
            // simply iterate through them to identify the one you want.
            var allWindowHandles = driver.WindowHandles;
            // Assuming you only have only one window entry in allWindowHandles and it is in fact the correct one,
            // switch the session to that window as follows. You can repeat this logic with any top window with the same
            // process id (any entry of allWindowHandles)
            driver.SwitchTo().Window(allWindowHandles[0]);
        }

        /// <summary>
        /// Waits for an element to be created.
        /// </summary>
        /// <param name="driver">this</param>
        /// <param name="id">the automation id</param>
        /// <param name="timeout">optional timeout in ms</param>
        /// <returns>the element with the matching automation id</returns>
        public static WindowsElement WaitForElementByAccessibilityId(this WindowsDriver<WindowsElement> driver, string id, int timeout = 1000)
        {
            Stopwatch timer = new Stopwatch();
            timer.Reset();
            timer.Start();
            while (timer.ElapsedMilliseconds < timeout)
            {
                try
                {
                    var element  = driver.TryFindElementByAccessibilityId(id);
                    return element;
                }
                catch(WebDriverException ex)
                {
                    if (ex.Message.Contains("An element could not be located on the page using the given search parameters"))
                    {
                        Logger.LogMessage("Element not found. Waiting for 10ms in WaitForElementByAccessibilityId");
                    }
                    else
                    {
                        throw;
                    }
                }

                Logger.LogMessage("Waiting for 10ms in WaitForElementByAccessibilityId");
                Thread.Sleep(10);
            }
            timer.Stop();

            // one last attempt. Throws the not found exception if this fails
            return driver.TryFindElementByAccessibilityId(id);
        }
    }
}
