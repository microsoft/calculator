// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <wrl/wrappers/corewrappers.h>

namespace CalculatorApp
{
    class AsyncHelper
    {
    public:
        static concurrency::task<void> RunOnUIThreadAsync(std::function<void()>&& action);
        static void RunOnUIThread(std::function<void()>&& action, DWORD timeout = INFINITE);
        static void Delay(DWORD milliseconds);

        template<typename T>
        static void RunOnUIThread(std::function<concurrency::task<T>()>&& action, DWORD timeout = INFINITE)
        {
            concurrency::task<T> t;
            concurrency::task<void> uiTask = RunOnUIThreadAsync([&t, action]()
            {
                t = action();
            }).then([&t]()
            {
                t.wait();
            }, concurrency::task_continuation_context::use_arbitrary());

            WaitForTask<void>(uiTask, timeout);
        }

        template<typename T>
        static bool WaitForTask(concurrency::task<T>& t, DWORD timeout = INFINITE)
        {
            Microsoft::WRL::Wrappers::Event event(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
            if (!event.IsValid())
            {
                throw std::bad_alloc();
            }

            Platform::Exception^ ex;
            t.then([&event, &ex](concurrency::task<T> prevTask)
            {
                try
                {
                    prevTask.get();
                }
                catch (Platform::Exception^ e)
                {
                    ex = e;
                }

                if (event.IsValid())
                {
                    SetEvent(event.Get());
                }
            }, concurrency::task_continuation_context::use_arbitrary());

            DWORD waitResult;// = STATUS_PENDING;
            waitResult = WaitForSingleObjectEx(event.Get(), timeout, true);
            event.Close();

            if (ex != nullptr)
            {
                throw ex;
            }

            if (waitResult == WAIT_FAILED)
            {
                throw ref new Platform::Exception(-1, L"Error in waiting for task completion: " + waitResult.ToString());
            }

            return waitResult == WAIT_OBJECT_0;
        }
    private:
        static void Sleep(DWORD milliseconds);
    };
}
