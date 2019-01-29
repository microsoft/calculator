// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ConversionResultTaskHelper.h"

using namespace CalculatorApp::Common;
using namespace concurrency;
using namespace std;

ConversionResultTaskHelper::ConversionResultTaskHelper(unsigned int delay, const function<void()> functionToRun) :
    m_delay{ delay },
    m_storedFunction{ functionToRun }
{
    auto token = m_cts.get_token();
    auto delayTask = CompleteAfter(delay);
    delayTask.then([this, token]()
    {
        if (!token.is_canceled())
        {
            m_storedFunction();
        }
    }, task_continuation_context::use_current());
}

ConversionResultTaskHelper::~ConversionResultTaskHelper()
{
    m_cts.cancel();
}

#pragma optimize("", off)
// Creates a task that completes after the specified delay.
//
// Taken from: How to: Create a Task that Completes After a Delay
// https://msdn.microsoft.com/en-us/library/hh873170.aspx
task<void> ConversionResultTaskHelper::CompleteAfter(unsigned int timeout)
{
    co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{ std::chrono::duration<uint32_t, std::milli>(timeout) });
};
#pragma optimize("", on)
