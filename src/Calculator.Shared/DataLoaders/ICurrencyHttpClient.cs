// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;

namespace CalculatorApp.DataLoaders
{
	public interface ICurrencyHttpClient
	{
		void SetSourceCurrencyCode(string sourceCurrencyCode);
        void SetResponseLanguage(string responseLanguage);

        Windows.Foundation.IAsyncOperationWithProgress<string, Windows.Web.Http.HttpProgress> GetCurrencyMetadata();
        Windows.Foundation.IAsyncOperationWithProgress<string, Windows.Web.Http.HttpProgress> GetCurrencyRatios();
	}
}
