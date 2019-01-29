// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Common
    {
        class ConversionResultTaskHelper
        {
        public:
            ConversionResultTaskHelper(unsigned int delay, const std::function<void()> functionToRun);
            ~ConversionResultTaskHelper();
            
        private:
            concurrency::task<void> CompleteAfter(unsigned int timeout);

            unsigned int m_delay;
            concurrency::cancellation_token_source m_cts;
            const std::function<void()> m_storedFunction;
        };
    }
}
