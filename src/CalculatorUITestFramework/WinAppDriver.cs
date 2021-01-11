// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System;
using System.Diagnostics;

namespace CalculatorUITestFramework
{
    public sealed class WinAppDriver
    {
        private WindowsDriverLocalService windowsDriverService = null;
        public MemoryPanel MemoryPanel = new MemoryPanel();
        private const string defaultAppId = "Microsoft.WindowsCalculator.Dev_8wekyb3d8bbwe!App";
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
            this.windowsDriverService = new WindowsDriverServiceBuilder().Build();

            this.windowsDriverService.OutputDataReceived += (sender, e) =>
            {
                var outputData = e.Data?.Replace("\0", string.Empty);
                if (!String.IsNullOrEmpty(outputData))
                {
                    Console.WriteLine(outputData);
                }
            };

            this.windowsDriverService.Start();

            // Launch Calculator application if it is not yet launched
            if (this.CalculatorSession == null)
            {
                // Create a new  WinAppDriver session to bring up an instance of the Calculator application
                // Note: Multiple calculator windows (instances) share the same process Id
                var options = new AppiumOptions();

                if (context.Properties.TryGetValue("AppId", out object appId))
                {
                    options.AddAdditionalCapability("app", (string)appId);
                }
                else
                {
                    options.AddAdditionalCapability("app", defaultAppId);
                }

                options.AddAdditionalCapability("deviceName", "WindowsPC");
                this.CalculatorSession = new WindowsDriver<WindowsElement>(this.windowsDriverService.ServiceUrl, options);
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

            if (this.windowsDriverService != null)
            {
                this.windowsDriverService.Dispose();
                this.windowsDriverService = null;
            }
        }

    }
}
