// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

namespace CalculatorApp
{
    public ref class DispatcherTimerDelayer sealed
    {
    public:
        DispatcherTimerDelayer(Windows::Foundation::TimeSpan timeSpan);

        void Start();
        void ResetAndStart();
        void Stop();

        event Windows::Foundation::EventHandler<Platform::Object ^> ^ Action;

    private:
        void Timer_Tick(Platform::Object ^ sender, Platform::Object ^ e);

        Windows::UI::Xaml::DispatcherTimer ^ m_timer;
    };
}
