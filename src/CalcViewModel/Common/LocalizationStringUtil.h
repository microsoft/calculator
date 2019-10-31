// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AppResourceProvider.h"

namespace CalculatorApp
{
    namespace Common
    {
    public
        ref class LocalizationStringUtil sealed
        {
        public:
            static Platform::String ^ GetLocalizedString(Platform::String ^ pMessage)
            {
                return GetLocalizedStringInternal(pMessage);
            }

            static Platform::String ^ GetLocalizedString(Platform::String ^ pMessage, Platform::String ^ param1)
            {
                return GetLocalizedStringInternal(pMessage, param1->Data());
            }

            static Platform::String ^ GetLocalizedString(Platform::String ^ pMessage, Platform::String ^ param1, Platform::String ^ param2)
            {
                return GetLocalizedStringInternal(pMessage, param1->Data(), param2->Data());
            }

            static Platform::String ^ GetLocalizedString(Platform::String ^ pMessage, Platform::String ^ param1, Platform::String ^ param2, Platform::String ^ param3)
            {
                return GetLocalizedStringInternal(pMessage, param1->Data(), param2->Data(), param3->Data());
            }

            static Platform::String ^ GetLocalizedString(Platform::String ^ pMessage, Platform::String ^ param1, Platform::String ^ param2, Platform::String ^ param3, Platform::String ^ param4)
            {
                return GetLocalizedStringInternal(pMessage, param1->Data(), param2->Data(), param3->Data(), param4->Data());
            }

            static Platform::String^ GetLocalizedNarratorAnnouncement(Platform::String ^ resourceKey, Platform::String^ formatVariable)
            {
                return GetLocalizedString(GetResourceValue(resourceKey));
            }

            static Platform::String^ GetLocalizedNarratorAnnouncement(Platform::String ^ resourceKey, Platform::String^ formatVariable, Platform::String^ param1)
            {
                return GetLocalizedString(GetResourceValue(resourceKey), param1);
            }

            static Platform::String^ GetLocalizedNarratorAnnouncement(Platform::String ^ resourceKey, Platform::String^ formatVariable, Platform::String^ param1, Platform::String^ param2)
            {
                return GetLocalizedString(GetResourceValue(resourceKey), param1, param2);
            }
        private:
    
            template <typename... T>
            static Platform::String^ GetLocalizedNarratorAnnouncementInternal(Platform::String ^ resourceKey, Platform::String^ formatVariable, T*... params)
            {
                return GetLocalizedString(GetResourceValue(resourceKey), params...).c_str();
            }

            template <typename... T>
            static Platform::String ^ GetLocalizedStringInternal(Platform::String ^ pMessage, T*...)
        {
            std::wstring returnString = L"";
            const UINT32 length = 1024;
            std::unique_ptr<wchar_t[]> spBuffer = std::unique_ptr<wchar_t[]>(new wchar_t[length]);
            va_list args = NULL;
            va_start(args, pMessage);
            DWORD fmtReturnVal = FormatMessage(FORMAT_MESSAGE_FROM_STRING, pMessage->Data(), 0, 0, spBuffer.get(), length, &args);
            va_end(args);

            if (fmtReturnVal != 0)
            {
                return ref new Platform::String(spBuffer.get());
            }
            else
            {
                return ref new Platform::String();
            }
        }

        private:
            static Platform::String^ GetResourceValue(Platform::String ^ resourceKey)
            {
                if (resourceKey == nullptr || resourceKey->IsEmpty())
                {
                    return L"";
                }

                return AppResourceProvider::GetInstance()->GetResourceString(resourceKey);
            }
        };
    }
}
