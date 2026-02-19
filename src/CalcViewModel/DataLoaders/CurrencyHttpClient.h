// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <cassert>
#include <future>

namespace CalculatorApp::ViewModel::DataLoaders
{
    class CurrencyHttpClient
    {
    public:
#ifdef VIEWMODEL_FOR_UT
        static bool ForceWebFailure;
#endif
        void Initialize(Platform::String ^ sourceCurrencyCode, Platform::String ^ responseLanguage);

        std::future<Platform::String ^> GetCurrencyMetadataAsync() const;
        std::future<Platform::String ^> GetCurrencyRatiosAsync() const;

    private:
        Platform::String ^ m_sourceCurrencyCode;
        Platform::String ^ m_responseLanguage;
    };
}
