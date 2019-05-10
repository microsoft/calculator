using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VisualStudio.TestTools.UnitTesting.Logging;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using OpenQA.Selenium.Interactions;
using OpenQA.Selenium.Remote;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// The ApplicationBase class handles setting up the WinAppDriver session and gives access to UI elements that are always present on the UI regardless of calculator mode.
    /// </summary>
    public class ApplicationBase
    {
        // Note: append /wd/hub to the URL if you're directing the test at Appium
        private const string windowsApplicationDriverUrl = "http://127.0.0.1:4723";
        private const string calculatorAppId = "Microsoft.WindowsCalculator.Dev_8wekyb3d8bbwe!App";
        private static Process winAppDriverProcess;
        public static NavigationMenu NavigationMenu = new NavigationMenu();
        public static WindowsDriver<WindowsElement> CalculatorSession { get; private set; }
        public static WindowsElement Header
        {
            get
            {
                try
                {
                    return TryFindElementByAccessibilityId("Header");
                }
                catch
                {
                    return TryFindElementByAccessibilityId("ContentPresenter");
                }
            }
        }
        public static WindowsElement CalculatorResult => TryFindElementByAccessibilityId("CalculatorResults");

        public ApplicationBase()
        {
        }

        public ApplicationBase(WindowsDriver<WindowsElement> calculatorSession)
        {
            CalculatorSession = calculatorSession;
        }

        /// <summary>
        /// Initalizes the WinAppDriver CalculatorSession, starts the session and launches the app.
        /// </summary>
        /// <param name="context">The test context passed into VSTest.</param>
        public static void ApplicationSetup(TestContext context)
        {
            // Launches the WinAppDriver.exe in a new Process.
            winAppDriverProcess = new Process();
            winAppDriverProcess.StartInfo.FileName = @"c:\Program Files (x86)\Windows Application Driver\winappdriver.exe";
            winAppDriverProcess.Start();
            winAppDriverProcess.WaitForExit();

            // Launch Calculator application if it is not yet launched
            if (CalculatorSession == null)
            {
                // Create a new  WinAppDriver session to bring up an instance of the Calculator application
                // Note: Multiple calculator windows (instances) share the same process Id
                var options = new AppiumOptions();
                options.AddAdditionalCapability("app", calculatorAppId);
                options.AddAdditionalCapability("deviceName", "WindowsPC");
                CalculatorSession = new WindowsDriver<WindowsElement>(new Uri(windowsApplicationDriverUrl), options);
                CalculatorSession.Manage().Timeouts().ImplicitWait = TimeSpan.FromSeconds(10);
                Assert.IsNotNull(CalculatorSession);
            }
        }

        public static void TearDown()
        {
            // Close the application and delete the session
            if (CalculatorSession != null)
            {
                CalculatorSession.Quit();
                CalculatorSession = null;
            }

            // Close the WinAppDriver process.
            if (winAppDriverProcess != null)
            {
                winAppDriverProcess.Close();
            }
        }

        /// <summary>
        /// Closes the Calculator app.
        /// </summary>
        public static void CloseApp()
        {
            CalculatorSession.CloseApp();
        }

        /// <summary>
        /// Wraps the WindowsDriver.FindElementByAccessibilityId and adds retry logic for when the element cannot be found due to WinAppDriver losing the window.
        /// If FindElementByAccessibilityId fails for a different reason log the error and return null.
        /// </summary>
        public static WindowsElement TryFindElementByAccessibilityId(string id)
        {
            try
            {
                return CalculatorSession.FindElementByAccessibilityId(id);
            }
            catch (WebDriverException ex)
            {
                if (ex.Message.Contains("Currently selected window has been closed"))
                {
                    SwitchToCurrentWindowHandle();
                    return CalculatorSession.FindElementByAccessibilityId(id);
                }
                else
                {
                    Logger.LogMessage(String.Format("FindElementByAccessibilityId failed due to {0} at {1}", ex.Message, ex.StackTrace));
                    return null;
                }
            }
        }

        /// <summary>
        /// Wraps the WindowsDriver.FindElementByClassName and adds retry logic for when the element cannot be found due to WinAppDriver losing the window.
        /// If FindElementByClassName fails for a different reason log the error and return null.
        /// </summary>
        public static WindowsElement TryFindElementByClassName(string name)
        {
            try
            {
                return CalculatorSession.FindElementByClassName(name);
            }
            catch (WebDriverException ex)
            {
                if (ex.Message.Contains("Currently selected window has been closed"))
                {
                    SwitchToCurrentWindowHandle();
                    return CalculatorSession.FindElementByClassName(name);
                }
                else
                {
                    Logger.LogMessage(String.Format("FindElementByAccessibilityId failed due to {0} at {1}", ex.Message, ex.StackTrace));
                    return null;
                }
            }
        }

        /// <summary>
        /// Gets the window handles for the current CalculatorSession and switches to the first one.
        /// </summary>
        public static void SwitchToCurrentWindowHandle()
        {
            // Identify the current window handle. You can check through inspect.exe which window this is.
            var currentWindowHandle = CalculatorSession.CurrentWindowHandle;
            // Return all window handles associated with this process/application.
            // At this point hopefully you have one to pick from. Otherwise you can
            // simply iterate through them to identify the one you want.
            var allWindowHandles = CalculatorSession.WindowHandles;
            // Assuming you only have only one window entry in allWindowHandles and it is in fact the correct one,
            // switch the session to that window as follows. You can repeat this logic with any top window with the same
            // process id (any entry of allWindowHandles)
            CalculatorSession.SwitchTo().Window(allWindowHandles[0]);
        }

        /// <summary>
        /// Sends keyboard key presses to the calculator app.
        /// </summary>
        /// <param name="keysToSend"></param>
        public static void SendKeys(string keysToSend)
        {
            Header.SendKeys(keysToSend);
        }
    }

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
    }

}
