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
        public GlobalCalculatorUI GlobalCalculatorUI = new GlobalCalculatorUI();
        public StandardOperatorsPanel StandardOperators = new StandardOperatorsPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public WindowsElement EnterAlwaysOnTopButton => this.session.TryFindElementByAccessibilityId("NormalAlwaysOnTopButton");
        public WindowsElement ExitAlwaysOnTopButton => this.session.TryFindElementByAccessibilityId("ExitAlwaysOnTopButton");
        public AppiumWebElement ToolTip => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("ToolTip").FindElementByClassName("TextBlock");

        public void StandardNonAoTMode()
        {
            string sourceF = this.session.PageSource;
            if (sourceF.Contains("ExitAlwaysOnTopButton"))
            {
                this.ExitAlwaysOnTopButton.Click();
                Assert.AreEqual("Standard", this.GlobalCalculatorUI.GetCalculatorHeaderText());
            }
            else
            {
                string source2 = this.session.PageSource;
                if (source2.Contains("NormalAlwaysOnTopButton"))
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
        public void StandardAoTMode()
        {
            string sourceG = this.session.PageSource;
            if (sourceG.Contains("NormalAlwaysOnTopButton"))
            {
                this.EnterAlwaysOnTopButton.Click();
                this.ExitAlwaysOnTopButton.WaitForDisplayed();
                string sourceH = WinAppDriver.Instance.CalculatorSession.PageSource;
                if (sourceH.Contains("Header"))
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
            if ((Width >= 168) && (Width <= 502) && (Height >= 320) && (Height <= 502))
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
            string sourceH = this.session.PageSource;
            if ((sourceH.Contains("Keep on top")) || (sourceH.Contains("Back to full view")))
            {
                if (sourceH.Contains("Keep on top"))
                {
                    Actions moveToAoTButton = new Actions(WinAppDriver.Instance.CalculatorSession);
                    moveToAoTButton.MoveToElement(EnterAlwaysOnTopButton);
                    moveToAoTButton.Perform();
                }
                else
                {
                    Actions moveToBTFVButton = new Actions(WinAppDriver.Instance.CalculatorSession);
                    moveToBTFVButton.MoveToElement(ExitAlwaysOnTopButton);
                    moveToBTFVButton.Perform();
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
            string sourceJ = this.session.PageSource;
            if (sourceJ.Contains("Keep on top"))
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
            string sourceK = this.session.PageSource;
            if ((sourceK.Contains("Keep on top")) && (sourceK.Contains("Header")))
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
            if ((X >= 1388) && (X <= 1722) && (Y >= 25) && (Y <= 35))
            {
                return;
            }
            else
            {
                throw new Exception("'Keep on top' window is not within range of its default position");
            }
        }
    }
}
