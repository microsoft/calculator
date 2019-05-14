#pragma once
namespace CalculatorApp
{
    public enum TraceLoggerCalculatorModeEnum
    {
        Standard,
        Scientific,
        Programmer,
        DateCalculation,
        CurrencyConverter,
        VolumeConverter,
        LengthConverter,
        WeightMassConverter,
        TemperatureConverter,
        EnergyConverter,
        AreaConverter,
        SpeedConverter,
        TimeConverter, 
    };


    public enum TraceLoggerButtonIdEnum
    {
        // Numberpad
        ZeroButton,
        OneButton,
        TwoButton,
        ThreeButton,
        FourButton,
        FiveButton,
        SixButton,
        SevenButton,
        EightButton,
        NineButton,
        DecimalButton,

        // Standard Operators
        NegateButton,
        EqualsButton,
        PlusButton,
        MinusButton,
        MultiplyButton,
        DivideButton,
        PercentButton,
        SquareRootButton,
        XPower2Button,
        XPower3Button,
        OpenParenthesisButton,
        CloseParenthesisButton,

        // Display Controls
        BackspaceButton,
        ClearButton,
        ClearEntryButton,

        // Scientific Operators
        InvertButton,
        SinButton,
        CosButton,
        TanButton,
        InvsinButton,
        InvcosButton,
        InvtanButton,
        SinhButton,
        CoshButton,
        TanhButton,
        InvsinhButton,
        InvcoshButton,
        InvtanhButton,
        LogBase10Button,
        ExpButton,
        ModButton,
        LogBaseEButton,
        DmsButton,
        DegreesButton,
        FactorialButton,
        PowerOf10Button,
        PowerOfEButton,
        ShiftButton,
        PiButton,
        YSquareRootButton,
        PowerButton,

        // Programmer Operators
        RolButton,
        RorButton,
        LshButton,
        RshButton,
        OrButton,
        XorButton,
        NotButton,
        AndButton,

        // Programmer Numberpad
        AButton,
        BButton,
        CButton,
        DButton,
        EButton,
        FButton
    };

    public enum TraceLoggerInputMethodEnum
    {
            PhysicalKeyboard,
            OnScreenKeyboard 
    };
}
