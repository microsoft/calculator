// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices;

namespace CalculatorApp.ViewModel.Common
{
    public static class LocalizationStringUtil
    {
        /// <summary>
        /// Returns the message as-is (no parameters to substitute).
        /// </summary>
        public static string GetLocalizedString(string message)
        {
            return FormatMessageFromString(message);
        }

        /// <summary>
        /// Formats a message string using FormatMessage-style %1 placeholders.
        /// </summary>
        public static string GetLocalizedString(string message, string param1)
        {
            return FormatMessageFromString(message, param1);
        }

        public static string GetLocalizedString(string message, string param1, string param2)
        {
            return FormatMessageFromString(message, param1, param2);
        }

        public static string GetLocalizedString(string message, string param1, string param2, string param3)
        {
            return FormatMessageFromString(message, param1, param2, param3);
        }

        public static string GetLocalizedString(string message, string param1, string param2, string param3, string param4)
        {
            return FormatMessageFromString(message, param1, param2, param3, param4);
        }

        public static string GetLocalizedString(string message, string param1, string param2, string param3, string param4, string param5)
        {
            return FormatMessageFromString(message, param1, param2, param3, param4, param5);
        }

        private static string FormatMessageFromString(string message, params string[] args)
        {
            if (string.IsNullOrEmpty(message))
            {
                return string.Empty;
            }

            // Use Win32 FormatMessage with FORMAT_MESSAGE_FROM_STRING to handle %1, %2, etc.
            // This matches the original C++ behavior that uses FormatMessage.
            const uint FORMAT_MESSAGE_FROM_STRING = 0x00000400;
            const uint FORMAT_MESSAGE_ARGUMENT_ARRAY = 0x00002000;

            IntPtr[] argPtrs = new IntPtr[args.Length];
            try
            {
                for (int i = 0; i < args.Length; i++)
                {
                    argPtrs[i] = Marshal.StringToHGlobalUni(args[i]);
                }

                IntPtr argsPtr = IntPtr.Zero;
                if (args.Length > 0)
                {
                    argsPtr = Marshal.AllocHGlobal(IntPtr.Size * args.Length);
                    for (int i = 0; i < args.Length; i++)
                    {
                        Marshal.WriteIntPtr(argsPtr, i * IntPtr.Size, argPtrs[i]);
                    }
                }

                try
                {
                    char[] buffer = new char[1024];
                    uint result = FormatMessageW(
                        FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                        message,
                        0,
                        0,
                        buffer,
                        (uint)buffer.Length,
                        argsPtr);

                    if (result != 0)
                    {
                        return new string(buffer, 0, (int)result);
                    }
                    return string.Empty;
                }
                finally
                {
                    if (argsPtr != IntPtr.Zero)
                    {
                        Marshal.FreeHGlobal(argsPtr);
                    }
                }
            }
            finally
            {
                for (int i = 0; i < argPtrs.Length; i++)
                {
                    if (argPtrs[i] != IntPtr.Zero)
                    {
                        Marshal.FreeHGlobal(argPtrs[i]);
                    }
                }
            }
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern uint FormatMessageW(
            uint dwFlags,
            [MarshalAs(UnmanagedType.LPWStr)] string lpSource,
            uint dwMessageId,
            uint dwLanguageId,
            [Out] char[] lpBuffer,
            uint nSize,
            IntPtr arguments);
    }
}
