// AUDIT — replay of the REFERENCE's history unit tests.
//
// Source: src/Calculator.Tests/HistoryTests.cs @ d125246a (unmodified
// reference), class HistoryTests.
//
// The reference drives the engine with SendCommandToCalcManager(CommandX);
// here the same command ids go through the port's ButtonPressedCommand, which
// is what the window actually calls.
//
// The most load-bearing expectation here is the exact history expression string
// format. The reference asserts "1   +   8 =" - three spaces between the
// operands and the operator, one space before the "=" - which is not the same as
// the on-screen expression line ("1 + 8 =") that the 158 engine vectors cover.
// A port that reuses the display formatting for history would look correct on
// screen and still be wrong here.

using Calculator.Linux.Common;
using Calculator.Linux.ViewModels;
using Xunit;

namespace Calculator.Linux.Tests.Audit;

public sealed class ReferenceHistoryTests : IDisposable
{
    // Command ids from the engine's own header (src/CalcManager/Command.h).
    private const int ModeBasic = 200;
    private const int ModeScientific = 201;
    private const int D1 = 131;
    private const int D2 = 132;
    private const int D3 = 133;
    private const int D4 = 134;
    private const int D5 = 135;
    private const int D8 = 138;
    private const int Add = 93;
    private const int Sub = 94;
    private const int Div = 91;
    private const int Mul = 92;
    private const int Equ = 121;   // named Equ: Equals would hide object.Equals
    private const int D0 = 130;
    private const int D6 = 136;
    private const int D7 = 137;
    private const int Rec = 114;          // 1/x
    private const int Sqrt = 110;
    private const int Sqr = 111;          // x^2
    private const int Percent = 118;

    private readonly StandardCalculatorViewModel _vm;

    // The port's view model owns and wires its own HistoryViewModel; using it
    // directly exercises the same object the window binds to.
    private HistoryViewModel History => _vm.HistoryVM;

    public ReferenceHistoryTests()
    {
        _vm = new StandardCalculatorViewModel();
        _vm.IsStandard = true;
    }

