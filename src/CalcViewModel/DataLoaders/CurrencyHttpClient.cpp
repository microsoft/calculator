// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CurrencyHttpClient.h"

using namespace CalculatorApp::DataLoaders;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;

static constexpr auto sc_MetadataUriLocalizeFor = L"https://go.microsoft.com/fwlink/?linkid=2041093&localizeFor=";
static constexpr auto sc_RatiosUriRelativeTo = L"https://go.microsoft.com/fwlink/?linkid=2041339&localCurrency=";

CurrencyHttpClient::CurrencyHttpClient() :
    m_client(ref new HttpClient()),
    m_responseLanguage(L"en-US")
{
}

void CurrencyHttpClient::SetSourceCurrencyCode(String^ sourceCurrencyCode)
{
    m_sourceCurrencyCode = sourceCurrencyCode;
}

void CurrencyHttpClient::SetResponseLanguage(String^ responseLanguage)
{
    m_responseLanguage = responseLanguage;
}

IAsyncOperationWithProgress<String^, HttpProgress>^ CurrencyHttpClient::GetCurrencyMetadata()
{
    wstring uri = wstring{ sc_MetadataUriLocalizeFor } + m_responseLanguage->Data();
    auto metadataUri = ref new Uri(StringReference(uri.c_str()));

    return m_client->GetStringAsync(metadataUri);
}

IAsyncOperationWithProgress<String^, HttpProgress>^ CurrencyHttpClient::GetCurrencyRatios()
{
    wstring uri = wstring{ sc_RatiosUriRelativeTo } + m_sourceCurrencyCode->Data();
    auto ratiosUri = ref new Uri(StringReference(uri.c_str()));

    return m_client->GetStringAsync(ratiosUri);
}
