// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium;

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
            return Item.Text;
        }
    }
}
