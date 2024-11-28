// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DataLoaders/CurrencyHttpClient.h"

namespace
{
    constexpr auto MockCurrencyConverterData =
        LR"([{"An":"USD","Ch":0,"Pc":0,"Rt":1},{"An":"EUR","Ch":0.003803,"Pc":0.4149,"Rt":0.920503,"Yh":0.9667,"Yl":0.86701}])";
    constexpr auto MockCurrencyStaticData =
        LR"([{"CountryCode":"USA","CountryName":"United States","CurrencyCode":"USD","CurrencyName":"Dollar","CurrencySymbol":"$"},{"CountryCode":"EUR","CountryName":"Europe","CurrencyCode":"EUR","CurrencyName":"Euro","CurrencySymbol":"€"}])";
}

namespace CalculatorApp::ViewModel::DataLoaders
{
    bool CurrencyHttpClient::ForceWebFailure = false;
    void CurrencyHttpClient::Initialize(Platform::String ^ sourceCurrencyCode, Platform::String ^ responseLanguage)
    {
        m_sourceCurrencyCode = sourceCurrencyCode;
        m_responseLanguage = responseLanguage;
    }

    std::future<Platform::String ^> CurrencyHttpClient::GetCurrencyMetadataAsync() const
    {
        if (ForceWebFailure)
        {
            throw ref new Platform::Exception(E_FAIL, L"Mocked Network Failure: failed to load currency metadata");
        }
        (void)m_responseLanguage; // to be used in production.
        std::promise<Platform::String ^> mockedTask;
        mockedTask.set_value(ref new Platform::String(MockCurrencyStaticData));
        return mockedTask.get_future();
    }

    std::future<Platform::String ^> CurrencyHttpClient::GetCurrencyRatiosAsync() const
    {
        if (ForceWebFailure)
        {
            throw ref new Platform::Exception(E_FAIL, L"Mocked Network Failure: failed to load currency metadata");
        }
        (void)m_sourceCurrencyCode; // to be used in production.

        std::promise<Platform::String ^> mockedTask;
        mockedTask.set_value(ref new Platform::String(MockCurrencyConverterData));
        return mockedTask.get_future();
    }
} // namespace CalculatorApp::ViewModel::DataLoaders
