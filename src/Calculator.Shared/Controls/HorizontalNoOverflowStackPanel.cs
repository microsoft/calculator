// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp.Controls
{
	public partial class HorizontalNoOverflowStackPanel : Panel
	{
		// TODO UNO: DEPENDENCY_PROPERTY_OWNER(HorizontalNoOverflowStackPanel);

		protected override Size MeasureOverride(Size availableSize)
		{
			double maxHeight = 0;
			double width = 0;
			foreach (UIElement child in Children)
			{
				child.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
				maxHeight = Math.Max(maxHeight, child.DesiredSize.Height);
				width += child.DesiredSize.Width;
			}
			return new Size(Math.Min(width, availableSize.Width), Math.Min(availableSize.Height, maxHeight));
		}

		protected virtual bool ShouldPrioritizeLastItem()
		{
			return false;
		}

		protected override Size ArrangeOverride(Size finalSize)
		{
			if (Children.Count == 0)
			{
				return finalSize;
			}

			double posX = 0;
			var lastChild = (UIElement)Children.Last();
			double lastChildWidth = 0;
			if (Children.Count > 2 && ShouldPrioritizeLastItem())
			{
				lastChildWidth = lastChild.DesiredSize.Width;
			}
			foreach (UIElement item in Children)
			{
				var widthAvailable = finalSize.Width - posX;
				if (item != lastChild)
				{
					widthAvailable -= lastChildWidth;
				}
				double itemWidth = item.DesiredSize.Width;
				if (widthAvailable > 0 && itemWidth <= widthAvailable)
				{
					// stack the items horizontally (left to right)
					item.Arrange(new Rect(posX, 0, itemWidth, finalSize.Height));
					posX += item.RenderSize.Width;
				}
				else
				{
					// Not display the item
					item.Arrange(new Rect(0, 0, 0, 0));
				}
			}
			return finalSize;
		}
	};
}
