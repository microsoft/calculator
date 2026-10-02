// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Windows.UI;

namespace GraphControl
{
    public enum EquationLineStyle : int
    {
        Solid,
        Dot,
        Dash,
        DashDot,
        DashDotDot
    }

    public enum ErrorType
    {
        Evaluation,
        Syntax,
        Abort,
    }

    public enum EvaluationErrorCode
    {
        // Number is too large. To see this error, assign a large number to variable a, then keep doing "a:=a*a" until it happens
        Overflow = 2,

        // doing trig in calculus and current mode is not Radians
        RequireRadiansMode = 3,

        // This is actually abort code generated as time limit reached.
        TooComplexToSolve = 4,

        // degree sign present and the current mode is not Degrees
        RequireDegreesMode = 5,

        // n! and n is not an non-negative integer or m/2
        FactorialInvalidArgument = -1,

        // n!! and n is not an non-negative integer or odd negative integers
        Factorial2InvalidArgument = -2,

        // n! and n is too big
        FactorialCannotPerformOnLargeNumber = -3,

        // x mod y, and one of them is not an integer
        ModuloCannotPerformOnFloat = -5,

        // below are equation solving specific errors

        // example1: sin(x)+x=2    example2: x^4-x^3-8x=9
        EquationTooComplexToSolveSymbolic = -7,

        // example1: x-x=3    example2: x^2=-1
        EquationHasNoSolution = -8,

        // only used for numeric solve where the algorithm doesn't converge
        EquationTooComplexToSolve = -9,

        // used when equation cannot be ploted
        EquationTooComplexToPlot = -10,

        // example1: 1/0
        DivideByZero = -15,

        // the inequality is too complex for the engine to solve
        InequalityTooComplexToSolve = -41,

        // the inequality has no solution
        InequalityHasNoSolution = -42,

        // An exression contains logical conditions that are mutually exclusive
        MutuallyExclusiveConditions = -43,

        // Arbitrary Precision Evaluation input is out of domain
        OutOfDomain = -101,

        // Not supported.
        GE_NotSupported = -503,

        // General error for graphing engine
        GE_GeneralError = -504,

        // Failed to calculate
        GE_TooComplexToSolve = -506,
    }

    public enum SyntaxErrorCode
    {
        // found ) without matching (
        ParenthesisMismatch = 1,

        // found ( without matching )
        UnmatchedParenthesis = 2,

        // more than 1 decimal point in a number. Example: 7.3.2
        TooManyDecimalPoints = 3,

        // decimal point on its own without any digits surrounding it. Example: 3+.+4
        DecimalPointWithoutDigits = 4,

        // example: 3-4*
        UnexpectedEndOfExpression = 5,

        // example: 3-*4
        UnexpectedToken = 6,

        // example: [    (or many other special characters), another example: "3,5" (comma is invalid here)
        InvalidToken = 7,

        // example: solve(x+3=8=x)
        TooManyEquals = 8,

        // example: ploteq(4+83=9)
        EqualWithoutGraphVariable = 10,

        // <para>example: ploteq(x+y)        (expecting "=" in equation ploting)</para>
        // <para>example2: Solve(5*x+9)    (expecting = in the equation solving)</para>
        InvalidEquationSyntax = 11,

        // there is nothing in the expression
        EmptyExpression = 12,

        // example: factor(x=3) (expecting solve(x=3)).
        EqualWithoutEquation = 14,

        // example: solve( (x=3)*2 )
        InvalidEquationFormat = 15,

        // This error only occurs when CasContext.ParsingOptions.AllowImplicitParentheses == false.
        // example: sin a    (expecting sin(a))
        ExpectParenthesisAfterFunctionName = 25,

        // example: root(a)    (expecting 2 parameters)
        IncorrectNumParameter = 26,

        // exmaple: "x_", "x_@", "x__1"
        InvalidVariableNameFormat = 32,

        // found } without matching {
        BracketMismatch = 34,

