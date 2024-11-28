// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <cassert>

namespace CalculatorApp::ViewModel::DataLoaders
{
    template <class T>
    struct MockAwaitable
    {
        T Value;

        bool await_ready() const noexcept
        {
            return true;
        }

        void await_suspend(std::experimental::coroutine_handle<>) const noexcept
        {
            assert(false && "not implemented.");
        }

        T await_resume() noexcept
        {
            return std::move(Value);
        }
    };

    class CurrencyHttpClient
    {
    public:
        static bool ForceWebFailure;
        void Initialize(Platform::String ^ sourceCurrencyCode, Platform::String ^ responseLanguage);

        MockAwaitable<Platform::String ^> GetCurrencyMetadata() const;
        MockAwaitable<Platform::String ^> GetCurrencyRatios() const;

    private:
        Platform::String ^ m_sourceCurrencyCode;
        Platform::String ^ m_responseLanguage;
    };
}
