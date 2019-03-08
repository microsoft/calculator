// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Etw.Managed;
using Microsoft.OneCoreUap.Test.AppModel;
using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using Calculator.UIAutomationLibrary.Components;
using WEX.Logging.Interop;

namespace Calculator.UIAutomationLibrary
{
    /// <summary>
    /// Class that can open and close the Calculator app.
    /// </summary>
    public static class CalculatorAppLauncher
    {
        public const string CoreWindowClassName = "Windows.UI.Core.CoreWindow";

        // This doesn't actually work right now because popup will disappear
        // Bug 13713223: ContentDialog/Popup does not show up in the UIA tree when Windows.Current.Content has an AutomationName set.
        // public static readonly UICondition TopLevelWindowUICondition = UICondition.CreateFromId(Constants.TopLevelWindowAutomationId);
        public static readonly UICondition CoreWindowUICondition = UICondition.CreateFromClassName(CoreWindowClassName)
            .AndWith(UICondition.CreateFromName(Constants.AppWindowName));

        /// <summary>
        /// Launch the app
        /// </summary>
        public static CalculatorAppLfm Launch()
        {
            Log.Comment("Launching Calculator and waiting for first page load...");

            // Need to set this for the MITALite Tap~ methods to work on high DPI screens.
            UAPApp.SetTestDPIAwareness();

            // We want to be able to see any element in the tree
            Context.RawContext.Activate();

            // Set default wait timeout.
            MS.Internal.Mita.Foundation.Waiters.Waiter.DefaultTimeout = TimeSpan.FromSeconds(30);

            // Enable Mita internal logging.
            MS.Internal.Mita.Foundation.Utilities.Log.OutImplementation = (s, a) => { Log.Comment($"- [MitaLite] { string.Format(s, a) }"); };
            
            using (EtwWaiter appLaunchWaiter = new EtwWaiter(Constants.CalculatorETWProviderGUID, Constants.AppLaunchEndETWEventName))
            {
                var viewDescriptor = NavigationHelper.LaunchApplication(Constants.PackageAppUserModelId);
                appLaunchWaiter.Wait(TimeSpan.FromSeconds(30));

                Window calculatorWindow = new Window(UIObject.Root.Descendants.Find(CoreWindowUICondition));
                Debug.Assert(calculatorWindow.ClassName == CoreWindowClassName);

                // Move our window to the foreground.
                WindowHelper.SetAsForeground(calculatorWindow.GetTopLevelWindow());

                return new CalculatorAppLfm(new CalculatorAppPom(calculatorWindow), viewDescriptor);
            }
        }
    }
}
