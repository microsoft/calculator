// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace Graphing
{
    public enum LocalizationType
    {
        Unknown,
        DecimalPointAndListComma,
        DecimalPointAndListSemicolon,
        DecimalCommaAndListSemicolon
    }

    public enum EquationParsingMode
    {
        SolveEquation,
        GraphEquation,
        NonEquation,
        DoNotCare
    }

    public enum FormatType
    {
        Formula,
        InvariantFormula,
        FormulaWithoutAggregate,
        Linear,
        LinearInput,
        MathML,
        MathMLNoWrapper,
        MathRichEdit,
        InlineMathRichEdit,
        Binary,
        InvariantBinary,
        Base64,
        InvariantBase64,
        Latex
    }

    public enum EvalNumberField
    {
        Invalid,
        Real,
        Complex
    }

    public enum EvalExpandMode
    {
        Neutral,
        Expand,
        Factor
    }

    public enum EvalTrigUnitMode
    {
        Invalid,
        Radians,
        Degrees,
        Grads
    }

    [Flags]
    public enum ContextualActionType
    {
        None = 0,
        SolveEquation = 1,
        Compare = 2,
        Expand = 3,
        Graph2D = 4,
        ListGraph2D = 5,
        GraphBothSides2D = 6,
        Graph3D = 7,
        GraphBothSides3D = 8,
        GraphInequality = 9,
        Assign = 10,
        Factor = 11,
        Deriv = 12,
        IndefiniteIntegral = 13,
        Graph2DExpression = 14,
        Graph3DExpression = 15,
        SolveInequality = 16,
        Calculate = 17,
        Round = 18,
        Floor = 19,
        Ceiling = 20,
        MatrixMask = 0x1000,
        MatrixDeterminant = MatrixMask | 1,
        MatrixInverse = MatrixMask | 2,
        MatrixTrace = MatrixMask | 3,
        MatrixTranspose = MatrixMask | 4,
        MatrixSize = MatrixMask | 5,
        MatrixReduce = MatrixMask | 6,
        ListMask = 0x2000,
        ListSort = ListMask | 1,
        ListMean = ListMask | 2,
        ListMedian = ListMask | 3,
        ListMode = ListMask | 4,
        ListLcm = ListMask | 5,
        ListGcf = ListMask | 6,
        ListSum = ListMask | 7,
        ListProduct = ListMask | 8,
        ListMax = ListMask | 9,
        ListMin = ListMask | 10,
        ListVariance = ListMask | 11,
        ListStdDev = ListMask | 12,
        ShowVerboseSolution = 30,
        TypeMask = 0xFFFF,
        Informational = 0x10000
    }

    public enum MathActionCategoryType
    {
        Unknown,
        Calculate,
        Solve,
        Integrate,
        Differentiate,
        Algebra,
        Matrix,
        List,
        Graph
    }

    public enum StepSequenceType
    {
        None,
        Text,
        Expression,
        NewLine,
        NewStep,
        Conditional,
        Composite,
        Goto,
        Call,
        Return,
        Stop,
        Error,
        GotoTemp
    }

    public enum FormatVerbosityMode
    {
        Verbose,
        Simple
    }

    namespace Renderer
    {
        public enum ChangeRangeAction
        {
            ZoomIn,
            ZoomOut,
            WidenX,
            ShrinkX,
            WidenY,
            ShrinkY,
            WidenZ,
            ShrinkZ,
            MoveNegativeX,
            MovePositiveX,
            MoveNegativeY,
            MovePositiveY,
            MoveNegativeZ,
            MovePositiveZ,
            SmoothZoomIn,
            SmoothZoomOut,
            PinchZoomIn,
            PinchZoomOut
        }

        public enum LineStyle
        {
            Solid,
            Dot,
            Dash,
            DashDot,
            DashDotDot
        }
    }

    namespace Analyzer
    {
        public enum GraphAnalyzerMessage
        {
            GraphAnalyzerMessage_None = 0,
            GraphAnalyzerMessage_NoZeros = 1,
            GraphAnalyzerMessage_NoYIntercept = 2,
            GraphAnalyzerMessage_NoMinima = 3,
            GraphAnalyzerMessage_NoMaxima = 4,
            GraphAnalyzerMessage_NoInflectionPoints = 5,
            GraphAnalyzerMessage_NoVerticalAsymptotes = 6,
            GraphAnalyzerMessage_NoHorizontalAsymptotes = 7,
            GraphAnalyzerMessage_NoObliqueAsymptotes = 8,
            GraphAnalyzerMessage_NotAbleToCalculate = 9,
            GraphAnalyzerMessage_NotAbleToMarkAllGraphFeatures = 10,
            GraphAnalyzerMessage_TheseFeaturesAreTooComplexToCalculate = 11,
            GraphAnalyzerMessage_ThisFeatureIsTooComplexToCalculate = 12
        }

        public enum AnalysisType
        {
            AnalysisType_Domain = 0,
            AnalysisType_Range = 1,
            AnalysisType_Parity = 2,
            AnalysisType_Zeros = 3,
            AnalysisType_YIntercept = 4,
            AnalysisType_Minima = 5,
            AnalysisType_Maxima = 6,
            AnalysisType_InflectionPoints = 7,
            AnalysisType_VerticalAsymptotes = 8,
            AnalysisType_HorizontalAsymptotes = 9,
            AnalysisType_ObliqueAsymptotes = 10,
            AnalysisType_Monotonicity = 11,
            AnalysisType_Period = 12
        }

        [Flags]
        public enum PerformAnalysisType
        {
            PerformAnalysisType_Domain = 0x01,
            PerformAnalysisType_Range = 0x02,
            PerformAnalysisType_Parity = 0x04,
            PerformAnalysisType_InterceptionPointsWithXAndYAxis = 0x08,
            PerformAnalysisType_CriticalPoints = 0x10,
            PerformAnalysisType_Asymptotes = 0x20,
            PerformAnalysisType_Monotonicity = 0x40,
            PerformAnalysisType_Period = 0x80,
            PerformAnalysisType_All = 0xFF
        }

        public enum FunctionParityType
        {
            FunctionParityType_Unknown = 0,
            FunctionParityType_Odd = 1,
            FunctionParityType_Even = 2,
            FunctionParityType_None = 3
        }

        public enum FunctionMonotonicityType
        {
            FunctionMonotonicityType_Unknown = 0,
            FunctionMonotonicityType_Ascending = 1,
            FunctionMonotonicityType_Descending = 2,
            FunctionMonotonicityType_Constant = 3
        }

        public enum AsymptoteType
        {
            AsymptoteType_Unknown = 0,
            AsymptoteType_PositiveInfinity = 1,
            AsymptoteType_NegativeInfinity = 2,
            AsymptoteType_AnyInfinity = 3
        }

        public enum FunctionPeriodicityType
        {
            FunctionPeriodicityType_Unknown = 0,
            FunctionPeriodicityType_Periodic = 1,
            FunctionPeriodicityType_NotPeriodic = 2
        }
    }
}
