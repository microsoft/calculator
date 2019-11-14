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
    /// This class contains the UI automation objects and helper methods available when the Calculator is in Standard Mode.
    /// </summary>
    public class StandardCalculatorPage
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public GlobalCalculatorUI GlobalCalculatorUI = new GlobalCalculatorUI();
        public StandardOperatorsPanel StandardOperators = new StandardOperatorsPanel();
        public MemoryPanel MemoryPanel = new MemoryPanel();
        public HistoryPanel HistoryPanel = new HistoryPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public WindowsElement EnterAlwaysOnTopButton => this.session.TryFindElementByAccessibilityId("NormalAlwaysOnTopButton");
        public WindowsElement ExitAlwaysOnTopButton => this.session.TryFindElementByAccessibilityId("ExitAlwaysOnTopButton");
        public AppiumWebElement ToolTip => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("ToolTip").FindElementByClassName("TextBlock");
        public WindowsElement CalculatorAlwaysOnTopResults => this.session.TryFindElementByAccessibilityId("CalculatorAlwaysOnTopResults");

        public void NavigateToStandardCalculator()
        {
            // Ensure that calculator is in standard mode
            this.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            Assert.IsNotNull(this.GlobalCalculatorUI.CalculatorResult);
        }
        /// <summary>
        /// Clear the Calculator display, Memory Panel and optionally the History Panel
        /// </summary>
        public void ClearAll()
        {
            this.StandardNonKoTMode();
            this.MemoryPanel.ResizeWindowToDiplayMemoryLabel();
            this.StandardOperators.ClearButton.Click();
            this.MemoryPanel.MemoryClear.Click();
            this.HistoryPanel.ClearHistory();
        }
        /// <summary>
        /// Gets the text from the display control in KOT mode and removes the narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetKOTCalculatorResultText()
        {
            return this.CalculatorAlwaysOnTopResults.Text.Replace("Display is", string.Empty).Trim();
        }
        public void StandardNonKoTMode()
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
        ///// Ensures the calculator is in KOT(Keep on top) mode, and verifies that the KOT mode through the absence of the header
        ///// </summary>
        public void StandardKoTMode()
        {
            string sourceG = this.session.PageSource;
            if (sourceG.Contains("NormalAlwaysOnTopButton"))
            {
                this.EnterAlwaysOnTopButton.Click();
                string sourceH = WinAppDriver.Instance.CalculatorSession.PageSource;
                if (sourceH.Contains("Header"))
                {
                    throw new NotFoundException("Failed to enter 'Keep on top' mode; In KoT mode, Calculator does not have header");
                }
                else
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }
        /// <summary>
        /// Checks to see if the size of the Calculator window is within the KOT arange of size
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public void KOTWindowSizeWithinRange()
        {
            Size getCalculatorWindowSize = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size;
            int Width = getCalculatorWindowSize.Width;
            int Height = getCalculatorWindowSize.Height;
            if ((Width >= 168) || (Width <= 502) && (Height >= 361) || (Height<= 502))
            {
                return;
            }
            else
            {
                throw new Exception("'Keep on top' window size is too small/large");
            }
        }
        ///// <summary>
        ///// Gets the calculators window's width and height and returns it as a string. e.g. "(322, 396)"
        ///// </summary>
        public string GetCalculatorWindowSize()
        {
            Size getCalculatorWindowSize = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size;
            int Width = getCalculatorWindowSize.Width;
            int Height = getCalculatorWindowSize.Height;
            var CalculatorWindowSize = (Width, Height);
            return CalculatorWindowSize.ToString();
        }
        ///// <summary>
        ///// Gets the KOT ToolTip text
        ///// </summary>
        public string GetKOTToolTipText()
        {
            string sourceH = this.session.PageSource;
            if ((sourceH.Contains("Keep on top")) || (sourceH.Contains("Back to full view")))
            {
                if (sourceH.Contains("Keep on top"))
                {
                    Actions moveToKOTButton = new Actions(WinAppDriver.Instance.CalculatorSession);
                    moveToKOTButton.MoveToElement(EnterAlwaysOnTopButton);
                    moveToKOTButton.Perform();
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
        ///// Checks in KOT (Keep on top) button is present
        ///// </summary>
        public string GetKOTPresence()
        {
            bool KOTPresent;
            string sourceJ = this.session.PageSource;
            if (sourceJ.Contains("Keep on top"))
            {
                KOTPresent = true;
            }
            else
            {
                KOTPresent = false;
            }
            return KOTPresent.ToString();
        }
        ///// <summary>
        ///// Checks Standard calculator to see if it's in KOT (Keep on top)
        ///// </summary>
        public string KOTModeCheck()
        {
            bool InKOTMode;
            string sourceK = this.session.PageSource;
            if ((sourceK.Contains("Keep on top")) && (sourceK.Contains("Header")))
            {
                InKOTMode = false;
            }
            else
            {
                InKOTMode = true;
            }
            return InKOTMode.ToString();
        }
        ///// <summary>
        ///// Gets the calculators window's X and Y position and returns it as a string. e.g. "(0, 0)"
        ///// </summary>
        public string GetCalculatorWindowPosition()
        {
            Point getCalculatorWindowPosition = WinAppDriver.Instance.CalculatorSession.Manage().Window.Position;
            int X = getCalculatorWindowPosition.X;
            int Y = getCalculatorWindowPosition.Y;
            var CalculatorWindowPosition = (X, Y);
            return CalculatorWindowPosition.ToString();
        }
    }

}
