// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium.Windows;
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
        public StandardAoTCalculatorPage StandardAoTCalculatorPage = new StandardAoTCalculatorPage();
        public CalculatorResults CalculatorResults = new CalculatorResults();

        public void NavigateToStandardCalculator()
        {
            // Ensure that calculator is in standard mode
            this.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            Assert.IsNotNull(this.CalculatorResults.CalculatorResult);
        }
        /// <summary>
        /// Clear the Calculator display, Memory Panel and optionally the History Panel
        /// </summary>
        public void ClearAll()
        {
            this.StandardAoTCalculatorPage.StandardNonAoTMode();
            this.MemoryPanel.ResizeWindowToDiplayMemoryLabel();
            this.StandardOperators.ClearButton.Click();
            this.MemoryPanel.MemoryClear.Click();
            this.HistoryPanel.ClearHistory();
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
    }

}
