// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Threading.Tasks;

namespace CalculatorUnitTests
{
    public static class AsyncHelper
    {
        public static T RunSynchronously<T>(Task<T> task)
        {
            return task.GetAwaiter().GetResult();
        }

        public static void RunSynchronously(Task task)
        {
            task.GetAwaiter().GetResult();
        }
    }
}
