// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Calculator.UIAutomationLibrary
{
    public class ServiceHelper
    {
        public static void Start(string serviceName, int timeoutInMilliSeconds = 30000)
        {
            IntPtr hService = IntPtr.Zero;
            IntPtr hSCManager = IntPtr.Zero;
            try
            {
                hSCManager = NativeMethods.OpenSCManager(null, null, NativeMethods.SC_MANAGER_ALL_ACCESS);
                if (IntPtr.Zero == hSCManager)
                {
                    throw new Exception($"Start: Cannot Open OpenSCManager, {Marshal.GetLastWin32Error()}");
                }

                hService = NativeMethods.OpenService(hSCManager, serviceName, NativeMethods.SERVICE_ALL_ACCESS);
                if (IntPtr.Zero == hService)
                {
                    throw new Exception($"Start: Cannot Open Service, {Marshal.GetLastWin32Error()}");
                }

                NativeMethods.SERVICE_STATUS serviceStatus = new NativeMethods.SERVICE_STATUS();
                if (!NativeMethods.QueryServiceStatus(hService, ref serviceStatus))
                {
                    throw new Exception($"Start: Unable to query status of Service, {Marshal.GetLastWin32Error()}");
                }

                if (serviceStatus.dwCurrentState != (int)NativeMethods.SERVICE_STATE.SERVICE_RUNNING &&
                    serviceStatus.dwCurrentState != (int)NativeMethods.SERVICE_STATE.SERVICE_START_PENDING)
                {
                    if (!NativeMethods.StartService(hService, 0, null))
                    {
                        throw new Exception($"Start: Service cannot be started, {Marshal.GetLastWin32Error()}");
                    }
                }

                WaitForStatus(hService, NativeMethods.SERVICE_STATE.SERVICE_RUNNING, TimeSpan.FromMilliseconds(timeoutInMilliSeconds));
            }
            finally
            {
                if (IntPtr.Zero != hService)
                {
                    NativeMethods.CloseServiceHandle(hService);
                }
                if (IntPtr.Zero != hSCManager)
                {
                    NativeMethods.CloseServiceHandle(hSCManager);
                }
            }
        }

        public static void Stop(string serviceName, int timeoutInMilliSeconds = 30000)
        {
            IntPtr hSCManager = IntPtr.Zero;
            IntPtr hService = IntPtr.Zero;
            try
            {
                hSCManager = NativeMethods.OpenSCManager(null, null, NativeMethods.SC_MANAGER_ALL_ACCESS);
                if (IntPtr.Zero == hSCManager)
                {
                    throw new Exception($"Stop: Cannot Open OpenSCManager, {Marshal.GetLastWin32Error()}");
                }

                hService = NativeMethods.OpenService(hSCManager, serviceName, NativeMethods.SERVICE_ALL_ACCESS);
                if (IntPtr.Zero == hService)
                {
                    throw new Exception($"Stop: Cannot Open Service, {Marshal.GetLastWin32Error()}");
                }

                NativeMethods.SERVICE_STATUS serviceStatus = new NativeMethods.SERVICE_STATUS();
                if (!NativeMethods.QueryServiceStatus(hService, ref serviceStatus))
                {
                    throw new Exception($"Stop: Unable to query status of Service, {Marshal.GetLastWin32Error()}");
                }

                if (serviceStatus.dwCurrentState != (int)NativeMethods.SERVICE_STATE.SERVICE_STOPPED &&
                    serviceStatus.dwCurrentState != (int)NativeMethods.SERVICE_STATE.SERVICE_STOP_PENDING)
                {
                    if (!NativeMethods.ControlService(hService, NativeMethods.SERVICE_CONTROL.STOP, ref serviceStatus))
                    {
                        throw new Exception($"Stop: Service cannot be stopped, {Marshal.GetLastWin32Error()}");
                    }
                }

                WaitForStatus(hService, NativeMethods.SERVICE_STATE.SERVICE_STOPPED, TimeSpan.FromMilliseconds(timeoutInMilliSeconds));
            }
            finally
            {
                if (IntPtr.Zero != hService)
                {
                    NativeMethods.CloseServiceHandle(hService);
                }

                if (IntPtr.Zero != hSCManager)
                {
                    NativeMethods.CloseServiceHandle(hSCManager);
                }
            }
        }

        public static bool IsInstalled(string svcName)
        {
            IntPtr sc_handle = NativeMethods.OpenSCManager(null, null, NativeMethods.SC_MANAGER_ALL_ACCESS);
            if (sc_handle == IntPtr.Zero)
            {
                throw new Exception($"IsInstalled: Cannot open service manager, {Marshal.GetLastWin32Error()}");
            }

            bool bResult = false;
            IntPtr sv_handle = NativeMethods.OpenService(sc_handle, svcName, NativeMethods.SERVICE_QUERY_CONFIG);
            if (sv_handle.ToInt64() != 0)
            {
                bResult = true;
                NativeMethods.CloseServiceHandle(sv_handle);
            }
            NativeMethods.CloseServiceHandle(sc_handle);
            return bResult;
        }

        private static void WaitForStatus(IntPtr hService, NativeMethods.SERVICE_STATE desiredStatus, TimeSpan timeout)
        {
            Stopwatch swLoop = new Stopwatch();
            swLoop.Start();

            NativeMethods.SERVICE_STATUS serviceStatus = new NativeMethods.SERVICE_STATUS();

            do
            {
                Thread.Sleep(500);
                if (!NativeMethods.QueryServiceStatus(hService, ref serviceStatus))
                {
                    throw new Exception($"WaitForStatus: Unable to query status of service, {Marshal.GetLastWin32Error()}");
                }
            }
            while (serviceStatus.dwCurrentState != (int)desiredStatus && (swLoop.ElapsedMilliseconds <= timeout.TotalMilliseconds));

            if (serviceStatus.dwCurrentState != (int)desiredStatus)
            {
                throw new Exception($"WaitForStatus: Service failed to reach desired state: {desiredStatus}, current state: {serviceStatus.dwCurrentState}");
            }
        }
    }
}
