// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 6 STUB for the currency data loader. Task 7 replaces this file with
// the full Frankfurter-backed port (cache, ratios, symbols, timestamps).
// The stub keeps UnitConverterViewModel compiling with dormant currency
// behavior: no currency units, no ratios, loads always fail closed.
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Calculator.Linux.DataLoaders;

public interface IViewModelCurrencyCallback
{
    void CurrencyDataLoadFinished(bool didLoad);
    void CurrencyTimestampUpdated(string timestamp, bool isWeekOld);
    void NetworkBehaviorChanged(Common.NetworkAccessBehavior newBehavior);
}

public static class UnitConverterResourceKeys
{
    public const string CurrencyUnitFromKey = "CURRENCY_UNIT_FROM_KEY";
    public const string CurrencyUnitToKey = "CURRENCY_UNIT_TO_KEY";
}

public sealed class CurrencyDataLoader
{
    public void SetViewModelCallback(IViewModelCurrencyCallback callback)
    {
    }

    public void LoadData()
    {
    }

    public List<CurrencyUnit> GetOrderedUnits() => new();

    public Dictionary<int, ConversionData> LoadOrderedRatios(int unitId) => new();

    public (string Symbol1, string Symbol2) GetCurrencySymbols(int unit1Id, int unit2Id) => (string.Empty, string.Empty);

    public (string Ratio1, string Ratio2) GetCurrencyRatioEquality(int unit1Id, int unit2Id) =>
        (string.Empty, string.Empty);

    public Task<bool> TryLoadDataFromWebOverrideAsync() => Task.FromResult(false);
}
