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

        /// <summary>
        /// The implicit wait applied to every session. Exposed so helpers that need to poll for an
        /// element's absence can drop the wait to zero and then restore it.
        /// </summary>
        public static readonly TimeSpan ImplicitWait = TimeSpan.FromSeconds(10);

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
            server = new WinAppDriverLocalServer();

            // Launch Calculator application if it is not yet launched
            if (CalculatorSession == null)
            {
                // Create a new  WinAppDriver session to bring up an instance of the Calculator application
                // Note: Multiple calculator windows (instances) share the same process Id
                var options = new AppiumOptions();

                if (context.Properties.ContainsKey("AppId"))
                {
                    options.AddAdditionalCapability("app", (string)context.Properties["AppId"]);
                }
                else
                {
                    options.AddAdditionalCapability("app", defaultAppId);
                }

                options.AddAdditionalCapability("deviceName", "WindowsPC");
                this.CalculatorSession = new WindowsDriver<WindowsElement>(this.server.ServiceUrl, options);
                this.CalculatorSession.Manage().Timeouts().ImplicitWait = ImplicitWait;
                Assert.IsNotNull(this.CalculatorSession);
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

            if (server != null)
            {
                server.Dispose();
                server = null;
            }
        }
    }
}
