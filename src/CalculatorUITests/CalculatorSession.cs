// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System;

namespace CalculatorUITests
{
    public class CalculatorSession
    {
        // Note: append /wd/hub to the URL if you're directing the test at Appium
        private const string WindowsApplicationDriverUrl = "http://127.0.0.1:4723";
        private const string CalculatorAppId = "Microsoft.WindowsCalculator.Dev_8wekyb3d8bbwe!App";
        protected static WindowsDriver<WindowsElement> session;

        public static void Setup(TestContext context)
        {
            // Launch Calculator application if it is not yet launched
            if (session == null)
            {
                // Create a new session to bring up an instance of the Calculator application
                // Note: Multiple calculator windows (instances) share the same process Id
                var options = new AppiumOptions();
                options.AddAdditionalCapability("app", CalculatorAppId);
                options.AddAdditionalCapability("deviceName", "WindowsPC");
                session = new WindowsDriver<WindowsElement>(new Uri(WindowsApplicationDriverUrl), options);
                session.Manage().Timeouts().ImplicitWait = TimeSpan.FromSeconds(180);
                Assert.IsNotNull(session);
            }
        }

        public static void TearDown()
        {
            // Close the application and delete the session
            if (session != null)
            { 
                session.Quit();
                session = null;
            }
        }
    }
}
