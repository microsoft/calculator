// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CurrencyHttpClient.h"

#include "CalcViewModel/Common/NetworkManager.h"

using namespace CalculatorApp::ViewModel::DataLoaders;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::System::UserProfile;
using namespace Windows::Web::Http;

// Generic responses so unit tests will pass.
static constexpr auto STATIC_DATA_RESPONSE =
    LR"([{"CountryCode":"USA","CountryName":"United States","CurrencyCode":"USD","CurrencyName":"Dollar","CurrencySymbol":"$"},{"CountryCode":"EUR","CountryName":"Europe","CurrencyCode":"EUR","CurrencyName":"Euro","CurrencySymbol":"€"}])";
static constexpr auto ALL_RATIOS_RESPONSE =
    LR"([{"An":"USD","Ch":0,"Pc":0,"Rt":1},{"An":"EUR","Ch":0.003803,"Pc":0.4149,"Rt":0.920503,"Yh":0.9667,"Yl":0.86701}])";

CurrencyHttpClient::CurrencyHttpClient()
{
}

String ^ CurrencyHttpClient::GetRawStaticDataResponse()
{
    return StringReference(STATIC_DATA_RESPONSE);
}

String ^ CurrencyHttpClient::GetRawAllRatiosDataResponse()
{
    return StringReference(ALL_RATIOS_RESPONSE);
}

IAsyncOperationWithProgress<String ^, HttpProgress> ^ CurrencyHttpClient::GetCurrencyMetadata()
{
    return ref new MockAsyncOperationWithProgress(StringReference(STATIC_DATA_RESPONSE));
}

IAsyncOperationWithProgress<String ^, HttpProgress> ^ CurrencyHttpClient::GetCurrencyRatios()
{
    return ref new MockAsyncOperationWithProgress(StringReference(ALL_RATIOS_RESPONSE));
}

MockAsyncOperationWithProgress::MockAsyncOperationWithProgress(String ^ result)
    : m_result(result)
{
}

HResult MockAsyncOperationWithProgress::ErrorCode::get()
{
    HResult okayResult;
    okayResult.Value = S_OK;
    return okayResult;
}
