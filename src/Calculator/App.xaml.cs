// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// App.xaml.h
// Declaration of the App class.
//

using System;
using System.Diagnostics;
using System.Threading.Tasks;

using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.Storage;
using Windows.UI.StartScreen;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

using CalculatorApp.Utils;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.Automation;

namespace CalculatorApp
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public sealed partial class App
    {
        /// <summary>
        /// Initializes the singleton application object. This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            InitializeComponent();
            NarratorNotifier.RegisterDependencyProperties();

            // TODO: MSFT 14645325: Set this directly from XAML.
            // Currently this is bugged so the property is only respected from code-behind.
            HighContrastAdjustment = ApplicationHighContrastAdjustment.None;
            Suspending += OnSuspending;
#if DEBUG
            DebugSettings.IsBindingTracingEnabled = true;
            DebugSettings.BindingFailed += (sender, args) =>
            {
                if (Debugger.IsAttached)
                {
                    string errorMessage = args.Message;
                    Debugger.Break();
                }
            };
#endif
        }

        /// <summary>
        /// Invoked when the application is launched normally by the end user. Other entry points
        /// will be used when the application is launched to open a specific file, to display
        /// search results, and so forth.
        /// </summary>
        /// <param name="e">Details about the launch request and process.</param>
        protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            NavCategoryStates.SetCurrentUser(args.User.NonRoamableId);

            // It takes time to check GraphingMode at the very first time. Warm up in a background thread.
            Task.Run(() => NavCategoryStates.IsViewModeEnabled(ViewMode.Graphing));

            OnAppLaunch(args, args.Arguments, args.PrelaunchActivated);
        }

        protected override void OnActivated(IActivatedEventArgs args)
        {
            if (args.Kind != ActivationKind.Protocol) return;
            
            if (args.IsSnapshotProtocol())
            {
                var protoArgs = (IProtocolActivatedEventArgs)args;
                OnAppLaunch(args,
                    new SnapshotLaunchArguments
                    {
                        ActivityId = protoArgs.Uri.GetActivityId(),
                        LaunchUri = protoArgs.Uri
                    },
                    false);
            }
            else
            {
                // handle any unknown protocol launch as a normal app launch.
                OnAppLaunch(args, null, false);
            }            
        }

        private void OnAppLaunch(IActivatedEventArgs args, object arguments, bool isPreLaunch)
        {
            // Uncomment the following lines to display frame-rate and per-frame CPU usage info.
            //#if DEBUG
            //    if (IsDebuggerPresent())
            //    {
            //        DebugSettings.EnableFrameRateCounter = true;
            //    }
            //#endif

            args.SplashScreen.Dismissed += async (_, __) => await SetupJumpListAsync();

            var minWindowWidth = Convert.ToSingle(Resources["AppMinWindowWidth"]);
            var minWindowHeight = Convert.ToSingle(Resources["AppMinWindowHeight"]);
            var minWindowSize = SizeHelper.FromDimensions(minWindowWidth, minWindowHeight);
            var appView = ApplicationView.GetForCurrentView();
            var localSettings = ApplicationData.Current.LocalSettings;

            // SetPreferredMinSize should always be called before Window.Activate
            appView.SetPreferredMinSize(minWindowSize);

            // For very first launch, set the size of the calc as size of the default standard mode
            if (!localSettings.Values.ContainsKey("VeryFirstLaunch"))
            {
                localSettings.Values["VeryFirstLaunch"] = false;
                appView.TryResizeView(minWindowSize); // the requested size must not be less than the min size.
            }
            else
            {
                ApplicationView.PreferredLaunchWindowingMode = ApplicationViewWindowingMode.Auto;
            }

            // Do not repeat app initialization when the Window already has content,
            // just ensure that the window is active
            var rootFrame = Window.Current.Content as Frame;
            if (rootFrame == null)
            {
                rootFrame = new Frame
                {
                    FlowDirection = LocalizationService.GetInstance().GetFlowDirection()
                };
            }

            if (isPreLaunch)
            {
                return;
            }

            // When the navigation stack isn't restored navigate to the first page,
            // configuring the new page by passing required information as a navigation
            // parameter
            if (rootFrame.Content == null && !rootFrame.Navigate(typeof(MainPage), arguments))
            {
                // We couldn't navigate to the main page, kill the app so we have a good
                // stack to debug
                throw new SystemException("6d430286-eb5d-4f8d-95d2-3d1059552968");
            }

            // Place the frame in the current Window
            Window.Current.Content = rootFrame;
            ThemeHelper.InitializeAppTheme();
            Window.Current.Activate();
        }

        private void OnSuspending(object sender, SuspendingEventArgs args)
        {
            TraceLogger.GetInstance().LogButtonUsage();
        }

        private async Task SetupJumpListAsync()
        {
            try
            {
                var calculatorOptions = NavCategoryStates.CreateCalculatorCategoryGroup();

                var jumpList = await JumpList.LoadCurrentAsync();
                jumpList.SystemGroupKind = JumpListSystemGroupKind.None;
                jumpList.Items.Clear();

                foreach (NavCategory option in calculatorOptions.Categories)
                {
                    if (!NavCategoryStates.IsViewModeEnabled(option.ViewMode))
                    {
                        continue;
                    }
                    ViewMode mode = option.ViewMode;
                    var item = JumpListItem.CreateWithArguments(((int)mode).ToString(), "ms-resource:///Resources/" + NavCategoryStates.GetNameResourceKey(mode));
                    item.Description = "ms-resource:///Resources/" + NavCategoryStates.GetNameResourceKey(mode);
                    item.Logo = new Uri("ms-appx:///Assets/" + mode + ".png");
                    jumpList.Items.Add(item);
                }

                await jumpList.SaveAsync();
            }
            catch (Exception ex)
            {
                TraceLogger.GetInstance().LogError(ViewMode.None, nameof(SetupJumpListAsync), ex.ToString());
#if DEBUG
                throw;
#endif
            }
        }
    }
}

