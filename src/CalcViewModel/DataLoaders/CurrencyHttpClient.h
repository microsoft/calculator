// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ICurrencyHttpClient.h"

namespace CalculatorApp
{
    namespace ViewModel::DataLoaders
    {
        class CurrencyHttpClient : public ICurrencyHttpClient
        {
        public:
            CurrencyHttpClient();

            void SetSourceCurrencyCode(Platform::String ^ sourceCurrencyCode) override;
            void SetResponseLanguage(Platform::String ^ responseLanguage) override;

            Windows::Foundation::IAsyncOperationWithProgress<Platform::String ^, Windows::Web::Http::HttpProgress> ^ GetCurrencyMetadata() override;
            Windows::Foundation::IAsyncOperationWithProgress<Platform::String ^, Windows::Web::Http::HttpProgress> ^ GetCurrencyRatios() override;

        private:
            Windows::Web::Http::HttpClient ^ m_client;
            Platform::String ^ m_responseLanguage;
            Platform::String ^ m_sourceCurrencyCode;
        };
    }
}
