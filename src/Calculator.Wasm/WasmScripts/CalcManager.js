class CalcManager {
    static registerCallbacks() {

        var _getCEngineStringCallback       = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:GetCEngineStringCallback");
        var _binaryOperatorReceivedCallback = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:BinaryOperatorReceivedCallback");
        var _setPrimaryDisplayCallback      = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:SetPrimaryDisplayCallback");
        var _setIsInErrorCallback           = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:SetIsInErrorCallback");
        var _setParenthesisNumberCallback   = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:SetParenthesisNumberCallback");
        var _maxDigitsReachedCallback       = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:MaxDigitsReachedCallback");
        var _memoryItemChangedCallback      = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:MemoryItemChangedCallback");
        var _onHistoryItemAddedCallback     = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:OnHistoryItemAddedCallback");
        var _onNoRightParenAddedCallback    = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:OnNoRightParenAddedCallback");
        var _setExpressionDisplayCallback   = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:SetExpressionDisplayCallback");
        var _setMemorizedNumbersCallback    = Module.mono_bind_static_method("[Calculator.Wasm] CalculationManager.NativeDispatch:SetMemorizedNumbersCallback");

        var fGetCEngineStringCallback       = Module.addFunction((state, id) => _getCEngineStringCallback(state, Module.UTF8ToString(id)), 'iii');

        var fBinaryOperatorReceivedCallback = Module.addFunction((state) => _binaryOperatorReceivedCallback(state), 'vi');
        var fSetPrimaryDisplayCallback      = Module.addFunction((state, displayStringValue, isError) => _setPrimaryDisplayCallback(state, Module.UTF8ToString(displayStringValue), isError), 'viii');
        var fSetIsInErrorCallback           = Module.addFunction((state, isError) => _setIsInErrorCallback(state, isError), 'vii');
        var fSetParenthesisNumberCallback   = Module.addFunction((state, parenthesisCount) => _setParenthesisNumberCallback(state, parenthesisCount), 'vii');
        var fMaxDigitsReachedCallback       = Module.addFunction((state) => _maxDigitsReachedCallback(state), 'vii');
        var fMemoryItemChangedCallback      = Module.addFunction((state, indexOfMemory) => _memoryItemChangedCallback(state, indexOfMemory), 'vii');
        var fOnHistoryItemAddedCallback     = Module.addFunction((state, addedItemIndex) => _onHistoryItemAddedCallback(state, addedItemIndex), 'vii');
        var fOnNoRightParenAddedCallback    = Module.addFunction((state) => _onNoRightParenAddedCallback    (state), 'vi');
        var fSetExpressionDisplayCallback   = Module.addFunction((state) => _setExpressionDisplayCallback   (state), 'vi');
        var fSetMemorizedNumbersCallback    = Module.addFunction((state, numbers) => _setMemorizedNumbersCallback(state, numbers), 'vii');

        var ret = `${fGetCEngineStringCallback};`
            + `${fBinaryOperatorReceivedCallback};`
            + `${fSetPrimaryDisplayCallback};`
            + `${fSetIsInErrorCallback};`
            + `${fSetParenthesisNumberCallback};`
            + `${fMaxDigitsReachedCallback};`
            + `${fMemoryItemChangedCallback};`
            + `${fOnHistoryItemAddedCallback};`
            + `${fOnNoRightParenAddedCallback};`
            + `${fSetExpressionDisplayCallback};`
            + `${fSetMemorizedNumbersCallback};`
            ;

        return ret;
    }
}
