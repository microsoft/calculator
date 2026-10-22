// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace CalcManager.Interop
{
    // Delegates for display callbacks
    public delegate void SetPrimaryDisplayHandler(string display, bool isError);
    public delegate void SetIsInErrorHandler(bool isError);
    public delegate void SetExpressionDisplayHandler(HistoryToken[] tokens, ExpressionCommandWrapper[] commands);
    public delegate void SetParenthesisNumberHandler(uint count);
    public delegate void SimpleHandler();
    public delegate void OnHistoryItemAddedHandler(uint addedItemIndex);
    public delegate void SetMemorizedNumbersHandler(string[] memorizedNumbers);
    public delegate void MemoryItemChangedHandler(uint indexOfMemory);

    // Delegate for resource provider
    public delegate string GetCEngineStringHandler(string id);

    public class UnitConverterVMCallbackBase
    {
        public virtual void DisplayCallback(string fromValue, string toValue) { }
        public virtual void SuggestedValueCallback(SuggestedValueWrapper[] suggestedValues) { }
        public virtual void MaxDigitsReached() { }
    }

    public class ViewModelCurrencyCallbackBase
    {
        public virtual void CurrencyDataLoadFinished(bool didLoad) { }
        public virtual void CurrencySymbolsCallback(string fromSymbol, string toSymbol) { }
        public virtual void CurrencyRatiosCallback(string ratioEquality, string accRatioEquality) { }
        public virtual void CurrencyTimestampCallback(string timestamp, bool isWeekOldData) { }
        public virtual void NetworkBehaviorChanged(int newBehavior) { }
    }

    public class ConverterDataLoaderBase
    {
        public virtual void LoadData() { }
        public virtual CategoryWrapper[] GetOrderedCategories() => Array.Empty<CategoryWrapper>();
        public virtual UnitWrapper[] GetOrderedUnits(CategoryWrapper category) => Array.Empty<UnitWrapper>();
        public virtual UnitConversionEntry[] LoadOrderedRatios(UnitWrapper unit) => Array.Empty<UnitConversionEntry>();
        public virtual bool SupportsCategory(CategoryWrapper target) => false;
    }
}
