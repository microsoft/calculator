// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium.Windows;
using System;

namespace CalculatorUITestFramework
{
    public enum CalculatorMode
    {
        StandardCalculator,
        ScientificCalculator,
        ProgrammerCalculator,
        DateCalculator,
        Currency,
        Volume,
        Length,
        Weight,
        Temperature,
        Energy,
        Area,
        Speed,
        Time,
        Power,
        Data,
        Pressure,
        Angle
    }

    public class NavigationMenu
    {
        public WindowsElement NavigationMenuButton => this.session.TryFindElementByAccessibilityId("TogglePaneButton");
        public WindowsElement NavigationMenuPane => this.session.TryFindElementByClassName("SplitViewPane");

        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;

        /// <summary>
        /// Changes the mode using the navigation menu in the UI
        /// </summary>
        /// <param name="mode">The mode to be changed to</param>
        public void ChangeCalculatorMode(CalculatorMode mode)
        {
            string modeAccessibilityId;
            switch (mode)
            {
                case CalculatorMode.StandardCalculator:
                    modeAccessibilityId =  "Standard";
                    break;
                case CalculatorMode.ScientificCalculator:
                    modeAccessibilityId = "Scientific";
                    break;
                case CalculatorMode.ProgrammerCalculator:
                    modeAccessibilityId = "Programmer";
                    break;
                case CalculatorMode.DateCalculator:
                    modeAccessibilityId = "Date";
                    break;
                case CalculatorMode.Currency:
                    modeAccessibilityId = "Currency";
                    break;
                case CalculatorMode.Volume:
                    modeAccessibilityId = "Volume";
                    break;
                case CalculatorMode.Length:
                    modeAccessibilityId = "Length";
                    break;
                case CalculatorMode.Weight:
                    modeAccessibilityId = "Weight";
                    break;
                case CalculatorMode.Temperature:
                    modeAccessibilityId = "Temperature";
                    break;
                case CalculatorMode.Energy:
                    modeAccessibilityId = "Energy";
                    break;
                case CalculatorMode.Area:
                    modeAccessibilityId = "Area";
                    break;
                case CalculatorMode.Speed:
                    modeAccessibilityId = "Speed";
                    break;
                case CalculatorMode.Time:
                    modeAccessibilityId = "Time";
                    break;
                case CalculatorMode.Power:
                    modeAccessibilityId = "Power";
                    break;
                case CalculatorMode.Data:
                    modeAccessibilityId = "Data";
                    break;
                case CalculatorMode.Pressure:
                    modeAccessibilityId = "Pressure";
                    break;
                case CalculatorMode.Angle:
                    modeAccessibilityId = "Angle";
                    break;
                default:
                    throw (new ArgumentException("The mode is not valid"));
            }

            this.NavigationMenuButton.Click();
            this.NavigationMenuPane.WaitForDisplayed();
            this.session.TryFindElementByAccessibilityId(modeAccessibilityId).Click();
        }
    }
}
