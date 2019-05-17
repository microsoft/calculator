using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System;
using System.Collections.Generic;
using System.Text;

namespace CalculatorUITestFramework
{
    public sealed class WinAppDriver
    {
        // Note: append /wd/hub to the URL if you're directing the test at Appium
        private const string windowsApplicationDriverUrl = "http://127.0.0.1:4723";
        private const string calculatorAppId = "Microsoft.WindowsCalculator.Dev_8wekyb3d8bbwe!App";
        private static WinAppDriver instance = null;
        public static WinAppDriver Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new WinAppDriver();
                }
                return instance;
            }

        }

        public WindowsDriver<WindowsElement> CalculatorSession { get; private set; }


        private WinAppDriver()
        {
        }

        public void SetupCalculatorSession(TestContext context)
        {
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

        public void TearDownCalculatorSession()
        {
            // Close the application and delete the session
            if (CalculatorSession != null)
            {
                CalculatorSession.Quit();
                CalculatorSession = null;
            }
        }


    }
}
