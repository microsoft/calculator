// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CalculatorApp.Common
{
	class ConversionResultTaskHelper
	{
		uint m_delay;
		CancellationTokenSource m_cts = new CancellationTokenSource();
		Action m_storedFunction;

		public ConversionResultTaskHelper(uint  delay, Action functionToRun)
		{
			m_delay = delay;
			m_storedFunction = functionToRun;

			var delayTask = CompleteAfter(delay);
		}

		~ConversionResultTaskHelper()
		{
			m_cts.Cancel();
		}

		// Creates a task that completes after the specified delay.
		//
		// Taken from: How to: Create a Task that Completes After a Delay
		// https://msdn.microsoft.com/en-us/library/hh873170.aspx
		async Task CompleteAfter(uint timeout)
		{
			await Task.Delay(TimeSpan.FromMilliseconds(timeout));
			if (!m_cts.Token.IsCancellationRequested)
			{
				m_storedFunction();
			}
		}
	}
}
