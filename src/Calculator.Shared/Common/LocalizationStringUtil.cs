// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalculatorApp
{
    namespace Common
    {
        public static class LocalizationStringUtil
        {
            public static string GetLocalizedString(string pMessage, params object[] args)
            {
                // UNO TODO

                //string returnString = "";
                //const uint length = 1024;
                //std.unique_ptr<wchar_t[]> spBuffer = std.unique_ptr<wchar_t[]>(new wchar_t[length]);
                //va_list args = NULL;
                //va_start(args, pMessage);
                //int fmtReturnVal = FormatMessage(FORMAT_MESSAGE_FROM_STRING, pMessage, 0, 0, spBuffer.get(), length, &args);
                //va_end(args);

                //if (fmtReturnVal != 0)
                //{
                //    returnString = spBuffer.get();
                //}

                //return returnString;

                return $"[{pMessage}]";
            }

            public static string GetLocalizedNarratorAnnouncement(string resourceKey, string formatVariable, params object[] args)
            {
                EnsureInitialization(resourceKey, formatVariable);
                return GetLocalizedString(formatVariable, args);
            }

            static void EnsureInitialization(string resourceKey, string formatVariable)
            {
                if (resourceKey == null || string.IsNullOrEmpty(resourceKey))
                {
                    return;
                }

                // If the formatVariable already has a value, we don't need to set it again. Simply return.
                if (formatVariable != null && !string.IsNullOrEmpty(formatVariable))
                {
                    return;
                }

                formatVariable = AppResourceProvider.GetInstance().GetResourceString(resourceKey);
            }
        }
    }
}
