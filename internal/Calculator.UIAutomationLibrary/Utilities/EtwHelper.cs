// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.OneCoreUap.Test.AppModel;
using WEX.Logging.Interop;

namespace Calculator.UIAutomationLibrary
{
    public class EtwHelper
    {
        private static bool etwServiceWasInstalled = false;

        /// <summary>
        /// Installs and starts the Etw.Service so that Tests may utilize Etw Waiters.
        /// Wex.Services.exe is part of TAEF.
        /// </summary>
        public static void InstallAndStartETWService()
        {
            etwServiceWasInstalled = ServiceHelper.IsInstalled("Etw.Service");
            if (!etwServiceWasInstalled)
            {
                string wexServices = Path.Combine(TAEFHelper.GetTestDeploymentDirectory(), "Wex.Services.exe");
                if (!File.Exists(wexServices))
                {
                    throw new FileNotFoundException(wexServices);
                }

                Log.Comment("Attempting to install Etw.Service...");
                var startInfo = new ProcessStartInfo();
                startInfo.FileName = wexServices;
                startInfo.Arguments = "/install:Etw.Service";

                var process = new Process();
                process.StartInfo = startInfo;
                if (process.Start())
                {
                    process.WaitForExit();
                    Log.Comment("Completed installation of Etw.Service");
                }
                else
                {
                    throw new Exception("ETW service was not able to be installed. Process didn't start.");
                }
            }

            Log.Comment("Attempting to start Etw.Service...");
            ServiceHelper.Start("Etw.Service");
            Log.Comment("Etw.Service started");
        }

        /// <summary>
        /// Stops the Etw.Service.
        /// </summary>
        public static void StopAndRemoveETWService()
        {
            if (ServiceHelper.IsInstalled("Etw.Service"))
            {
                Log.Comment("Attempting to stop Etw.Service...");
                ServiceHelper.Stop("Etw.Service");
                Log.Comment("Etw.Service stopped");

                // if we installed the Etw.Service as part of this test we should also remove it.
                // This prevents cases where TDP is upgraded but the service tregistration is referencing the old
                // location that no longer exists.
                if (!etwServiceWasInstalled)
                {

                    string wexServices = Path.Combine(TAEFHelper.GetTestDeploymentDirectory(), "Wex.Services.exe");
                    if (!File.Exists(wexServices))
                    {
                        throw new FileNotFoundException(wexServices);
                    }

                    Log.Comment("Attempting to remove Etw.Service...");
                    var startInfo = new ProcessStartInfo();
                    startInfo.FileName = wexServices;
                    startInfo.Arguments = "/remove:Etw.Service";

                    var process = new Process();
                    process.StartInfo = startInfo;
                    if (process.Start())
                    {
                        process.WaitForExit();
                        Log.Comment("Completed removal of Etw.Service");
                    }
                    else
                    {
                        throw new Exception("ETW service could not be removed. Process didn't start.");
                    }
                }
            }
        }
    }
}
