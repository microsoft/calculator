// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using CalculatorApp.ViewModel.Common;

namespace CalculatorApp.Utils
{
    static class DelegateCommandUtils
    {
        public static DelegateCommand MakeDelegateCommand<TTarget>(TTarget target, Action<TTarget, object> handler)
            where TTarget : class
        {
            WeakReference weakTarget = new WeakReference(target);
            return new DelegateCommand(param =>
            {
                TTarget thatTarget = weakTarget.Target as TTarget;
                if(null != thatTarget)
                {
                    handler.Invoke(thatTarget, param);
                }
            });
        }
    }
}

