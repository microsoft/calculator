// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using Graphing;

namespace MockGraphingImpl
{
    public class Bitmap : IBitmap
    {
        public IReadOnlyList<byte> GetData()
        {
            return Array.Empty<byte>();
        }
    }
}
