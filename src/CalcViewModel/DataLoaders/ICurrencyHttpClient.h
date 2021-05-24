// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace ViewModel::DataLoaders
    {
        class ICurrencyHttpClient
        {
        public:
            virtual ~ICurrencyHttpClient()
            {
            }

            virtual void SetSourceCurrencyCode(Platform::String ^ sourceCurrencyCode) = 0;
            virtual void SetResponseLanguage(Platform::String ^ responseLanguage) = 0;

            virtual Windows::Foundation::IAsyncOperationWithProgress<Platform::String ^, Windows::Web::Http::HttpProgress> ^ GetCurrencyMetadata() = 0;
            virtual Windows::Foundation::IAsyncOperationWithProgress<Platform::String ^, Windows::Web::Http::HttpProgress> ^ GetCurrencyRatios() = 0;
        };
    }
}
