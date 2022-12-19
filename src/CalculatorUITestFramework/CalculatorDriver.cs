// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;

using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;

using System;

namespace CalculatorUITestFramework
{
    public sealed class CalculatorDriver
    {
        private const string defaultAppId = "Microsoft.WindowsCalculator.Dev_8wekyb3d8bbwe!App";

        private static CalculatorDriver instance = null;
        public static CalculatorDriver Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new CalculatorDriver();
                }
                return instance;
            }

        }

        private WinAppDriverLocalServer server;

        public WindowsDriver<WindowsElement> CalculatorSession { get; private set; }

        private CalculatorDriver()
        {
        }

        public void SetupCalculatorSession(TestContext context)
        {
            this.server = new WinAppDriverLocalServer();

            // Launch Calculator application if it is not yet launched
            if (this.CalculatorSession == null)
            {
                // Create a new  WinAppDriver session to bring up an instance of the Calculator application
                // Note: Multiple calculator windows (instances) share the same process Id
                var options = new AppiumOptions();

                if (context.Properties.Contains("AppId"))
                {
                    options.AddAdditionalCapability("app", (string)context.Properties["AppId"]);
                }
                else
                {
                    options.AddAdditionalCapability("app", defaultAppId);
                }

                options.AddAdditionalCapability("deviceName", "WindowsPC");
                this.CalculatorSession = new WindowsDriver<WindowsElement>(this.server.ServiceUrl, options);
                this.CalculatorSession.Manage().Timeouts().ImplicitWait = TimeSpan.FromSeconds(10);
                Assert.IsNotNull(this.CalculatorSession);
            }
        }

        public void TearDownCalculatorSession()
        {
            // Close the application and delete the session
            if (this.CalculatorSession != null)
            {
                this.CalculatorSession.Quit();
                this.CalculatorSession = null;
            }

            if (this.server != null)
            {
                this.server.Dispose();
                this.server = null;
            }
        }
    }
}
