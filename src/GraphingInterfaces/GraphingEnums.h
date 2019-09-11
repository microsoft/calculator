#pragma once

namespace Graphing
{
    enum class LocalizationType
    {
        Unknown,
        DecimalPointAndListComma,
        DecimalPointAndListSemicolon,
        DecimalCommaAndListSemicolon
    };

    enum class EquationParsingMode
    {
        // Solving an equation. At least one equal sign is required        
        SolveEquation,

        // Graphing an equation. At least one equal sign is required        
        GraphEquation,

        // Not expecting an equation. No equal sign is allowed
        NonEquation,

        // Accept zero or more equal signs
        DoNotCare
    };

    enum class FormatType
    {
        // This format is not very human-readable, but best for machine processing and unit test.
        // The meaning of an expression is precise, there is no need for parentheses, and it's easy to parse.
        // While other serializers may contain little tweaks to make the UI component work as expected, this 
        // format doesn't change often because no real UI uses it.        
        // Example: Sum[1,Divide[2,x]]        
        Formula,

        // Similar to Formula, except the variables are in the format of Var(0), Var(1), ... instead of in their names.
        // Used in serialization only (CasContext.FormatOptions) but not in parsing (CasContext.ParsingOptions)
        // Example: Sum[1,Divide[2,Var(0)]]
        InvariantFormula,

        // Similar to Formula, except the aggregates (grouping parentheses) are silently removed during serialization. 
        // The primary usage for this format is in internal test cases.
        // When used in parsing, it's identical to Formula        
        // Example: Sum[1,Divide[2,x]]
        FormulaWithoutAggregate,

        // This format is the most human-readable one. Can be used in command line applications or application that accept
        // and displays linear syntax. Also, the RichEdit wrapper returns linear syntax and it should be parsed with Linear format type.
        // Example: 1+2/x
        Linear,

        // This format is similar to linear format, but during serialization it uses ASCII characters only (except for variable 
        // named specified by the user) and can round trip back to the linear parser.
        // 
        // When used in parsing it's identical to Linear.        
        LinearInput,

        // The standard MathML format. Note that the math engine can only parse a subset of the syntax in the MathML specification.
        // Example: &lt;math&gt;&lt;mn&gt;1&lt;/mn&gt;&lt;mo&gt;+&lt;/mo&gt;&lt;mfrac&gt;&lt;mn&gt;2&lt;/mn&gt;&lt;mi&gt;x&lt;/mi&gt;&lt;/mfrac&gt;&lt;/math&gt;
        MathML,

        // Same as MathML, except this format type won't generate the root element &lt;math&gt; . 
        // Used in serialization only (CasContext.FormatOptions) but not in parsing (CasContext.ParsingOptions)
        // Example: &lt;mn&gt;1&lt;/mn&gt;&lt;mo&gt;+&lt;/mo&gt;&lt;mfrac&gt;&lt;mn&gt;2&lt;/mn&gt;&lt;mi&gt;x&lt;/mi&gt;&lt;/mfrac&gt;
        MathMLNoWrapper,

        // This format type is for the RichEdit wrapper to render math in RichEdit.
        // 
        // Used in serialization only (CasContext.FormatOptions) but not in parsing (CasContext.ParsingOptions)
        // Example: 1 + \frac{2,x}
        MathRichEdit,

        // This format type is for the RichEdit wrapper to render math in RichEdit.
        // It's same with MathRichEdit format with one exception: fractions are rendered 
        // horizontally instead of vertically. This is a better choice if the display area
        // is confined with a small height.
        //
        // Used in serialization only (CasContext.FormatOptions) but not in parsing (CasContext.ParsingOptions)
        // Example: 1 + x/(2 a)
        InlineMathRichEdit,

        // The most compact format. Uses binary data. Like Format, it guarantees round trip between parsing and serialization.
        Binary,

        // Similar to Binary, except variableIds are used instead of variable name.
        InvariantBinary,

        // This is the base-64 encoded Binary format
        Base64,

        // This is the base-64 encoded InvariantBinary format.
        InvariantBase64,

        // Latex format
        // Example: \frac{1}{2}
        Latex
    };

    // Specify on what number field the evaluation should be performed: Real or Complex.
    enum class EvalNumberField
    {
        // This is invalid setting.
        Invalid,

        // Evaluation should be performed on Real number field
        Real,

        // Evaluation should be performed on Complex number field
        Complex
    };

    // Specify the evaluation direction: Expand, Factor or Neutral (neither Expand nor Factor)
    enum class EvalExpandMode
    {
        // Neither Expand nor Factor
        Neutral,

        // To expand during evaluation
        Expand,

        // To factor during evaluation
        Factor
    };

    // Specify the current trigonometry unit mode: Radians, Degrees, Grads. This has effect on these trig operators:
    // Sin Cos Tan Cot Sec Csc
    // ASin ACos ATan ACot ASec ACsc. 
    // It has NO effect on hyperbolic trig operators (same behavior as Windows Calc), and any other operators.
    enum class EvalTrigUnitMode
    {
        // Invalid value.
        Invalid = 3,

        // Default trig unit. Period of sin is 2pi
        Radians = 0,

