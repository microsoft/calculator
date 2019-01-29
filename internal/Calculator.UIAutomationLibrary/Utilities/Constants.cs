// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace Calculator.UIAutomationLibrary
{
    public class Constants
    {
        /// <summary>
        /// The path to the certificate file.
        /// </summary>
        public const string CertificateFileName = @"Calculator.cer";

        /// <summary>
        /// The path to the appxbundle file.
        /// </summary>
        public const string PackageFileName = @"Calculator.appxbundle";

        /// <summary>
        /// The path to the appxbundle file.
        /// </summary>
        public const string VCLibsPackageFileName = @"Microsoft.VCLibs.appx";

        /// <summary>
        /// The path to the appxbundle file.
        /// </summary>
        public const string WinUIPackageFileName = @"Microsoft.UI.Xaml.appx";

        /// <summary>
        /// Name of the CoreWindow.
        /// </summary>
        public const string AppWindowName = "Calculator";

        /// <summary>
        /// Name of the process executable.
        /// </summary>
        public const string ProcessName = "Calculator.exe";

        /// <summary>
        /// The package name.
        /// </summary>
        public const string PackageName = "Microsoft.WindowsCalculator";

        /// <summary>
        /// The package family name for the app to test.
        /// </summary>
        public const string PackageFamilyName = PackageName + "_8wekyb3d8bbwe";

        /// <summary>
        /// The package App User Model Id.
        /// </summary>
        public const string PackageAppUserModelId = PackageFamilyName + "!App";

        /// <summary>
        /// AutomationId for the top level UI element.
        /// </summary>
        public const string TopLevelWindowAutomationId = "CalculatorWindow";

        /// <summary>
        /// Event fired when the first page is loaded.
        /// </summary>
        public const string AppLaunchEndETWEventName = "AppLaunchEnd";

        /// <summary>
        /// App Provider GUID for ETW Events
        /// </summary>
        public static readonly Guid CalculatorETWProviderGUID = new Guid("0905CA09-610E-401E-B650-2F212980B9E0");

        /// <summary>
        /// Event fired when a calculator mode change is complete.
        /// </summary>
        public const string AppModeChangeEndETWEventName = "ModeChangeEnd";

        /// <summary>
        /// Event fired when the History panel is opened by flyout or by changing pivot tabs.
        /// </summary>
        public const string HistoryBodyOpenedETWEventName = "HistoryBodyOpened";

        /// <summary>
        /// Event fired when the Memory panel is opened by flyout or by changing pivot tabs.
        /// </summary>
        public const string MemoryBodyOpenedETWEventName = "MemoryBodyOpened";

        /// <summary>
        /// Event fired when the About flyout control is loaded.
        /// </summary>
        public const string AboutFlyoutOpenedETWEventName = "AboutFlyoutOpened";

        /// <summary>
        /// Event fired when the Nav Bar control is opened.
        /// </summary>
        public const string NavBarOpenedETWEventName = "NavBarOpened";

        /// <summary>
        /// Margin used to click in the gutter beneath the History and Memory lists
        /// </summary>
        public const int ClickMargin = 10;
    }
}
