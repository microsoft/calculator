// AUDIT — keyboard shortcut parity and layout independence.
//
// Reference: src/Calculator/Views/*.xaml + Resources.resw declare 71 Button
// shortcuts via KeyboardShortcutManager.Character / .VirtualKey (extracted to
// AUDIT/evidence/reference_shortcuts.json).
//
// Defects guarded:
//  AUD-S3-004  the port mapped hardware Key codes only (OemPlus, OemMinus, ...)
//               so every operator shortcut depended on the physical key
//               position and broke on AZERTY/QWERTZ/non-Latin layouts. The brief
//               (addendum A2 item 3) requires the typed character.
//  AUD-S3-005  roughly a dozen of the reference's character shortcuts were
//               implemented; the rest were unreachable from the keyboard.
//
// The window is driven through Avalonia.Headless KeyPress using the
// 4-argument overload so the CHARACTER is what is under test, not a US key code.
//
// NOTE: the native engine keeps process-global state (the whole test assembly
// is serialised for that reason), so a freshly constructed window inherits the
// previous one's display. End-to-end window tests therefore share one window.

using System.Reflection;
using Avalonia.Controls;
using Avalonia.Headless;
using Avalonia.Headless.XUnit;
using Avalonia.Input;
using Avalonia.LogicalTree;
using Calculator.Linux.Common;
using Xunit;
using Xunit.Abstractions;

namespace Calculator.Linux.Tests.Audit;

public sealed class KeyboardShortcutParityTests : IDisposable
{
    private readonly ITestOutputHelper _o;
    private readonly MainWindow _window;

    public KeyboardShortcutParityTests(ITestOutputHelper o)
    {
        _o = o;
        _window = new MainWindow { Width = 1100, Height = 900 };
        _window.Show();
    }

    public void Dispose()
    {
        _window.Close();
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
    }

    private void SelectMode(string mode)
    {
        var list = _window.GetLogicalDescendants().OfType<ListBox>().First(c => c.Name == "ModeList");
        list.SelectedItem = list.Items.OfType<ListBoxItem>().First(i => i.Tag as string == mode);
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
        _window.KeyPress(Key.Escape, RawInputModifiers.None, PhysicalKey.Escape, string.Empty);
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
    }

    private void Press(char c)
    {
        Key key = c switch
        {
            >= '0' and <= '9' => Key.D0 + (c - '0'),
            _ => Key.A, // deliberately a non-US key: only KeySymbol can route it
        };
        _window.KeyPress(key, RawInputModifiers.None, PhysicalKey.None, c.ToString());
    }

    private string Display()
    {
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
        return _window.DataContext!.GetType().GetProperty("DisplayValue")!.GetValue(_window.DataContext) as string ?? "";
    }

    // ---------------- end-to-end through the real window ----------------

    [AvaloniaFact]
    public void Scientific_Pi_Shortcut()
    {
        SelectMode("Scientific");
        Press('p');
        Assert.StartsWith("3.14159", Display());
    }

    [AvaloniaFact]
    public void Scientific_Sin_Shortcut()
    {
        SelectMode("Scientific");
        Press('9'); Press('0'); Press('s');
        Assert.Equal("1", Display());              // sin(90 deg)
    }

    [AvaloniaFact]
    public void Scientific_Sqrt_Shortcut()
    {
        SelectMode("Scientific");
        Press('9'); Press('@');
        Assert.Equal("3", Display());
    }

    [AvaloniaFact]
    public void Scientific_Factorial_Shortcut()
    {
        SelectMode("Scientific");
        Press('5'); Press('!');
        Assert.Equal("120", Display());
    }

    [AvaloniaFact]
    public void OperatorsWorkByCharacter_NotByPhysicalKey()
    {
        // '+' is Key.OemPlus on US and something else on QWERTZ/AZERTY. Pressing
        // it with a non-US Key code can only work if the handler reads KeySymbol.
        SelectMode("Standard");
        Press('1'); Press('+'); Press('2');
        _window.KeyPress(Key.Enter, RawInputModifiers.None, PhysicalKey.Enter, string.Empty); // reference binds '=' to VK:Enter
        Assert.Equal("3", Display());
    }

    // ---------------- table contents (order-independent) ----------------

