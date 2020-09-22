// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DispatcherTimerDelayer.h"

using namespace CalculatorApp;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;

DispatcherTimerDelayer::DispatcherTimerDelayer(TimeSpan timeSpan)
{
    m_timer = ref new DispatcherTimer();
    m_timer->Interval = timeSpan;
    auto interval = m_timer->Interval;
    m_timer->Tick += ref new EventHandler<Object ^>(this, &DispatcherTimerDelayer::Timer_Tick);
}

void DispatcherTimerDelayer::Timer_Tick(Object ^ sender, Object ^ e)
{
    m_timer->Stop();
    Action(this, nullptr);
}

void DispatcherTimerDelayer::Start()
{
    m_timer->Start();
}

void DispatcherTimerDelayer::ResetAndStart()
{
    m_timer->Stop();
    m_timer->Start();
}

void DispatcherTimerDelayer::Stop()
{
    m_timer->Stop();
}
