// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Threading.Tasks;

namespace CalculatorApp.ViewModel.DataLoaders
{
    public sealed class CurrencyHttpClient
    {
#if VIEWMODEL_FOR_UT
        public static bool ForceWebFailure = false;
#endif

        private string _responseLanguage;

        private const string MockCurrencyConverterData = @"[
  { ""An"": ""MAR"", ""Rt"": 1.00 },
  { ""An"": ""MON"", ""Rt"": 0.50 },
  { ""An"": ""NEP"", ""Rt"": 0.00125 },
  { ""An"": ""SAT"", ""Rt"": 0.25 },
  { ""An"": ""URA"", ""Rt"": 2.75 },
  { ""An"": ""VEN"", ""Rt"": 900.00 },
  { ""An"": ""JUP"", ""Rt"": 1.23456789123456789 },
  { ""An"": ""MER"", ""Rt"": 2.00 },
  { ""An"": ""JPY"", ""Rt"": 0.00125 },
  { ""An"": ""JOD"", ""Rt"": 0.25 }
]";

        private const string MockCurrencyStaticData = @"[
  { ""CountryCode"": ""MAR"", ""CountryName"": ""Mars"", ""CurrencyCode"": ""MAR"", ""CurrencyName"": ""The Martian Dollar"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""MON"", ""CountryName"": ""Moon"", ""CurrencyCode"": ""MON"", ""CurrencyName"": ""Moon Bucks"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""NEP"", ""CountryName"": ""Neptune"", ""CurrencyCode"": ""NEP"", ""CurrencyName"": ""Space Coins"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""SAT"", ""CountryName"": ""Saturn"", ""CurrencyCode"": ""SAT"", ""CurrencyName"": ""Rings"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""URA"", ""CountryName"": ""Uranus"", ""CurrencyCode"": ""URA"", ""CurrencyName"": ""Galaxy Credits"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""VEN"", ""CountryName"": ""Venus"", ""CurrencyCode"": ""VEN"", ""CurrencyName"": ""Venusian Seashells"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""JUP"", ""CountryName"": ""Jupiter"", ""CurrencyCode"": ""JUP"", ""CurrencyName"": ""Gas Money"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""MER"", ""CountryName"": ""Mercury"", ""CurrencyCode"": ""MER"", ""CurrencyName"": ""Sun Notes"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""TEST1"", ""CountryName"": ""Test No Fractional Digits"", ""CurrencyCode"": ""JPY"", ""CurrencyName"": ""Test No Fractional Digits"", ""CurrencySymbol"": ""¤"" },
  { ""CountryCode"": ""TEST2"", ""CountryName"": ""Test Fractional Digits"", ""CurrencyCode"": ""JOD"", ""CurrencyName"": ""Test Fractional Digits"", ""CurrencySymbol"": ""¤"" }
]";

        public void Initialize(string sourceCurrencyCode, string responseLanguage)
        {
            _responseLanguage = responseLanguage;
        }

        public async Task<string> GetCurrencyMetadataAsync()
        {
#if VIEWMODEL_FOR_UT
            if (ForceWebFailure)
            {
                throw new Exception("Forced web failure for unit testing");
            }
#endif
            await Task.CompletedTask;
            return MockCurrencyStaticData;
        }

        public async Task<string> GetCurrencyRatiosAsync()
        {
#if VIEWMODEL_FOR_UT
            if (ForceWebFailure)
            {
                throw new Exception("Forced web failure for unit testing");
            }
#endif
            await Task.CompletedTask;
            return MockCurrencyConverterData;
        }
    }
}
