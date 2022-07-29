// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using Microsoft.VisualStudio.TestTools.UnitTesting;

using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using OpenQA.Selenium.Interactions;

using System.Drawing;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in Standard / AoT Mode.
    /// </summary>
    public class StandardAoTCalculatorPage
    {
        public StandardOperatorsPanel StandardOperators = new StandardOperatorsPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public WindowsElement EnterAlwaysOnTopButton => this.session.TryFindElementByAccessibilityId("NormalAlwaysOnTopButton");
        public WindowsElement ExitAlwaysOnTopButton => this.session.TryFindElementByAccessibilityId("ExitAlwaysOnTopButton");
        public AppiumWebElement ToolTip => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("ToolTip").FindElementByClassName("TextBlock");

        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;

        ///// <summary>
        ///// Navigates from AoT(Keep on top) to Standard
        ///// </summary>
        public void NavigateToStandardMode()
        {
            string source = this.session.PageSource;
            if (source.Contains("ExitAlwaysOnTopButton"))
            {
                this.ExitAlwaysOnTopButton.Click();
                Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            }
            else
            {
                source = this.session.PageSource;
                if (source.Contains("NormalAlwaysOnTopButton"))
                {
                    return;
                }
                else
                {
                    throw new NotFoundException("Could not find the 'Keep on top' button");
                }
            }
        }

        ///// <summary>
        ///// Ensures the calculator is in AoT(Keep on top) mode, and verifies that the AoT mode through the absence of the header
        ///// </summary>
        public void NavigateToStandardAoTMode()
        {
            string source = this.session.PageSource;
            if (source.Contains("NormalAlwaysOnTopButton"))
            {
                this.EnterAlwaysOnTopButton.Click();
                this.ExitAlwaysOnTopButton.WaitForDisplayed();
                source = WinAppDriver.Instance.CalculatorSession.PageSource;
                if (source.Contains("Header"))
                {
                    throw new NotFoundException("Failed to enter 'Keep on top' mode; In AoT mode, Calculator does not have header");
                }
            }
        }

        ///// <summary>
        ///// Gets the AoT ToolTip text
        ///// </summary>
        public string GetAoTToolTipText()
        {
            string source = this.session.PageSource;
            if ((source.Contains("Keep on top")) || (source.Contains("Back to full view")))
            {
                if (source.Contains("Keep on top"))
                {
                    Actions moveToAoTButton = new Actions(WinAppDriver.Instance.CalculatorSession);
                    moveToAoTButton.MoveToElement(EnterAlwaysOnTopButton);
                    moveToAoTButton.Perform();
                }
                else
                {
                    Actions moveToBackToFullViewVButton = new Actions(WinAppDriver.Instance.CalculatorSession);
                    moveToBackToFullViewVButton.MoveToElement(ExitAlwaysOnTopButton);
                    moveToBackToFullViewVButton.Perform();
                }
            }
            else
            {
                throw new NotFoundException("Could not find 'Keep on top' button or 'Back to full view' button");
            }
            return ToolTip.Text;
        }

        /// <summary>
        /// Checks in AoT (Keep on top) button is present
        /// </summary>
        public bool IsKeepOnTopButtonPresent()
        {
            string source = this.session.PageSource;
            return source.Contains("Keep on top");
        }

        /// <summary>
        /// Checks Standard calculator to see if it's in AoT (Keep on top)
        /// </summary>
        public bool IsInAlwaysOnTopMode()
        {
            string source = this.session.PageSource;
            if ((source.Contains("Keep on top")) && (source.Contains("Header")))
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        /// <summary>
        /// If the Invert button is not displayed, resize the window
        /// Two attempts are made, the the button is not found a "not found" exception is thrown
        /// </summary>
        public void ResizeAoTWindowToDisplayInvertButton()
        {
            // Put the calculator in the upper left region of the screen
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = new Point(8, 8);
            GrowWindowToShowInvertButton(WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Height);
        }

        /// <summary>
        /// Increases the height of the window until invert button is visible
        /// </summary>
        private void GrowWindowToShowInvertButton(int height)
        {
            if (height > 700)
            {
                throw new NotFoundException("Could not find the Invert Button");
            }

            if (!this.session.PageSource.Contains("invertButton"))
            {
                var width = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Width;
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = new Size(width, height);
                //give window time to render new size
                System.Threading.Thread.Sleep(10);
                GrowWindowToShowInvertButton(height + 100);
            }
        }
    }
}
