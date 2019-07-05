using CalculatorApp;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Web;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

#if HAS_UNO
using Microsoft.Extensions.Logging;
#endif

namespace CalculatorApp
{
	/// <summary>
	/// Provides application-specific behavior to supplement the default Application class.
	/// </summary>
	sealed partial class App : Application
    {
        static bool m_isAnimationEnabled = true;

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
		{
#if HAS_UNO
			ConfigureFilters(Uno.Extensions.LogExtensionPoint.AmbientLoggerFactory);
#endif

			this.InitializeComponent();
			this.Suspending += OnSuspending;

#if !__ANDROID__
			// Android needs to be initialized in the CalculatorApp.Droid.Application.ctor()

			Telemetry.AnalyticsService.Initialize();
#endif
		}

		/// <summary>
		/// Invoked when the application is launched normally by the end user.  Other entry points
		/// will be used such as when the application is launched to open a specific file.
		/// </summary>
		/// <param name="e">Details about the launch request and process.</param>
		protected override void OnLaunched(LaunchActivatedEventArgs e)
		{
#if DEBUG
            if (System.Diagnostics.Debugger.IsAttached)
            {
               // this.DebugSettings.EnableFrameRateCounter = true;
            }
#endif

#if !HAS_UNO
            var userSettings = new Windows.UI.ViewManagement.UISettings();
            m_isAnimationEnabled = userSettings.AnimationsEnabled;
#endif

#if __WASM__
			if (!string.IsNullOrWhiteSpace(e.Arguments))
			{
				var parameters = HttpUtility.ParseQueryString(e.Arguments);
				var theme = (parameters.GetValues("Theme") ?? parameters.GetValues("theme"))?.FirstOrDefault();
				if(theme != null)
				{
					Uno.UI.ApplicationHelper.RequestedCustomTheme = theme;
				}
			}
#endif


			Frame rootFrame = Windows.UI.Xaml.Window.Current.Content as Frame;

			// Do not repeat app initialization when the Window already has content,
			// just ensure that the window is active
			if (rootFrame == null)
			{
				// Create a Frame to act as the navigation context and navigate to the first page
				rootFrame = new Frame();

				rootFrame.NavigationFailed += OnNavigationFailed;

				if (e.PreviousExecutionState == ApplicationExecutionState.Terminated)
				{
					//TODO: Load state from previously suspended application
				}

				// Place the frame in the current Window
				Windows.UI.Xaml.Window.Current.Content = rootFrame;
			}

			if (e.PrelaunchActivated == false)
			{
				if (rootFrame.Content == null)
				{
					// When the navigation stack isn't restored navigate to the first page,
					// configuring the new page by passing required information as a navigation
					// parameter
					rootFrame.Navigate(typeof(MainPage), e.Arguments);
				}
				// Ensure the current window is active
				Windows.UI.Xaml.Window.Current.Activate();
			}
		}

#if HAS_UNO
		static void ConfigureFilters(ILoggerFactory factory)
		{
#if DEBUG
			factory
				.WithFilter(new FilterLoggerSettings
					{
						{ "Uno", LogLevel.Warning },
						{ "Windows", LogLevel.Warning },
						{ "Windows.UI.ViewManagement", LogLevel.Debug },
					
						// Generic Xaml events
						//{ "Windows.UI.Xaml", LogLevel.Debug },
						// { "Windows.UI.Xaml.Shapes", LogLevel.Debug },
						//{ "Windows.UI.Xaml.VisualStateGroup", LogLevel.Debug },
						//{ "Windows.UI.Xaml.StateTriggerBase", LogLevel.Debug },
						// { "Windows.UI.Xaml.UIElement", LogLevel.Debug },
						// { "Windows.UI.Xaml.Setter", LogLevel.Debug },
						   
						// Layouter specific messages
						// { "Windows.UI.Xaml.Controls", LogLevel.Debug },
						//{ "Windows.UI.Xaml.Controls.Layouter", LogLevel.Debug },
						//{ "Windows.UI.Xaml.Controls.Panel", LogLevel.Debug },
						   
						// Binding related messages
						// { "Windows.UI.Xaml.Data", LogLevel.Debug },
						// { "Windows.UI.Xamll.Data", LogLevel.Debug },
						   
						//  Binder memory references tracking
						// { "ReferenceHolder", LogLevel.Debug },
					}
				)
				.AddConsole(LogLevel.Trace);
#else
#if !__WASM__
			factory
				.AddConsole(LogLevel.Error);
#endif
#endif
		}
#endif


		/// <summary>
		/// Return True if animation is enabled by user setting.
		/// </summary>
		public static bool IsAnimationEnabled()
        {
            return m_isAnimationEnabled;
        }

        /// <summary>
        /// Return the current application view state. The value
        /// will match one of the constants in the ViewState namespace.
        /// </summary>
        public static string GetAppViewState()
        {
            String newViewState;
#if NETFX_CORE
			CoreWindow window = CoreWindow.GetForCurrentThread();
#else
			var window = Windows.UI.Xaml.Window.Current;
#endif
			if ((window.Bounds.Width >= 560) && (window.Bounds.Height >= 356))
            {
                newViewState = ViewState.DockedView;
            }
            else
            {
                newViewState = ViewState.Snap;
            }

            return newViewState;
        }

		/// <summary>
		/// Invoked when Navigation to a certain page fails
		/// </summary>
		/// <param name="sender">The Frame which failed navigation</param>
		/// <param name="e">Details about the navigation failure</param>
		void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
		{
			throw new Exception($"Failed to load Page {e.SourcePageType}: {e.Exception}");

        }

		/// <summary>
		/// Invoked when application execution is being suspended.  Application state is saved
		/// without knowing whether the application will be terminated or resumed with the contents
		/// of memory still intact.
		/// </summary>
		/// <param name="sender">The source of the suspend request.</param>
		/// <param name="e">Details about the suspend request.</param>
		private void OnSuspending(object sender, SuspendingEventArgs e)
		{
			var deferral = e.SuspendingOperation.GetDeferral();
			//TODO: Save application state and stop any background activity
			deferral.Complete();
		}
	}
}