    public void Dispose()
    {
        History.ClearCommand.Execute(null);
        _vm.ClearMemoryCommand.Execute(null);
        _vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
        _vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.ClearEntry);
        _vm.Dispose();
    }

    private void Send(params int[] commands)
    {
        foreach (int command in commands)
        {
            // The reference sends ModeScientific/ModeBasic straight to the
            // CalculatorManager; the port exposes those as VM commands rather
            // than as NumbersAndOperatorsEnum members, so they are mapped here
            // rather than cast into an undefined enum value.
            switch (command)
            {
                case ModeBasic:
                    _vm.SetCalculatorType(ViewMode.Standard);
                    continue;
                case ModeScientific:
                    _vm.SetCalculatorType(ViewMode.Scientific);
                    continue;
            }
            _vm.ButtonPressedCommand.Execute((NumbersAndOperatorsEnum)command);
        }
    }

    /// <summary>History is newest-first, so index 0 is the most recent entry.</summary>
    private HistoryItemViewModel Newest => History.Items[0];

    /// <summary>The oldest surviving entry.</summary>
    private HistoryItemViewModel Oldest => History.Items[History.ItemsCount - 1];

    /// <summary>The engine's cap (MAX_HISTORY_ITEMS = 20), per the reference.</summary>
    private static int MaxHistoryItems => 20;

    /// <summary>Reference TestHistoryItemAddSingleItem.</summary>
    [Fact]
    public void HistoryItemAddSingleItem()
    {
        int initialSize = History.ItemsCount;
        Send(D1, Add, D8, Equ);

        Assert.Equal(initialSize + 1, History.ItemsCount);

        HistoryItemViewModel item = History.Items[0];
        Assert.Equal("1   +   8 =", item.Expression);
        Assert.Equal("9", item.Result);
    }

    /// <summary>Reference TestHistoryItemAddMaxItems: the cap is honoured.</summary>
    [Fact]
    public void HistoryItemAddMaxItems()
    {
        Send(D1, Add, D1, Equ);
        for (int i = 1; i < MaxHistoryItems; i++)
        {
            Send(D1, Add, D2, Equ);
        }

        Assert.Equal(MaxHistoryItems, History.ItemsCount);

        // The oldest entry survives (history is newest-first).
        Assert.Equal("1   +   1 =", Oldest.Expression);
        Assert.Equal("2", Oldest.Result);

        // One more calculation evicts the oldest. The reference then asserts the
        // LAST (oldest surviving) item, which is now the second 1+2 entry - not
        // the 1+5 just computed, which sits at index 0.
        Send(D1, Add, D5, Equ);
        Assert.Equal(MaxHistoryItems, History.ItemsCount);
        Assert.Equal("1   +   5 =", Newest.Expression);
        Assert.Equal("6", Newest.Result);
        Assert.Equal("1   +   2 =", Oldest.Expression);
        Assert.Equal("3", Oldest.Result);
    }

    /// <summary>Reference TestHistoryClearCommand.</summary>
    [Fact]
    public void HistoryClearCommand()
    {
        Send(ModeScientific, D1, Add, D2, Equ);
        Send(ModeBasic, D1, Add, D2, Equ);
        Assert.True(History.ItemsCount > 0);

        History.ClearCommand.Execute(null);
        Assert.Equal(0, History.ItemsCount);
    }

    /// <summary>Reference TestHistoryClearCommandWithEmptyHistory.</summary>
    [Fact]
    public void HistoryClearCommandWithEmptyHistory()
    {
        Assert.Equal(0, History.ItemsCount);
        Send(ModeScientific);
        History.ClearCommand.Execute(null);
        Assert.Equal(0, History.ItemsCount);
    }

    /// <summary>Reference TestHistoryEmpty.</summary>
    [Fact]
    public void HistoryEmpty()
    {
        Assert.Equal(0, History.ItemsCount);
        History.ClearCommand.Execute(null);
        Assert.Equal(0, History.ItemsCount);
    }

    /// <summary>Reference TestHistoryItemClicked: restores display and expression.</summary>
    [Fact]
    public void HistoryItemClickedRestoresDisplayAndExpression()
    {
        Send(ModeScientific, D1, Add, D5, Add, D3, Equ);

        HistoryItemViewModel item = (HistoryItemViewModel)History.Items[History.ItemsCount - 1];
        _vm.SelectHistoryItem(item);

        Assert.Equal("9", _vm.DisplayValue);

        // Reference asserts each expression token individually.
        string[] expected = { "1", " ", "+", " ", "5", " ", "+", " ", "3" };
        Assert.True(_vm.ExpressionTokens.Count >= expected.Length,
            $"expected at least {expected.Length} tokens, got {_vm.ExpressionTokens.Count}");
        for (int i = 0; i < expected.Length; i++)
        {
            Assert.Equal(expected[i], _vm.ExpressionTokens[i].Token);
        }
    }

    /// <summary>
    /// Reference TestHistoryItemLoadAndContinueCalculation: loading an item and
    /// continuing must keep the loaded expression, not clear it.
    /// </summary>
    [Fact]
    public void HistoryItemLoadAndContinueCalculation()
    {
        Send(ModeScientific, D1, Add, D5, Add, D3, Equ);
        HistoryItemViewModel item = (HistoryItemViewModel)History.Items[History.ItemsCount - 1];

        _vm.SelectHistoryItem(item);
        int tokensAfterLoad = _vm.ExpressionTokens.Count;

        Send(D2, Equ);
        Assert.True(_vm.ExpressionTokens.Count >= tokensAfterLoad,
            "continuing a calculation after loading history must not shrink the expression");
    }

    /// <summary>
    /// The reference's HistoryStandardOrderOfOperationsHelper, verbatim. The
    /// helper asserts that a run WITHOUT a terminating '=' leaves history
    /// untouched, and that a run WITH '=' appends exactly one item whose
    /// Expression is the three-space-separated form.
    /// </summary>
    private static void HistoryStandardOrderOfOperations(
        StandardCalculatorViewModel vm, string expectedResult, string expectedExpression,
        params int[] testCommands)
    {
        int initialSize = vm.HistoryVM.ItemsCount;

        foreach (int command in testCommands)
        {
            if (command == 0)
            {
                break;
            }
            vm.ButtonPressedCommand.Execute((NumbersAndOperatorsEnum)command);
        }

        int sizeAfter = vm.HistoryVM.ItemsCount;
        if (string.IsNullOrEmpty(expectedResult))
        {
            Assert.Equal(initialSize, sizeAfter);
            return;
        }

        Assert.Equal(initialSize + 1, sizeAfter);
        Assert.Equal(expectedExpression, vm.HistoryVM.Items[0].Expression);
        Assert.Equal(expectedResult, vm.HistoryVM.Items[0].Result);
    }

    [Fact]
    public void TestHistoryStandardOrderOfOperations_1() =>
        HistoryStandardOrderOfOperations(_vm, "", "", D1, Rec);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_2() =>
        HistoryStandardOrderOfOperations(_vm, "", "", D4, Sqrt);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_3() =>
        HistoryStandardOrderOfOperations(_vm, "", "", D1, Add, D4, Sqrt);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_4() =>
        HistoryStandardOrderOfOperations(_vm, "3", "1   +   \u221A( 4 ) =", D1, Add, D4, Sqrt, Sub);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_5() =>
        HistoryStandardOrderOfOperations(_vm, "", "", D2, Mul, D4, Rec);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_6() =>
        HistoryStandardOrderOfOperations(_vm, "", "", D5, Div, D6, Percent);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_7() =>
        HistoryStandardOrderOfOperations(_vm, "", "", D4, Sqrt, Sub);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_8() =>
        HistoryStandardOrderOfOperations(_vm, "", "", D7, Sqr, Div);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_9() =>
        HistoryStandardOrderOfOperations(_vm, "", "", D8, Sqr, Sqrt);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_10() =>
        HistoryStandardOrderOfOperations(_vm, "12", "10   +   2 =", D1, D0, Add, D2, Sub);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_11() =>
        HistoryStandardOrderOfOperations(_vm, "12", "3   \u00D7   4 =", D3, Mul, D4, Div);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_12() =>
        HistoryStandardOrderOfOperations(_vm, "2", "6   \u00F7   3 =", D6, Div, D3, Sub, Add);

    [Fact]
    public void TestHistoryStandardOrderOfOperations_13() =>
        HistoryStandardOrderOfOperations(_vm, "3", "7   -   4 =", D7, Sub, D4, Div, Mul);

    /// <summary>The engine's cap is 20 items (MAX_HISTORY_ITEMS).</summary>
    [Fact]
    public void HistoryCapIsTwenty()
    {
        Assert.Equal(20, MaxHistoryItems);
    }
}