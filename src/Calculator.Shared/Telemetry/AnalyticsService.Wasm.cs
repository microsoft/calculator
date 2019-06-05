#if __WASM__
using System;
using System.Collections.Generic;
using System.Text;
using Uno.Foundation;

namespace CalculatorApp.Telemetry
{
	internal static partial class AnalyticsService
	{
		static partial void InitializePartial()
		{
			WebAssemblyRuntime.InvokeJS("Uno.UI.Demo.Analytics.reportPageView('main');");
		}

		static partial void TrackViewPartial(string viewName)
		{
			WebAssemblyRuntime.InvokeJS($"Uno.UI.Demo.Analytics.reportPageView('{viewName}');");
		}
	}
}
#endif
