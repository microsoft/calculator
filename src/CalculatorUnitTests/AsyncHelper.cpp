// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AsyncHelper.h"
#include <chrono>
#include <thread>

using namespace std;
using namespace concurrency;
using namespace Platform;
using namespace CalculatorApp;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;

task<void> AsyncHelper::RunOnUIThreadAsync(function<void()>&& action)
{
    auto callback = ref new DispatchedHandler([action]() { action(); });

    return create_task(CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, callback));
}

void AsyncHelper::RunOnUIThread(function<void()>&& action, DWORD timeout)
{
    task<void> waitTask = RunOnUIThreadAsync([action]() { action(); });

    WaitForTask<void>(waitTask, timeout);
}

void AsyncHelper::Delay(DWORD milliseconds)
{
    thread timer(bind(CalculatorApp::AsyncHelper::Sleep, milliseconds));
    timer.join();
}

void AsyncHelper::Sleep(DWORD milliseconds)
{
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}
