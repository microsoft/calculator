// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace Calculator.UIAutomationLibrary
{
    public class PerfConstants
    {
        /// <summary>
        /// Path where the regions, wprprofiles and wpaprofiles will be deployed to by the spkg.
        /// </summary>
        public const string ConfigDirectory = @"Config\";

        /// <summary>
        /// Our FunGates source, where we can view test results.
        /// </summary>
        public const string FunGatesSource =
#if DEBUG
            "TestSite";
#else
            "Utility Apps Performance Tests";
#endif

        /// <summary>
        /// The Windows Performance Recorder profile. These strings must have the config directory prefix.
        /// For use with the WPRProfileFile test attribute.
        /// </summary>
        public const string AppLifecycleWPRProfile = ConfigDirectory + "AppLifecycle.Profile.wprp";

        /// <summary>
        /// The regions of interest file that contains the events we are interested in measuring.
        /// </summary>
        public const string AppLifecycleRegions = ConfigDirectory + "AppLifecycle.regions.xml";
        
        /// <summary>
        /// These are uses with the DataSource test property to specify iteration info.
        /// </summary>
        public const string AppLifecycleIterationsSource = "Table:" + ConfigDirectory + "AppLifecycle.Iterations.xml#PerformanceConfigurations";
    }
}