        // found { without matching }
        UnmatchedBracket = 35,

        // syntax error in MathML format. Used only if CasContext.ParsingOptions.FormatType is MathML or MathMLNoWrapper
        InvalidMathMLFormat = 40,

        // The input has an unknown MathML entity. Used only if CasContext.ParsingOptions.FormatType is MathML or MathMLNoWrapper
        UnknownMathMLEntity = 41,

        // The input has an unknown MathML element. Used only if CasContext.ParsingOptions.FormatType is MathML or MathMLNoWrapper
        UnknownMathMLElement = 42,

        // "i" and "I" cannot be used as variable names in real number field
        CannotUseIInReal = 48,

        // General error
        GeneralError = 52,

        // used in parsing numbers with arbitrary bases. example: base(2, 1020), base(16, 1AG)
        InvalidNumberDigit = 55,

        // a valid number base must be an integer >=2 and <=36
        InvalidNumberBase = 56,

        // some functions require a variable in certain argument position. e.g. 2nd argument of deriv, integral, limit, etc.
        // this error code is used if the argument at the position is not a variable
        InvalidVariableSpecification = 57,

        // all operands of logical operators must be logical. example: "true and 1"
        ExpectingLogicalOperands = 58,

        // all operands of a non-logical operator must not be logical. example: "sin(true)"
        ExpectingScalarOperands = 59,

        // a list can contain logicals or scalars, but not both.
        CannotMixLogicalScalarInList = 60,

        // in definite integral, seriesSum and seriesProduct, the index variable is used in the lower/upper limits.
        // example: integral(sin(x), x, 0, x)
        CannotUseIndexVarInOpLimits = 61,

        // in limit, the index variable is used in the limit point
        // example: limit(sin(x), x, x-1)
        CannotUseIndexVarInLimPoint = 62,

        // ComplexInfinity cannot be used in real number field
        CannotUseComplexInfinityInReal = 72,

        // complex numbers are not allowed in inequality solving
        CannotUseIInInequalitySolving = 123,

        // Indicate a bug in the MathRichEdit serializer
        RichEditSerializationError = 201,

        // can't initialize math zone in richedit, meaning it's the wrong version richedit dll, need reinstall
        RichEditInitialization = 202,

        // indicate bug in either richedit or richedit wrapper
        RichEditInlineObjectStructure = 203,

        // in a structure like integral, sum, product, one of the boxes is not filled
        RichEditMissingArgument = 204,

        // errors in richedit wrapper that are not specifically handled for
        RichEditGeneralError = 210,
    }

    public sealed class Equation : INotifyPropertyChanged
    {
        private const string s_mathPrefix = "mml:";

        private string _expression = string.Empty;
        private bool _isLineEnabled = true;
        private bool _isValidated;
        private bool _hasGraphError;
        private bool _isInequality;
        private bool _isSelected;
        private EquationLineStyle _equationStyle = EquationLineStyle.Solid;
        private ErrorType _graphErrorType;
        private int _graphErrorCode;
        private Color _lineColor = Colors.Transparent;
        private object _graphedEquation;

        public event PropertyChangedEventHandler PropertyChanged;

        public static string ExpressionPropertyName => nameof(Expression);
        public static string IsLineEnabledPropertyName => nameof(IsLineEnabled);
        public static string IsValidatedPropertyName => nameof(IsValidated);
        public static string HasGraphErrorPropertyName => nameof(HasGraphError);
        public static string IsInequalityPropertyName => nameof(IsInequality);
        public static string IsSelectedPropertyName => nameof(IsSelected);
        public static string EquationStylePropertyName => nameof(EquationStyle);
        public static string GraphErrorTypePropertyName => nameof(GraphErrorType);
        public static string GraphErrorCodePropertyName => nameof(GraphErrorCode);
        public static string LineColorPropertyName => nameof(LineColor);

