// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AppResourceProvider.h"

namespace CalculatorApp
{
    namespace Common
    {
        class LocalizationStringUtil
        {
        public:
            static std::wstring GetLocalizedString(const wchar_t* pMessage, ...)
            {
                std::wstring returnString = L"";
                const UINT32 length = 1024;
                std::unique_ptr<wchar_t[]> spBuffer = std::unique_ptr<wchar_t[]>(new wchar_t[length]);
                va_list args = NULL;
                va_start(args, pMessage);
                DWORD fmtReturnVal = FormatMessage(FORMAT_MESSAGE_FROM_STRING,
                                                   pMessage, 
                                                   0,
                                                   0,
                                                   spBuffer.get(), 
                                                   length, 
                                                   &args);
                va_end(args);

                if (fmtReturnVal != 0)
                {
                    returnString = spBuffer.get();
                }

                return returnString;
            }

            template <typename... T>
            static Platform::String^ GetLocalizedNarratorAnnouncement(Platform::String^ resourceKey, Platform::String^& formatVariable, T*... params)
            {
                EnsureInitialization(resourceKey, formatVariable);
                return StringReference(GetLocalizedString(formatVariable->Data(), params...).c_str());
            }

        private:
            static void EnsureInitialization(Platform::String^ resourceKey, Platform::String^& formatVariable)
            {
                if (resourceKey == nullptr || resourceKey->IsEmpty())
                {
                    return;
                }

                // If the formatVariable already has a value, we don't need to set it again. Simply return.
                if (formatVariable != nullptr && !formatVariable->IsEmpty())
                {
                    return;
                }

                formatVariable = AppResourceProvider::GetInstance().GetResourceString(resourceKey);
            }
        };
    }
}
