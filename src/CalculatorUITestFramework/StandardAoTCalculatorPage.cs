// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using OpenQA.Selenium.Interactions;
using System;
using System.Drawing;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in Standard / AoT Mode.
    /// </summary>
    public class StandardAoTCalculatorPage
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public CalculatorApp CalculatorApp = new CalculatorApp();
        public StandardOperatorsPanel StandardOperators = new StandardOperatorsPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public WindowsElement EnterAlwaysOnTopButton => this.session.TryFindElementByAccessibilityId("NormalAlwaysOnTopButton");
        public WindowsElement ExitAlwaysOnTopButton => this.session.TryFindElementByAccessibilityId("ExitAlwaysOnTopButton");
        public AppiumWebElement ToolTip => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("ToolTip").FindElementByClassName("TextBlock");

        public void NavigateToStandardMode()
        {
            string source = this.session.PageSource;
            if (source.Contains("ExitAlwaysOnTopButton"))
            {
                this.ExitAlwaysOnTopButton.Click();
                Assert.AreEqual("Standard", this.CalculatorApp.GetCalculatorHeaderText());
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
        /// <summary>
        /// Checks to see if the size of the Calculator window is within the AoT arange of size
        /// </summary>
        public void AoTWindowSizeWithinRange()
        {
            Size getCalculatorWindowSize = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size;
            int Width = getCalculatorWindowSize.Width;
            int Height = getCalculatorWindowSize.Height;
            if ((Width >= 168) && (Width <= 844) && (Height >= 320) && (Height <= 914))
            {
                return;
            }
            else
            {
                throw new Exception("'Keep on top' window size is too small/large");
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
            var ToolTipText = ToolTip.Text.ToString();
            return ToolTipText;
        }
        ///// <summary>
        ///// Checks in AoT (Keep on top) button is present
        ///// </summary>
        public string GetAoTPresence()
        {
            bool AoTPresent;
            string source = this.session.PageSource;
            if (source.Contains("Keep on top"))
            {
                AoTPresent = true;
            }
            else
            {
                AoTPresent = false;
            }
            return AoTPresent.ToString();
        }
        ///// <summary>
        ///// Checks Standard calculator to see if it's in AoT (Keep on top)
        ///// </summary>
        public string AoTModeCheck()
        {
            bool InAoTMode;
            string source = this.session.PageSource;
            if ((source.Contains("Keep on top")) && (source.Contains("Header")))
            {
                InAoTMode = false;
            }
            else
            {
                InAoTMode = true;
            }
            return InAoTMode.ToString();
        }
        /// <summary>
        /// Checks to see if the size of the Calculator window is within the AoT arange of size
        /// To-do: Find a way to get resolution and then do something like monitor quadrant location, or
        /// something like (X >= (0.8*screenWidth))... etc.
        /// </summary>
        public void AoTWindowPositionWithinRange()
        {
            Point getCalculatorWindowPosition = WinAppDriver.Instance.CalculatorSession.Manage().Window.Position;
            int X = getCalculatorWindowPosition.X;
            int Y = getCalculatorWindowPosition.Y;
            if ((X >= 1388) && (X <= 2525) && (Y >= 25) && (Y <= 65))
            {
                return;
            }
            else
            {
                throw new Exception("'Keep on top' window is not within range of its default position");
            }
        }
        /// <summary>
        /// If the Invert button is not displayed, resize the window
        /// Two attempts are made, the the button is not found a "not found" exception is thrown
        /// </summary>
        public void ResizeAoTWindowToDiplayInvertButton()
        {
            string source = this.session.PageSource;
            if (source.Contains("invertButton"))
            {
                return;
            }
            else
            {
                Size newWindowSize = new Size(502, 502);
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = newWindowSize;
                source = this.session.PageSource;
                if (source.Contains("invertButton"))
                {
                    return;
                }
                else
                {
                    Size newWindowSize2 = new Size(750, 850);
                    WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = newWindowSize2;
                }
                source = this.session.PageSource;
                if (source.Contains("invertButton"))
                {
                    return;
                }
                else
                {
                    throw new NotFoundException("Could not find the Invert Button");
                }
            }
        }
    }
}