        // Degrees as trig unit. Period of sin is 360 degrees
        Degrees = 1,

        // Grads as trig unit. Period of sin is 400 grads
        Grads = 2
    };

    // Specifies the type of contextual action
    enum class ContextualActionType
    {
        // The input didn't generate any contextual action
        None,

        // Solve equation
        SolveEquation,

        // perform comparison
        Compare,

        // Expand the expression
        Expand,

        // Perform a 2D graphing
        Graph2D,

        // Perform a 2D graphing on all items in the list
        ListGraph2D,

        // Perform 2D graphing of the two functions on both side of the equal sign or inequality sign
        GraphBothSides2D,

        // Perform a 3D graphing
        Graph3D,

        // Perform 3D graphing of the two functions on both side of the equal sign or inequality sign
        GraphBothSides3D,

        // Perform 2D inequality graphing
        GraphInequality,

        // Perform assignment
        Assign,

        // Factor the expression
        Factor,

        // Compute the derivate of the expression
        Deriv,

        // Compute the integral of the expression
        IndefiniteIntegral,

        // Perform 2D graph
        Graph2DExpression,

        // Perform 3D graph
        Graph3DExpression,

        // solve the inequality
        SolveInequality,

        // calculate/simplify the expression
        Calculate,

        // round of the number
        Round,

        // floor of the number
        Floor,

        // ceiling of the number
        Ceiling,

        // The bit mask for matrix related contextual action types. if (type &amp; MatrixMask)!=0 then the type is matrix related
        MatrixMask,

        // Compute the determinant of the matrix
        MatrixDeterminant,

        // Compute the inverse of the matrix
        MatrixInverse,

        // Compute the trace of the matrix
        MatrixTrace,

        // Compute the transpose of the matrix
        MatrixTranspose,

        // Compute the size of the matrix
        MatrixSize,

        // Compute the reduce of the matrix
        MatrixReduce,

        // The bit mask for list related contextual action types. if (type &amp; ListMask)!=0 then the type is list related
        ListMask,

        // Sort the list
        ListSort,

        // Compute the mean of the list
        ListMean,

        // Compute the median of the list
        ListMedian,

        // Compute the mode of the list
        ListMode,

        // Compute the LCM (least common multiplier) of the list
        ListLcm,

        // Compute the GCF (greatest common factor) of the list
        ListGcf,

        // Compute the sum of the list elements
        ListSum,

        // Compute the product of the list elements
        ListProduct,

        // Compute the max of the list elements
        ListMax,

        // Compute the min of the list elements
        ListMin,

        // Compute the variance of the list elements
        ListVariance,

        // Compute the standard deviation of the list elements
        ListStdDev,

        // Show complete (verbose) solution
        ShowVerboseSolution,

        // bit mask of action type. Can be used to remove the Informational flag
        TypeMask, // mask to get the type 

        // A flag that can be added onto any type. This is informational only that explains what the straight input would do.
        // No action should be performed
        Informational
    };

    enum class MathActionCategoryType
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
    };

    enum class StepSequenceType
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
    };

    enum class FormatVerbosityMode
    {
        Verbose,
        Simple
    };

    namespace Renderer
    {
        //   Used to indicate what action should be performed to change the range.
        enum class ChangeRangeAction
        {
            //   Zoom in on all axes by the predefined ratio
            ZoomIn,

            //   Zoom out on all axes by the predefined ratio
            ZoomOut,

            //   Zoom out on X axis only, leave the range of Y (and Z in 3D) unchanged
            WidenX,

            //   Zoom in on X axis only, leave the range of Y (and Z in 3D) unchanged
            ShrinkX,

            //   Zoom out on Y axis only, leave the range of X (and Z in 3D) unchanged
            WidenY,

            //   Zoom in on Y axis only, leave the range of X (and Z in 3D) unchanged
            ShrinkY,

            //   Zoom out on Z axis only, leave the range of X and Y unchanged. Apply to 3D graph only but not 2D graph.
            WidenZ,

            //   Zoom in on Z axis only, leave the range of X and Y unchanged. Apply to 3D graph only but not 2D graph.
            ShrinkZ,

            //   Move the view window of the graph towards the negative X axis.
            MoveNegativeX,

            //   Move the view window of the graph towards the positive X axis.
            MovePositiveX,

            //   Move the view window of the graph towards the negative Y axis.
            MoveNegativeY,

            //   Move the view window of the graph towards the positive Y axis.
            MovePositiveY,

            //   Move the view window of the graph towards the negative Z axis.
            MoveNegativeZ,

            //   Move the view window of the graph towards the positive Z axis.
            MovePositiveZ,

            //   Zoom in on all axes by the predefined ratio. The ratio is smaller than used in ZoomIn result in a smoother motion
            SmoothZoomIn,

            //   Zoom out on all axes by the predefined ratio. The ratio is smaller than used in ZoomIn result in a smoother motion
            SmoothZoomOut,

            //   Zoom in on all axes by the predefined ratio
            PinchZoomIn,

            //   Zoom out on all axes by the predefined ratio
            PinchZoomOut
        };

        enum class LineStyle
        {
            Solid,
            Dot,
            Dash,
            DashDot,
            DashDotDot
        };
    }
}
