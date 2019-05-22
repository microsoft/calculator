// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using Windows.Foundation;
using Windows.Web.Http;

namespace CalculatorApp.DataLoaders
{
	public class CurrencyHttpClient : ICurrencyHttpClient
	{
		Windows.Web.Http.HttpClient m_client;
		string m_responseLanguage;
		string m_sourceCurrencyCode;

		static string sc_MetadataUriLocalizeFor = "https://go.microsoft.com/fwlink/?linkid=2041093&localizeFor=";
		static string sc_RatiosUriRelativeTo = "https://go.microsoft.com/fwlink/?linkid=2041339&localCurrency=";

		public CurrencyHttpClient()
		{
			m_client = new HttpClient();
			m_responseLanguage = "en-US";
		}

		public void SetSourceCurrencyCode(String sourceCurrencyCode)
		{
			m_sourceCurrencyCode = sourceCurrencyCode;
		}

		public void SetResponseLanguage(String responseLanguage)
		{
			m_responseLanguage = responseLanguage;
		}

		public IAsyncOperationWithProgress<String, HttpProgress> GetCurrencyMetadata()
		{
			string uri = sc_MetadataUriLocalizeFor + m_responseLanguage;
			var metadataUri = new Uri(uri);

			return m_client.GetStringAsync(metadataUri);
		}

		public IAsyncOperationWithProgress<String, HttpProgress> GetCurrencyRatios()
		{
			string uri = sc_RatiosUriRelativeTo + m_sourceCurrencyCode;
			var ratiosUri = new Uri(uri);

			return m_client.GetStringAsync(ratiosUri);
		}
	}
}
