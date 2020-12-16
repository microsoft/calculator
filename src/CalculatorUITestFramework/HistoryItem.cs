// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium;
using System;
using System.Collections.Generic;
using System.Text;

namespace CalculatorUITestFramework
{
    public class HistoryItem
    {
        public AppiumWebElement Item { get; }

        public HistoryItem(AppiumWebElement item)
        {
            Item = item;
        }

        public string GetValue()
        {
            return Item.FindElementByAccessibilityId("HistoryItemValue")?.Text;
        }
        public string GetExpression()
        {
            return Item.FindElementByAccessibilityId("HistoryItemExpression")?.Text;
        }
    }
}
