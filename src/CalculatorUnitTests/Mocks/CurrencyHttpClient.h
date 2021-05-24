// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/DataLoaders/ICurrencyHttpClient.h"

namespace CalculatorApp::ViewModel
{
    namespace DataLoaders
    {
        class CurrencyHttpClient : public ICurrencyHttpClient
        {
        public:
            CurrencyHttpClient();

            static Platform::String ^ GetRawStaticDataResponse();
            static Platform::String ^ GetRawAllRatiosDataResponse();

            // ICurrencyHttpClient
            void SetSourceCurrencyCode(Platform::String ^ sourceCurrencyCode) override
            {
            }
            void SetResponseLanguage(Platform::String ^ responseLanguage) override
            {
            }

            virtual Windows::Foundation::IAsyncOperationWithProgress<Platform::String ^, Windows::Web::Http::HttpProgress> ^ GetCurrencyMetadata() override;
            virtual Windows::Foundation::IAsyncOperationWithProgress<Platform::String ^, Windows::Web::Http::HttpProgress> ^ GetCurrencyRatios() override;
            // ICurrencyHttpClient
        };

    public
        ref class MockAsyncOperationWithProgress sealed
            : public Windows::Foundation::IAsyncOperationWithProgress<Platform::String ^, Windows::Web::Http::HttpProgress>
        {
        public:
            MockAsyncOperationWithProgress(Platform::String ^ result);

            // IAsyncInfo
            virtual property Windows::Foundation::HResult ErrorCode
            {
                Windows::Foundation::HResult get();
            }

            virtual property unsigned int Id
            {
                unsigned int get()
                {
                    return 128u;
                }
            }

            virtual property Windows::Foundation::AsyncStatus Status
            {
                Windows::Foundation::AsyncStatus get()
                {
                    return Windows::Foundation::AsyncStatus::Completed;
                }
            }

            virtual void Cancel()
            {
            }
            virtual void Close()
            {
            }
            // IAsyncInfo

            // IAsyncOperationWithProgress
            virtual property Windows::Foundation::AsyncOperationProgressHandler<Platform::String^, Windows::Web::Http::HttpProgress>^ Progress
            {
                Windows::Foundation::AsyncOperationProgressHandler<Platform::String^, Windows::Web::Http::HttpProgress>^ get() { return nullptr; }
                void set(Windows::Foundation::AsyncOperationProgressHandler<Platform::String^, Windows::Web::Http::HttpProgress>^ handler) {}
            }

            virtual property Windows::Foundation::AsyncOperationWithProgressCompletedHandler<Platform::String^, Windows::Web::Http::HttpProgress>^ Completed
            {
                Windows::Foundation::AsyncOperationWithProgressCompletedHandler<Platform::String^, Windows::Web::Http::HttpProgress>^ get() { return nullptr; }
                void set(Windows::Foundation::AsyncOperationWithProgressCompletedHandler<Platform::String^, Windows::Web::Http::HttpProgress>^ handler) {}
            }

            virtual Platform::String^ GetResults() { return m_result; }
            // IAsyncOperationWithProgress

        private:
            Platform::String^ m_result;
        };
    }
}
