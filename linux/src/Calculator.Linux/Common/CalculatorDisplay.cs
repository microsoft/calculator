// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux port of CalculatorDisplay: subscribes to CalcEngine events and
// forwards them to the ViewModels via weak references (same shape as
// upstream, but driven by C# events instead of WinRT delegates).
using Calculator.Linux.Interop;

namespace Calculator.Linux.Common;

internal sealed class CalculatorDisplay
{
    private readonly CalcEngine _engine;
    private WeakReference<ICalcDisplayTarget>? _callbackReference;
    private WeakReference<IHistoryDisplayTarget>? _historyCallbackReference;

    public CalculatorDisplay(CalcEngine engine)
    {
        _engine = engine;
        _engine.PrimaryDisplayChanged += (display, isError) =>
        {
            if (_callbackReference?.TryGetTarget(out var vm) == true)
            {
                vm.SetPrimaryDisplay(display, isError);
            }
        };
        _engine.IsInErrorChanged += isError =>
        {
            if (_callbackReference?.TryGetTarget(out var vm) == true)
            {
                vm.SetIsInError(isError);
            }
        };
        _engine.ExpressionChanged += () =>
        {
            if (_callbackReference?.TryGetTarget(out var vm) == true)
            {
                vm.SetExpressionDisplay(new List<(string Token, int CommandIndex, int CommandType)>(_engine.ExpressionTokens));
            }
        };
        _engine.ParenthesesChanged += count =>
        {
            if (_callbackReference?.TryGetTarget(out var vm) == true)
            {
                vm.SetParenthesisCount(count);
            }
        };
        _engine.NoRightParenAdded += () =>
        {
            if (_callbackReference?.TryGetTarget(out var vm) == true)
            {
                vm.OnNoRightParenAdded();
            }
        };
        _engine.MaxDigitsReached += () =>
        {
            if (_callbackReference?.TryGetTarget(out var vm) == true)
            {
                vm.OnMaxDigitsReached();
            }
        };
        _engine.BinaryOperatorReceived += () =>
        {
            if (_callbackReference?.TryGetTarget(out var vm) == true)
            {
                vm.OnBinaryOperatorReceived();
            }
        };
        _engine.HistoryItemAdded += index =>
        {
            if (_historyCallbackReference?.TryGetTarget(out var histVM) == true)
            {
                histVM.OnHistoryItemAdded(index);
            }
        };
        _engine.MemoryChanged += changedIndex =>
        {
            if (_callbackReference?.TryGetTarget(out var vm) == true)
            {
                vm.SyncMemorizedNumbers(_engine.MemorizedNumbers());
                if (changedIndex >= 0)
                {
                    vm.OnMemoryItemChanged((uint)changedIndex);
                }
            }
        };
        _engine.InputChanged += () =>
        {
            if (_callbackReference?.TryGetTarget(out var vm) == true)
            {
                vm.OnInputChanged();
            }
        };
    }

    public void SetCallback(ICalcDisplayTarget viewModel)
    {
        _callbackReference = new WeakReference<ICalcDisplayTarget>(viewModel);
    }

    public void SetHistoryCallback(IHistoryDisplayTarget historyViewModel)
    {
        _historyCallbackReference = new WeakReference<IHistoryDisplayTarget>(historyViewModel);
    }
}

internal interface ICalcDisplayTarget
{
    void SetPrimaryDisplay(string display, bool isError);
    void SetIsInError(bool isError);
    void SetExpressionDisplay(List<(string Token, int CommandIndex, int CommandType)> tokens);
    void SetParenthesisCount(uint count);
    void OnNoRightParenAdded();
    void OnMaxDigitsReached();
    void OnBinaryOperatorReceived();
    void SyncMemorizedNumbers(List<string> memorizedNumbers);
    void OnMemoryItemChanged(uint indexOfMemory);
    void OnInputChanged();
}

internal interface IHistoryDisplayTarget
{
    void OnHistoryItemAdded(uint addedItemIndex);
}
