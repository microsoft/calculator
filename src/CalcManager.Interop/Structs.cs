// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalcManager.Interop
{
    public struct UnitWrapper
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string AccessibleName { get; set; }
        public string Abbreviation { get; set; }
        public bool IsConversionSource { get; set; }
        public bool IsConversionTarget { get; set; }
        public bool IsWhimsical { get; set; }
    }

    public struct CategoryWrapper
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public bool SupportsNegative { get; set; }
    }

    public struct ConversionDataWrapper
    {
        public double Ratio { get; set; }
        public double Offset { get; set; }
        public bool OffsetFirst { get; set; }
    }

    public struct UnitConversionEntry
    {
        public UnitWrapper Unit { get; set; }
        public double Ratio { get; set; }
        public double Offset { get; set; }
        public bool OffsetFirst { get; set; }
    }

    public struct SuggestedValueWrapper
    {
        public string Value { get; set; }
        public UnitWrapper Unit { get; set; }
    }
}
