// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Diagnostics;
using System.IO;
using WEX.Logging.Interop;

namespace Calculator.UIAutomationLibrary
{
    public class Utilities
    {
        public static void KillExistingCalculatorProcesses()
        {
            Log.Comment("Killing any existing Calculator processes");

            foreach (var process in Process.GetProcessesByName(Path.GetFileNameWithoutExtension(Constants.ProcessName)))
            {
                try
                {
                    process.Kill();
                    Log.Comment($"Killed {process.ProcessName}, Id: {process.Id}");
                }
                catch (Exception) when (process.HasExited)
                {
                    Log.Comment($"{process.ProcessName}, Id: {process.Id} already exited.");
                }
            }
        }
    }
}
