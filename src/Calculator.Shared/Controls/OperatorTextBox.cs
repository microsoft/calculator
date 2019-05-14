using System;
using System.Collections.Generic;
using System.Text;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace CalculatorApp.Controls
{
    partial class OperatorTextBox : TextBox
	{

		protected override void OnApplyTemplate()
		{
			this.IsEnabled = false;
			this.IsHitTestVisible = false;
			this.IsTapEnabled = false;
			this.MaxLength = 50;
			this.IsReadOnly = true;
			this.IsRightTapEnabled = false;
			this.IsTabStop = false;

			var parent = VisualTreeHelper.GetParent(this);

			ListViewItem listViewItem = null;
			ListView listView = null;

			while (parent != null)
			{
				if (listViewItem == null)
				{
					listViewItem = parent as ListViewItem;
				}

				listView = parent as ListView;
				if (listView != null)
				{
					break;
				}
				parent = VisualTreeHelper.GetParent(parent);
			}

			if (listView != null)
			{
				listViewItem.IsEnabled = false;
				listViewItem.IsHitTestVisible = false;
				listViewItem.IsTapEnabled = false;
			}

			base.OnApplyTemplate();
		}
	}
}
