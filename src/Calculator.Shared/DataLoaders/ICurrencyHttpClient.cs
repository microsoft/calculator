// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace CalculatorApp.DataLoaders
{
	public interface ICurrencyHttpClient
	{
		void SetSourceCurrencyCode(string sourceCurrencyCode);
        void SetResponseLanguage(string responseLanguage);

        Task<string> GetCurrencyMetadata();
        Task<string> GetCurrencyRatios();
	}
}
