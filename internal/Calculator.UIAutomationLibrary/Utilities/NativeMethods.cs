// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Calculator.UIAutomationLibrary
{
    internal static class NativeMethods
    {
        internal const int GW_OWNER = 4;

        internal delegate bool EnumThreadWindowsCallback(IntPtr hWnd, IntPtr lParam);

        [DllImport("api-ms-win-ntuser-ie-window-l1-1-0.dll", SetLastError = true)]
        internal static extern bool EnumWindows(EnumThreadWindowsCallback callback, IntPtr extraData);

        [DllImport("api-ms-win-ntuser-ie-window-l1-1-0.dll", SetLastError = true)]
        internal static extern int GetWindowThreadProcessId(IntPtr handle, out int processId);

        [DllImport("api-ms-win-ntuser-ie-window-l1-1-0.dll", SetLastError = true)]
        internal static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);

        [DllImport("api-ms-win-ntuser-ie-window-l1-1-0.dll", SetLastError = true)]
        internal static extern int GetWindowTextLength(IntPtr hWnd);

        [DllImport("api-ms-win-ntuser-ie-window-l1-1-0.dll", SetLastError = true)]
        internal static extern bool IsWindowVisible(IntPtr hWnd);

        [DllImport("api-ms-win-ntuser-ie-window-l1-1-0.dll", SetLastError = true)]
        internal static extern int GetWindowLong(IntPtr hWnd, int nIndex);

        [DllImport("api-ms-win-ntuser-ie-window-l1-1-0.dll", SetLastError = true)]
        internal static extern IntPtr GetWindow(IntPtr hWnd, int uCmd);

        [DllImport("api-ms-win-ntuser-ie-window-l1-1-0.dll", SetLastError = true)]
        internal static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("api-ms-win-service-management-l1-1-0.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool StartService(IntPtr hService, int dwNumServiceArgs, string[] lpServiceArgVectors);

        [DllImport("api-ms-win-service-management-l1-1-0.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool ControlService(IntPtr hService, SERVICE_CONTROL dwControl, ref SERVICE_STATUS lpServiceStatus);

        [DllImport("api-ms-win-service-management-l1-1-0.dll")]
        internal static extern IntPtr OpenSCManager(string machineName, string databaseName, uint dwAccess);

        [DllImport("api-ms-win-service-management-l1-1-0.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool CloseServiceHandle(IntPtr hSCObject);

        [DllImport("api-ms-win-service-management-l1-1-0.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        internal static extern IntPtr OpenService(IntPtr hSCManager, string lpServiceName, uint dwDesiredAccess);

        [DllImport("api-ms-win-service-management-l1-1-0.dll", SetLastError = true)]
        internal static extern bool QueryServiceStatus(IntPtr hService, ref SERVICE_STATUS dwServiceStatus);

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        internal struct SERVICE_STATUS
        {
            public int dwServiceType;
            public int dwCurrentState;
            public int dwControlsAccepted;
            public int dwWin32ExitCode;
            public int dwServiceSpecificExitCode;
            public int dwCheckPoint;
            public int dwWaitHint;
        }

        internal static uint STANDARD_RIGHTS_REQUIRED = 0xF0000;

        internal static uint SC_MANAGER_CONNECT = 0x0001;
        internal static uint SC_MANAGER_CREATE_SERVICE = 0x0002;
        internal static uint SC_MANAGER_ENUMERATE_SERVICE = 0x0004;
        internal static uint SC_MANAGER_LOCK = 0x0008;
        internal static uint SC_MANAGER_QUERY_LOCK_STATUS = 0x0010;
        internal static uint SC_MANAGER_MODIFY_BOOT_CONFIG = 0x0020;
        internal static uint SC_MANAGER_ALL_ACCESS = (STANDARD_RIGHTS_REQUIRED |
                                        SC_MANAGER_CONNECT |
                                        SC_MANAGER_CREATE_SERVICE |
                                        SC_MANAGER_ENUMERATE_SERVICE |
                                        SC_MANAGER_LOCK |
                                        SC_MANAGER_QUERY_LOCK_STATUS |
                                        SC_MANAGER_MODIFY_BOOT_CONFIG);

        internal static uint SERVICE_QUERY_CONFIG = 0x0001;
        internal static uint SERVICE_CHANGE_CONFIG = 0x0002;
        internal static uint SERVICE_QUERY_STATUS = 0x0004;
        internal static uint SERVICE_ENUMERATE_DEPENDENTS = 0x0008;
        internal static uint SERVICE_START = 0x0010;
        internal static uint SERVICE_STOP = 0x0020;
        internal static uint SERVICE_PAUSE_CONTINUE = 0x0040;
        internal static uint SERVICE_INTERROGATE = 0x0080;
        internal static uint SERVICE_USER_DEFINED_CONTROL = 0x0100;
        internal static uint SERVICE_ALL_ACCESS = (STANDARD_RIGHTS_REQUIRED |
                                      SERVICE_QUERY_CONFIG |
                                      SERVICE_CHANGE_CONFIG |
                                      SERVICE_QUERY_STATUS |
                                      SERVICE_ENUMERATE_DEPENDENTS |
                                      SERVICE_START |
                                      SERVICE_STOP |
                                      SERVICE_PAUSE_CONTINUE |
                                      SERVICE_INTERROGATE |
                                      SERVICE_USER_DEFINED_CONTROL);

        [Flags]
        internal enum SERVICE_CONTROL : uint
        {
            STOP = 0x00000001,
            PAUSE = 0x00000002,
            CONTINUE = 0x00000003,
            INTERROGATE = 0x00000004,
            SHUTDOWN = 0x00000005,
            PARAMCHANGE = 0x00000006,
            NETBINDADD = 0x00000007,
            NETBINDREMOVE = 0x00000008,
            NETBINDENABLE = 0x00000009,
            NETBINDDISABLE = 0x0000000A,
            DEVICEEVENT = 0x0000000B,
            HARDWAREPROFILECHANGE = 0x0000000C,
            POWEREVENT = 0x0000000D,
            SESSIONCHANGE = 0x0000000E
        }

        internal enum SERVICE_STATE : int
        {
            SERVICE_STOPPED = 0x00000001,
            SERVICE_START_PENDING = 0x00000002,
            SERVICE_STOP_PENDING = 0x00000003,
            SERVICE_RUNNING = 0x00000004,
            SERVICE_CONTINUE_PENDING = 0x00000005,
            SERVICE_PAUSE_PENDING = 0x00000006,
            SERVICE_PAUSED = 0x00000007
        }
    }
}
