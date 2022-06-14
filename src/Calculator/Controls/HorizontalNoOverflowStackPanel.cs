// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// HorizontalNoOverflowStackPanel.h
// Declaration of the HorizontalNoOverflowStackPanel class
//

using System;

using Windows.Foundation;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;

namespace CalculatorApp
{
    namespace Controls
    {
        public class HorizontalNoOverflowStackPanel : Windows.UI.Xaml.Controls.Panel
        {
            // Prioritize the last item over all other items (except the first one)
            internal HorizontalNoOverflowStackPanel()
            { }

            protected override Size MeasureOverride(Size availableSize)
            {
                float maxHeight = 0;
                float width = 0;
                foreach (var child in Children)
                {
                    child.Measure(new Size(float.PositiveInfinity, float.PositiveInfinity));
                    maxHeight = (float)Math.Max(maxHeight, child.DesiredSize.Height);
                    width += (float)child.DesiredSize.Width;
                }
                return new Size(Math.Min(width, availableSize.Width), Math.Min(availableSize.Height, maxHeight));
            }

            protected override Size ArrangeOverride(Size finalSize)
            {
                if (Children.Count == 0)
                {
                    return finalSize;
                }

                float posX = 0;
                var lastChild = Children[Children.Count - 1];
                float lastChildWidth = 0;
                if (Children.Count > 2 && ShouldPrioritizeLastItem())
                {
                    lastChildWidth = (float)lastChild.DesiredSize.Width;
                }
                foreach (var item in Children)
                {
                    var widthAvailable = finalSize.Width - posX;
                    if (item != lastChild)
                    {
                        widthAvailable -= lastChildWidth;
                    }
                    float itemWidth = (float)item.DesiredSize.Width;
                    if (widthAvailable > 0 && itemWidth <= widthAvailable)
                    {
                        // stack the items horizontally (left to right)
                        item.Arrange(new Rect(posX, 0, itemWidth, finalSize.Height));
                        AutomationProperties.SetAccessibilityView(item, AccessibilityView.Content);
                        posX += (float)item.RenderSize.Width;
                    }
                    else
                    {
                        // Not display the item
                        item.Arrange(new Rect(0, 0, 0, 0));
                        AutomationProperties.SetAccessibilityView(item, AccessibilityView.Raw);
                    }
                }
                return finalSize;
            }

            protected virtual bool ShouldPrioritizeLastItem()
            {
                return false;
            }
        }
    }
}

