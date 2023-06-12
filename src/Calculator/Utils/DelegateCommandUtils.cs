// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;

using System;

namespace CalculatorApp.Utils
{
    internal static class DelegateCommandUtils
    {
        public static DelegateCommand MakeDelegateCommand<TTarget>(TTarget target, Action<TTarget, object> handler)
            where TTarget : class
        {
            WeakReference weakTarget = new WeakReference(target);
            return new DelegateCommand(param =>
            {
                if (weakTarget.Target is TTarget thatTarget)
                {
                    handler.Invoke(thatTarget, param);
                }
            });
        }
    }
}

