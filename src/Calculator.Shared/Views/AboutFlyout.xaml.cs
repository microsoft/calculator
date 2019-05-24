using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Resources;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
	public sealed partial class AboutFlyout : UserControl
	{
		public AboutFlyout()
		{
			this.InitializeComponent();
			PopulateStrings();
		}

		private void PopulateStrings()
		{
			var resourceLoader = Windows.ApplicationModel.Resources.ResourceLoader.GetForCurrentView();
			Header.Text = resourceLoader.GetString("AboutButton/Content");
			var version = Package.Current.Id.Version;
			var versionNumber = $"Version {version.Major}.{version.Minor}.{version.Build}.{version.Revision}";
			AboutFlyoutVersion.Text = versionNumber;
			AboutControlCopyrightRun.Text = resourceLoader.GetString("AboutControlCopyright").Replace("%1", "2019");
		}
	}
}






