// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Managed converter data types. Upstream these are WinRT projections
// (CalcManager.Interop); here they are plain records with identical shapes.
//
// Currency seam: Task 6 ships without live currency (Task 7). The loader
// talks to an ICurrencySource instead of the full CurrencyDataLoader;
// Task 7 adapts the Frankfurter-backed loader to this interface.
using System.Collections.Generic;

namespace Calculator.Linux.DataLoaders;

public sealed class CategoryData
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;
    public bool SupportsNegative { get; set; }
}

public sealed class UnitData
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;
    public string Abbreviation { get; set; } = string.Empty;
    public string AccessibleName { get; set; } = string.Empty;
    public bool IsConversionSource { get; set; }
    public bool IsConversionTarget { get; set; }
    public bool IsWhimsical { get; set; }
}

public sealed class ConversionEntry
{
    public UnitData Unit { get; set; } = new UnitData();
    public double Ratio { get; set; }
    public double Offset { get; set; }
    public bool OffsetFirst { get; set; }
}

public sealed class CurrencyUnit
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;
    public string CountryName { get; set; } = string.Empty;
    public string Abbreviation { get; set; } = string.Empty;
    public bool IsRtlLanguage { get; set; }
    public bool IsConversionSource { get; set; }
    public bool IsConversionTarget { get; set; }
}

public sealed class ConversionData
{
    public double Ratio { get; set; }
    public double Offset { get; set; }
    public bool OffsetFirst { get; set; }

    public ConversionData(double ratio, double offset = 0, bool offsetFirst = false)
    {
        Ratio = ratio;
        Offset = offset;
        OffsetFirst = offsetFirst;
    }
}

public sealed class CurrencyUnitMetadata
{
    public string Symbol { get; set; } = string.Empty;

    public CurrencyUnitMetadata(string symbol)
    {
        Symbol = symbol;
    }
}

public struct CurrencyStaticData
{
    public string CountryCode;
    public string CountryName;
    public string CurrencyCode;
    public string CurrencyName;
    public string CurrencySymbol;
}

public struct CurrencyRatio
{
    public double Ratio;
    public string SourceCurrencyCode;
    public string TargetCurrencyCode;
}

public interface ICurrencySource
{
    List<CurrencyUnit> GetOrderedUnits();
    Dictionary<int, ConversionData> LoadOrderedRatios(int unitId);
}

public interface IViewModelCurrencyCallback
{
    void CurrencyDataLoadFinished(bool didLoad);
    void CurrencyTimestampUpdated(string timestamp, bool isWeekOld);
    void NetworkBehaviorChanged(Common.NetworkAccessBehavior newBehavior);
}
