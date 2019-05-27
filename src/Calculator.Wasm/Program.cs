using CalculatorApp;
using Microsoft.Extensions.Logging;
using System;
using Uno.Extensions;
using Windows.UI.Xaml;
using Uno.UI;

namespace WindowsCalculator.Wasm
{
	public class Program
	{
		private static App _app;

		static void Main(string[] args)
		{
			Console.WriteLine("Program.Main");

			FeatureConfiguration.UIElement.AssignDOMXamlName = true;


			Windows.UI.Xaml.Application.Start(_ => _app = new App());
		}
    }
}
