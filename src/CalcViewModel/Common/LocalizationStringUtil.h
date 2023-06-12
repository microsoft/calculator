// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AppResourceProvider.h"

namespace CalculatorApp::ViewModel
{
    namespace Common
    {
        class LocalizationStringUtilInternal
        {
        public:
            static Platform::String ^ GetLocalizedString(Platform::String ^ pMessage, ...)
            {
                std::wstring returnString = L"";
                const UINT32 length = 1024;
                std::unique_ptr<wchar_t[]> spBuffer = std::make_unique<wchar_t[]>(length);
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
        };

    public
        ref class LocalizationStringUtil sealed
        {
        public:
            static Platform::String
                ^ GetLocalizedString(Platform::String ^ pMessage)
                {
                    return LocalizationStringUtilInternal::GetLocalizedString(pMessage);
                }

                static Platform::String
                ^ GetLocalizedString(
                    Platform::String ^ pMessage,
                    Platform::String ^ param1)
                {
                    return LocalizationStringUtilInternal::GetLocalizedString(pMessage, param1->Data());
                }

                static Platform::String
                ^ GetLocalizedString(
                    Platform::String ^ pMessage,
                    Platform::String ^ param1,
                    Platform::String ^ param2)
                {
                    return LocalizationStringUtilInternal::GetLocalizedString(pMessage, param1->Data(), param2->Data());
                }

                static Platform::String
                ^ GetLocalizedString(
                    Platform::String ^ pMessage,
                    Platform::String ^ param1,
                    Platform::String ^ param2,
                    Platform::String
                        ^ param3)
                {
                    return LocalizationStringUtilInternal::GetLocalizedString(pMessage, param1->Data(), param2->Data(), param3->Data());
                }

                static Platform::String
                ^ GetLocalizedString(
                    Platform::String ^ pMessage,
                    Platform::String ^ param1,
                    Platform::String ^ param2,
                    Platform::String ^ param3,
                    Platform::String ^ param4)
                {
                    return LocalizationStringUtilInternal::GetLocalizedString(pMessage, param1->Data(), param2->Data(), param3->Data(), param4->Data());
                }

                static Platform::String
                ^ GetLocalizedString(
                    Platform::String ^ pMessage,
                    Platform::String ^ param1,
                    Platform::String ^ param2,
                    Platform::String ^ param3,
                    Platform::String ^ param4,
                    Platform::String ^ param5)
                {
                    return LocalizationStringUtilInternal::GetLocalizedString(pMessage, param1->Data(), param2->Data(), param3->Data(), param4->Data(), param5->Data());
                }
        };
    }
}
