// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;

namespace Calculator.UIAutomationLibrary
{
    public static class Impersonater
    {
        public static void RunAs(RunAsUser user, Action action)
        {
            IntPtr errorInfo;
            SafeAccessTokenHandle restrictedToken;
            GetRunAsUserToken getRunAsUserToken = ResolveGetRunAsUserTokenMethod();
            Marshal.ThrowExceptionForHR(getRunAsUserToken(user, out restrictedToken, out errorInfo), errorInfo);

            WindowsIdentity.RunImpersonated(restrictedToken, action);
        }

        public static void RunAs<T>(RunAsUser user, Func<T> function)
        {
            IntPtr errorInfo;
            SafeAccessTokenHandle restrictedToken;
            GetRunAsUserToken getRunAsUserToken = ResolveGetRunAsUserTokenMethod();
            Marshal.ThrowExceptionForHR(getRunAsUserToken(user, out restrictedToken, out errorInfo), errorInfo);

            WindowsIdentity.RunImpersonated(restrictedToken, function);
        }

        // From: https://microsoft.visualstudio.com/EngSys/_git/validation.taef?path=%2Fsrc%2FTAEF%2FCommon%2FPublished%2FRunAsFromSystem.h&version=GBdevelop
        public enum RunAsUser
        {
            ElevatedUser,
            User,
            RestrictedUser,
            LowIL,
            InteractiveUser,
        };

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate int GetRunAsUserToken(RunAsUser user, out SafeAccessTokenHandle token, out IntPtr errorInfo);

        // GetRunAsUserToken is defined in a namespace so we have to do some tricks to use P/Invoke.
        // We got the actual exported method name by running dumpbin /exports TE.Common.dll
        private static GetRunAsUserToken ResolveGetRunAsUserTokenMethod()
        {
            IntPtr teCommonDll = IntPtr.Zero;
            try
            {
                teCommonDll = LoadLibrary(@"TE.Common.dll");

                IntPtr x64GetRunAsUserToken = GetProcAddress(teCommonDll, "?GetRunAsUserToken@TestExecution@WEX@@YAJW4RunAsUser@12@PEAPEAXPEAPEAUIErrorInfo@@@Z");
                if (x64GetRunAsUserToken != IntPtr.Zero)
                {
                    return Marshal.GetDelegateForFunctionPointer<GetRunAsUserToken>(x64GetRunAsUserToken);
                }
                IntPtr x86GetRunAsUserToken = GetProcAddress(teCommonDll, "?GetRunAsUserToken@TestExecution@WEX@@YGJW4RunAsUser@12@PAPAXPAPAUIErrorInfo@@@Z");
                if (x86GetRunAsUserToken != IntPtr.Zero)
                {
                    return Marshal.GetDelegateForFunctionPointer<GetRunAsUserToken>(x86GetRunAsUserToken);
                }
                IntPtr armGetRunAsUserToken = GetProcAddress(teCommonDll, "?GetRunAsUserToken@TestExecution@WEX@@YAJW4RunAsUser@12@PAPAXPAPAUIErrorInfo@@@Z");
                if (armGetRunAsUserToken != IntPtr.Zero)
                {
                    return Marshal.GetDelegateForFunctionPointer<GetRunAsUserToken>(armGetRunAsUserToken);
                }
                IntPtr arm64GetRunAsUserToken = GetProcAddress(teCommonDll, "?GetRunAsUserToken@TestExecution@WEX@@YAJW4RunAsUser@12@PEAPEAXPEAPEAUIErrorInfo@@@Z");
                if (arm64GetRunAsUserToken != IntPtr.Zero)
                {
                    return Marshal.GetDelegateForFunctionPointer<GetRunAsUserToken>(arm64GetRunAsUserToken);
                }

                throw new Exception("Unable to find the GetRunAsUserToken function in DLL 'TE.Common.dll'");
            }
            finally
            {
                if (teCommonDll != IntPtr.Zero)
                {
                    FreeLibrary(teCommonDll);
                }
            }
        }

        [DllImport("kernel32.dll")]
        public static extern IntPtr LoadLibrary(string dllToLoad);

        [DllImport("kernel32.dll")]
        public static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

        [DllImport("kernel32.dll")]
        public static extern bool FreeLibrary(IntPtr hModule);
    }
}
