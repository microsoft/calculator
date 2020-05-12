// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium;
using System;
using System.Collections.Generic;
using System.Text;

namespace CalculatorUITestFramework
{
    public class MemoryItem
    {
        public AppiumWebElement Item { get; }

        public MemoryItem(AppiumWebElement item)
        {
            Item = item;
        }

        public string GetValue()
        {
            return Item.FindElementByAccessibilityId("MemoryItemValue")?.Text;
        }
    }
}
