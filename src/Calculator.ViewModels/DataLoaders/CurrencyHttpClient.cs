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

        private static readonly Windows.Web.Http.HttpClient _sharedClient = new Windows.Web.Http.HttpClient();
        private string _responseLanguage;

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
            try
            {
                string url = BuildMetadataUrl();
                var response = await _sharedClient.GetStringAsync(new Uri(url));
                return response;
            }
            catch
            {
                return null;
            }
        }

        public async Task<string> GetCurrencyRatiosAsync()
        {
#if VIEWMODEL_FOR_UT
            if (ForceWebFailure)
            {
                throw new Exception("Forced web failure for unit testing");
            }
#endif
            try
            {
                string url = BuildRatiosUrl();
                var response = await _sharedClient.GetStringAsync(new Uri(url));
                return response;
            }
            catch
            {
                return null;
            }
        }

        private string BuildMetadataUrl()
        {
            // Build the URL for currency metadata based on source currency and language
            return $"https://go.microsoft.com/fwlink/?LinkID=828052&clcid={_responseLanguage}";
        }

        private string BuildRatiosUrl()
        {
            // Build the URL for currency ratios
            return $"https://go.microsoft.com/fwlink/?LinkID=828053&clcid={_responseLanguage}";
        }
    }
}
