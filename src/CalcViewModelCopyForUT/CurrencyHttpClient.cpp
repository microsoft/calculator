// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CalcViewModel/DataLoaders/CurrencyHttpClient.h"

namespace
{
    constexpr auto MockCurrencyConverterData =
        LR"([{"An":"USD","Ch":0,"Pc":0,"Rt":1},{"An":"EUR","Ch":0.003803,"Pc":0.4149,"Rt":0.920503,"Yh":0.9667,"Yl":0.86701}])";
    constexpr auto MockCurrencyStaticData =
        LR"([{"CountryCode":"USA","CountryName":"United States","CurrencyCode":"USD","CurrencyName":"Dollar","CurrencySymbol":"$"},{"CountryCode":"EUR","CountryName":"Europe","CurrencyCode":"EUR","CurrencyName":"Euro","CurrencySymbol":"€"}])";
}

namespace CalculatorApp::ViewModel::DataLoaders
{
    void CurrencyHttpClient::Initialize(Platform::String ^ sourceCurrencyCode, Platform::String ^ responseLanguage)
    {
        m_sourceCurrencyCode = sourceCurrencyCode;
        m_responseLanguage = responseLanguage;
    }

    Platform::String ^ CurrencyHttpClient::GetCurrencyMetadata() const
    {
        (void)m_responseLanguage; // to be used in production.
        return ref new Platform::String(MockCurrencyStaticData);
    }

    Platform::String ^ CurrencyHttpClient::GetCurrencyRatios() const
    {
        (void)m_sourceCurrencyCode; // to be used in production.
        return ref new Platform::String(MockCurrencyConverterData);
    }
} // namespace CalculatorApp::ViewModel::DataLoaders
