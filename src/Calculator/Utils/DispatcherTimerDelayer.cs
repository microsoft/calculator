// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

using Windows.UI.Xaml;

namespace CalculatorApp
{
    public sealed class DispatcherTimerDelayer
    {
        public event EventHandler<object> Action;

        public DispatcherTimerDelayer(TimeSpan timeSpan)
        {
            m_timer = new DispatcherTimer
            {
                Interval = timeSpan
            };
            var interval = m_timer.Interval;
            m_timer.Tick += Timer_Tick;
        }

        public void Start()
        {
            m_timer.Start();
        }

        public void ResetAndStart()
        {
            m_timer.Stop();
            m_timer.Start();
        }

        public void Stop()
        {
            m_timer.Stop();
        }

        private void Timer_Tick(object sender, object e)
        {
            m_timer.Stop();
            Action?.Invoke(this, null);
        }

        private readonly DispatcherTimer m_timer;
    }
}
