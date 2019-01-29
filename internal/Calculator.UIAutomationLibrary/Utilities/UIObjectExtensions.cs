// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Linq;
using System.Windows.Automation;
using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;
using MS.Internal.Mita.Foundation.Waiters;
using WEX.Logging.Interop;

namespace Calculator.UIAutomationLibrary
{
    public static class UIObjectExtensions
    {
        private const string NamePropertyName = "Name";
        private static UICondition CoreWindowCondition = UICondition.CreateFromClassName("Windows.UI.Core.CoreWindow");

        public static Window GetParentCoreWindow(this UIObject uiObject)
        {
            if (uiObject.Matches(CoreWindowCondition))
            {
                return new Window(uiObject);
            }

            return new Window(uiObject.Ancestors.Find(CoreWindowCondition));
        }

        public static Window GetTopLevelWindow(this Window window)
        {
            var node = window;
            while (node != null && node.Parent != null && node.Parent.ControlType == ControlType.Window)
            {
                node = new Window(node.Parent);
            }
            return node;
        }

        public static void VerifyParentTreeStructure(this UIObject uiObject)
        {
            var node = uiObject;
            while (node != null && node.Parent != null)
            {
                if (!node.Parent.Children.Contains(node))
                {
                    Log.Comment($"- [VerifyingTree] {node} specifies {node.Parent} as parent but is not part of its children.");
                }
                if (!node.Parent.Descendants.Contains(node))
                {
                    Log.Comment($"- [VerifyingTree] {node} specifies {node.Parent} as parent but is not part of its descendants.");
                }
                node = node.Parent;
            }
        }

        public static bool DoesDescendantExist(this UIObject uiObject, string automationId)
        {
            UIObject temp;
            return uiObject.Descendants.TryFind(automationId, out temp);
        }

        public static PropertyChangedEventWaiter GetNameChangedWaiter(this TextBlock textBlock)
        {
            return new PropertyChangedEventWaiter(textBlock, UIProperty.Get(NamePropertyName));
        }
    }
}
