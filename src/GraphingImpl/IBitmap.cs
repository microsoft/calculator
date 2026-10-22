// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;

namespace Graphing
{
    public interface IBitmap
    {
        IReadOnlyList<byte> GetData();
    }
}