        public string Expression
        {
            get => _expression;
            set
            {
                if (_expression != value)
                {
                    _expression = value;
                    RaisePropertyChanged();
                }
            }
        }

        public bool IsLineEnabled
        {
            get => _isLineEnabled;
            set
            {
                if (_isLineEnabled != value)
                {
                    _isLineEnabled = value;
                    RaisePropertyChanged();
                }
            }
        }

        public bool IsValidated
        {
            get => _isValidated;
            set
            {
                if (_isValidated != value)
                {
                    _isValidated = value;
                    RaisePropertyChanged();
                }
            }
        }

        public bool HasGraphError
        {
            get => _hasGraphError;
            set
            {
                if (_hasGraphError != value)
                {
                    _hasGraphError = value;
                    RaisePropertyChanged();
                }
            }
        }

        public bool IsInequality
        {
            get => _isInequality;
            set
            {
                if (_isInequality != value)
                {
                    _isInequality = value;
                    RaisePropertyChanged();
                }
            }
        }

        public bool IsSelected
        {
            get => _isSelected;
            set
            {
                if (_isSelected != value)
                {
                    _isSelected = value;
                    RaisePropertyChanged();
                }
            }
        }

        public EquationLineStyle EquationStyle
        {
            get => _equationStyle;
            set
            {
                if (_equationStyle != value)
                {
                    _equationStyle = value;
                    RaisePropertyChanged();
                }
            }
        }

        public ErrorType GraphErrorType
        {
            get => _graphErrorType;
            set
            {
                if (_graphErrorType != value)
                {
                    _graphErrorType = value;
                    RaisePropertyChanged();
                }
            }
        }

        public int GraphErrorCode
        {
            get => _graphErrorCode;
            set
            {
                if (_graphErrorCode != value)
                {
                    _graphErrorCode = value;
                    RaisePropertyChanged();
                }
            }
        }

        public Color LineColor
        {
            get => _lineColor;
            set
            {
                if (_lineColor != value)
                {
                    _lineColor = value;
                    RaisePropertyChanged();
                }
            }
        }

        public object GraphedEquation
        {
            get => _graphedEquation;
            set => _graphedEquation = value;
        }

        public Equation()
        {
        }

        public string GetRequest()
        {
            if (string.IsNullOrEmpty(Expression))
            {
                return null;
            }

            string expr = Expression;
            IsInequality = false;

            // Check for unicode characters of less than, less than or equal to, greater than and greater than or equal to.
            if (expr.Contains("&#x3E;<") || expr.Contains("&#x3C;<") || expr.Contains("&#x2265;<")
                || expr.Contains("&#x2264;<") || expr.Contains(((char)8805).ToString()) || expr.Contains(((char)8804).ToString())
                || expr.Contains("&lt;<") || expr.Contains("&gt;<"))
            {
                IsInequality = true;
                EquationStyle = EquationLineStyle.Dash;
                return $"<mrow><mi>plotIneq2D</mi><mfenced separators=\"\">{GetExpression()}</mfenced></mrow>";
            }
            else if (expr.Contains(">=<"))
            {
                return $"<mrow><mi>plotEq2d</mi><mfenced separators=\"\">{GetExpression()}</mfenced></mrow>";
            }
            // If the expression contains both x and y but no equal or inequality sign, then that cannot be graphed.
            else if (expr.Contains(">x<") && expr.Contains(">y<"))
            {
                return null;
            }
            // Else default to plot2d
            else
            {
                return $"<mrow><mi>plot2d</mi><mfenced separators=\"\">{GetExpression()}</mfenced></mrow>";
            }
        }

        private string GetExpression()
        {
            if (string.IsNullOrEmpty(Expression))
            {
                return string.Empty;
            }

            string mathML = Expression;
            return mathML.Replace(s_mathPrefix, string.Empty);
        }

        public bool IsGraphableEquation()
        {
            return !string.IsNullOrEmpty(Expression) && IsLineEnabled && !HasGraphError;
        }

        private void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
