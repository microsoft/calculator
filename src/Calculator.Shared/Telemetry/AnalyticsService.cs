using System;
using System.Collections.Generic;
using System.Text;

namespace CalculatorApp.Telemetry
{
	internal static partial class AnalyticsService
	{
		public static void Initialize()
			=> InitializePartial();

		public static void TrackView(string viewName)
			=> TrackViewPartial(viewName);

		static partial void TrackViewPartial(string viewName);

		static partial void InitializePartial();
	}
}