    private static IReadOnlyDictionary<char, NumbersAndOperatorsEnum> Table(string name)
    {
        var field = typeof(MainWindow).GetField(name, BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(field);
        return (IReadOnlyDictionary<char, NumbersAndOperatorsEnum>)field!.GetValue(null)!;
    }

    /// <summary>The reference's scientific character shortcuts, by name.</summary>
    public static TheoryData<char, string> ScientificShortcuts() => new()
    {
        { '!', nameof(NumbersAndOperatorsEnum.Factorial) },
        { '#', nameof(NumbersAndOperatorsEnum.Cube) },
        { '(', nameof(NumbersAndOperatorsEnum.OpenParenthesis) },
        { ')', nameof(NumbersAndOperatorsEnum.CloseParenthesis) },
        { '*', nameof(NumbersAndOperatorsEnum.Multiply) },
        { '+', nameof(NumbersAndOperatorsEnum.Add) },
        { '-', nameof(NumbersAndOperatorsEnum.Subtract) },
        { '/', nameof(NumbersAndOperatorsEnum.Divide) },
        { '@', nameof(NumbersAndOperatorsEnum.Sqrt) },
        { '^', nameof(NumbersAndOperatorsEnum.XPowerY) },
        { '[', nameof(NumbersAndOperatorsEnum.Floor) },
        { ']', nameof(NumbersAndOperatorsEnum.Ceil) },
        { '|', nameof(NumbersAndOperatorsEnum.Abs) },
        { '%', nameof(NumbersAndOperatorsEnum.Mod) },
        { 'g', nameof(NumbersAndOperatorsEnum.TwoPowerX) },
        { 'i', nameof(NumbersAndOperatorsEnum.Csc) },
        { 'j', nameof(NumbersAndOperatorsEnum.Cot) },
        { 'l', nameof(NumbersAndOperatorsEnum.LogBase10) },
        { 'm', nameof(NumbersAndOperatorsEnum.DMS) },
        { 'n', nameof(NumbersAndOperatorsEnum.LogBaseE) },
        { 'o', nameof(NumbersAndOperatorsEnum.Cos) },
        { 'p', nameof(NumbersAndOperatorsEnum.Pi) },
        { 'q', nameof(NumbersAndOperatorsEnum.XPower2) },
        { 'r', nameof(NumbersAndOperatorsEnum.Invert) },
        { 's', nameof(NumbersAndOperatorsEnum.Sin) },
        { 't', nameof(NumbersAndOperatorsEnum.Tan) },
        { 'u', nameof(NumbersAndOperatorsEnum.Sec) },
        { 'x', nameof(NumbersAndOperatorsEnum.Exp) },
        { 'y', nameof(NumbersAndOperatorsEnum.XPowerY) },
    };

    public static TheoryData<char, string> ProgrammerShortcuts() => new()
    {
        { '&', nameof(NumbersAndOperatorsEnum.And) },
        { '|', nameof(NumbersAndOperatorsEnum.Or) },
        { '~', nameof(NumbersAndOperatorsEnum.Not) },
        { '^', nameof(NumbersAndOperatorsEnum.Xor) },
        { '\\', nameof(NumbersAndOperatorsEnum.Nor) },
        { '.', nameof(NumbersAndOperatorsEnum.Nand) },
        { '<', nameof(NumbersAndOperatorsEnum.Lsh) },
        { '>', nameof(NumbersAndOperatorsEnum.Ror) },
    };

    [AvaloniaTheory]
    [MemberData(nameof(ScientificShortcuts))]
    public void ScientificShortcutTable_MatchesReference(char shortcut, string operation)
    {
        Assert.True(Table("s_scientificChars").TryGetValue(shortcut, out var op),
            $"scientific shortcut '{shortcut}' missing (reference: {operation})");
        Assert.Equal(operation, op.ToString());
    }

    [AvaloniaTheory]
    [MemberData(nameof(ProgrammerShortcuts))]
    public void ProgrammerShortcutTable_MatchesReference(char shortcut, string operation)
    {
        Assert.True(Table("s_programmerChars").TryGetValue(shortcut, out var op),
            $"programmer shortcut '{shortcut}' missing (reference: {operation})");
        Assert.Equal(operation, op.ToString());
    }


}