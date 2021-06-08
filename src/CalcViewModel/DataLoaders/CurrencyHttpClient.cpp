// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CurrencyHttpClient.h"

#ifdef USE_MOCK_DATA
#include "DataLoaderMockConstants.h"
#else
#include "DataLoaderConstants.h"
#endif

using namespace CalculatorApp::DataLoaders;
using namespace CalculatorApp::ViewModel::DataLoaders;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;

CurrencyHttpClient::CurrencyHttpClient()
    : m_client(ref new HttpClient())
    , m_responseLanguage(L"en-US")
{
}

void CurrencyHttpClient::SetSourceCurrencyCode(String ^ sourceCurrencyCode)
{
    m_sourceCurrencyCode = sourceCurrencyCode;
}

void CurrencyHttpClient::SetResponseLanguage(String ^ responseLanguage)
{
    m_responseLanguage = responseLanguage;
}

IAsyncOperationWithProgress<String ^, HttpProgress> ^ CurrencyHttpClient::GetCurrencyMetadata()
{
    wstring uri = wstring{ sc_MetadataUriLocalizeFor } + m_responseLanguage->Data();
    auto metadataUri = ref new Uri(StringReference(uri.c_str()));

    return m_client->GetStringAsync(metadataUri);
}

IAsyncOperationWithProgress<String ^, HttpProgress> ^ CurrencyHttpClient::GetCurrencyRatios()
{
    wstring uri = wstring{ sc_RatiosUriRelativeTo } + m_sourceCurrencyCode->Data();
    auto ratiosUri = ref new Uri(StringReference(uri.c_str()));

    return m_client->GetStringAsync(ratiosUri);
}
