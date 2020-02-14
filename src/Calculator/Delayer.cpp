// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Delayer.h"

using namespace CalculatorApp;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;

Delayer::Delayer(TimeSpan timeSpan)
{
    m_timer = ref new DispatcherTimer();
    m_timer->Interval = timeSpan;
    auto interval = m_timer->Interval;
    m_timer->Tick += ref new EventHandler<Object ^>(this, &Delayer::Timer_Tick);
}

void Delayer::Timer_Tick(Object ^ sender, Object ^ e)
{
    m_timer->Stop();
    Action(this, ref new RoutedEventArgs());
}

void Delayer::Start()
{
    m_timer->Start();
}

void Delayer::ResetAndStart()
{
    m_timer->Stop();
    m_timer->Start();
}

void Delayer::Stop()
{
    m_timer->Stop();
}
