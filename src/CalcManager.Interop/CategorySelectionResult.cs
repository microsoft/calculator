// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace CalcManager.Interop
{
    public sealed class CategorySelectionResult
    {
        public UnitWrapper[] Units { get; }
        public UnitWrapper FromUnit { get; }
        public UnitWrapper ToUnit { get; }

        public CategorySelectionResult()
        {
            Units = Array.Empty<UnitWrapper>();
            FromUnit = default;
            ToUnit = default;
        }

        public CategorySelectionResult(
            UnitWrapper[] units,
            UnitWrapper fromUnit,
            UnitWrapper toUnit)
        {
            Units = units ?? Array.Empty<UnitWrapper>();
            FromUnit = fromUnit;
            ToUnit = toUnit;
        }
    }
}
