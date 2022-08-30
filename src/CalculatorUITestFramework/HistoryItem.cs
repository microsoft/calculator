// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium;

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
            var equalSignIndex = Item.Text.IndexOf("=");
            return Item.Text[(equalSignIndex + 1)..].Trim();
        }
        public string GetExpression()
        {
            var equalSignIndex = Item.Text.IndexOf("=");
            return Item.Text[..(equalSignIndex + 1)].Trim();
        }
    }
}
