// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CurrencyHttpClient.h"

namespace
{
    constexpr auto MockCurrencyConverterData = LR"(
[
  {
    "An": "MAR",
    "Rt": 1.00
  },
  {
    "An": "MON",
    "Rt": 0.50
  },
  {
    "An": "NEP",
    "Rt": 0.00125
  },
  {
    "An": "SAT",
    "Rt": 0.25
  },
  {
    "An": "URA",
    "Rt": 2.75
  },
  {
    "An": "VEN",
    "Rt": 900.00
  },
  {
    "An": "JUP",
    "Rt": 1.23456789123456789
  },
  {
    "An": "MER",
    "Rt": 2.00
  },
  {
    "An": "JPY",
    "Rt": 0.00125
  },
  {
    "An": "JOD",
    "Rt": 0.25
  }
])";

    constexpr auto MockCurrencyStaticData = LR"(
[
  {
    "CountryCode": "MAR",
    "CountryName": "Mars",
    "CurrencyCode": "MAR",
    "CurrencyName": "The Martian Dollar",
    "CurrencySymbol": "¤"
  },
  {
    "CountryCode": "MON",
    "CountryName": "Moon",
    "CurrencyCode": "MON",
    "CurrencyName": "Moon Bucks",
    "CurrencySymbol": "¤"
  },
  {
    "CountryCode": "NEP",
    "CountryName": "Neptune",
    "CurrencyCode": "NEP",
    "CurrencyName": "Space Coins",
    "CurrencySymbol": "¤"
  },
  {
    "CountryCode": "SAT",
    "CountryName": "Saturn",
    "CurrencyCode": "SAT",
    "CurrencyName": "Rings",
    "CurrencySymbol": "¤"
  },
  {
    "CountryCode": "URA",
    "CountryName": "Uranus",
    "CurrencyCode": "URA",
    "CurrencyName": "Galaxy Credits",
    "CurrencySymbol": "¤"
  },
  {
    "CountryCode": "VEN",
    "CountryName": "Venus",
    "CurrencyCode": "VEN",
    "CurrencyName": "Venusian Seashells",
    "CurrencySymbol": "¤"
  },
  {
    "CountryCode": "JUP",
    "CountryName": "Jupiter",
    "CurrencyCode": "JUP",
    "CurrencyName": "Gas Money",
    "CurrencySymbol": "¤"
  },
  {
    "CountryCode": "MER",
    "CountryName": "Mercury",
    "CurrencyCode": "MER",
    "CurrencyName": "Sun Notes",
    "CurrencySymbol": "¤"
  },
  {
    "CountryCode": "TEST1",
    "CountryName": "Test No Fractional Digits",
    "CurrencyCode": "JPY",
    "CurrencyName": "Test No Fractional Digits",
    "CurrencySymbol": "¤"
  },
  {
    "CountryCode": "TEST2",
    "CountryName": "Test Fractional Digits",
    "CurrencyCode": "JOD",
    "CurrencyName": "Test Fractional Digits",
    "CurrencySymbol": "¤"
  }
])";
} // namespace

namespace CalculatorApp::ViewModel::DataLoaders
{
    void CurrencyHttpClient::Initialize(Platform::String ^ sourceCurrencyCode, Platform::String ^ responseLanguage)
    {
        m_sourceCurrencyCode = sourceCurrencyCode;
        m_responseLanguage = responseLanguage;
    }

    std::future<Platform::String ^> CurrencyHttpClient::GetCurrencyMetadataAsync() const
    {
        (void)m_responseLanguage; // to be used in production.
        std::promise<Platform::String ^> mockedTask;
        mockedTask.set_value(ref new Platform::String(MockCurrencyStaticData));
        return mockedTask.get_future();
    }

    std::future<Platform::String ^> CurrencyHttpClient::GetCurrencyRatiosAsync() const
    {
        (void)m_sourceCurrencyCode; // to be used in production.
        std::promise<Platform::String ^> mockedTask;
        mockedTask.set_value(ref new Platform::String(MockCurrencyConverterData));
        return mockedTask.get_future();
    }
} // namespace CalculatorApp::ViewModel::DataLoaders
