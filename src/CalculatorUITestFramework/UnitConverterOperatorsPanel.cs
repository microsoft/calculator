// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using OpenQA.Selenium.Appium.Windows;
using System;
using System.Collections.Generic;
using System.Text;

namespace CalculatorUITestFramework
{
    public class UnitConverterOperatorsPanel
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public NumberPad NumberPad = new NumberPad();
        public WindowsElement ClearButton => this.session.TryFindElementByAccessibilityId("ClearEntryButtonPos0");
        public WindowsElement BackSpaceButton => this.session.TryFindElementByAccessibilityId("BackSpaceButtonSmall");
        public WindowsElement Units1 => this.session.TryFindElementByAccessibilityId("Units1");
        public WindowsElement Units2 => this.session.TryFindElementByAccessibilityId("Units2");
    }
}
