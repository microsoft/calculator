// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp::ViewModel::DataLoaders
{
    class CurrencyHttpClient
    {
    public:
        static bool ForceWebFailure;
        void Initialize(Platform::String ^ sourceCurrencyCode, Platform::String ^ responseLanguage);

        Platform::String ^ GetCurrencyMetadata() const;
        Platform::String ^ GetCurrencyRatios() const;

    private:
        Platform::String ^ m_sourceCurrencyCode;
        Platform::String ^ m_responseLanguage;
    };
}
