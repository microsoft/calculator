// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.ObjectModel;
using System.ComponentModel;
using CalculatorApp.ViewModel.Common;
using CommunityToolkit.Mvvm.ComponentModel;
using Windows.UI;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class GridDisplayItems : ObservableObject
    {
        [ObservableProperty]
        private string _expression;

        [ObservableProperty]
        private string _direction;

        public GridDisplayItems()
        {
            _expression = string.Empty;
            _direction = string.Empty;
        }
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class KeyGraphFeaturesItem : ObservableObject
    {
        [ObservableProperty]
        private string _title;

        [ObservableProperty]
        private ObservableCollection<string> _displayItems;

        [ObservableProperty]
        private ObservableCollection<GridDisplayItems> _gridItems;

        [ObservableProperty]
        private bool _isText;

        public KeyGraphFeaturesItem()
        {
            _title = string.Empty;
            _displayItems = new ObservableCollection<string>();
            _gridItems = new ObservableCollection<GridDisplayItems>();
            _isText = false;
        }
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class EquationViewModel : ObservableObject
    {
        private readonly GraphControl.Equation _graphEquation;

        [ObservableProperty]
        private int _functionLabelIndex;

        [ObservableProperty]
        private bool _isLastItemInList;

        private int _lineColorIndex;

        [ObservableProperty]
        private string _expression;

        [ObservableProperty]
        private Color _lineColor;

        [ObservableProperty]
        private bool _isLineEnabled;

        private string _analysisErrorString;
        private bool _analysisErrorVisible;
        private ObservableCollection<KeyGraphFeaturesItem> _keyGraphFeaturesItems;

        public EquationViewModel(GraphControl.Equation graphEquation, int functionLabelIndex, Color color, int colorIndex)
        {
            _graphEquation = graphEquation;
            _functionLabelIndex = functionLabelIndex;
            _lineColor = color;
            _lineColorIndex = colorIndex;
            _isLineEnabled = true;
            _expression = string.Empty;
            _analysisErrorString = string.Empty;
            _keyGraphFeaturesItems = new ObservableCollection<KeyGraphFeaturesItem>();
            _graphEquation.LineColor = color;
            _graphEquation.IsLineEnabled = true;
        }

        public GraphControl.Equation GraphEquation => _graphEquation;

        partial void OnExpressionChanged(string value)
        {
            _graphEquation.Expression = value;
        }

        partial void OnLineColorChanged(Color value)
        {
            _graphEquation.LineColor = value;
        }

        partial void OnIsLineEnabledChanged(bool value)
        {
            _graphEquation.IsLineEnabled = value;
        }

        public int LineColorIndex
        {
            get => _lineColorIndex;
            set => _lineColorIndex = value;
        }

        public string AnalysisErrorString
        {
            get => _analysisErrorString;
            private set => SetProperty(ref _analysisErrorString, value);
        }

        public bool AnalysisErrorVisible
        {
            get => _analysisErrorVisible;
            private set => SetProperty(ref _analysisErrorVisible, value);
        }

        public ObservableCollection<KeyGraphFeaturesItem> KeyGraphFeaturesItems
        {
            get => _keyGraphFeaturesItems;
            private set => SetProperty(ref _keyGraphFeaturesItems, value);
        }

        public void PopulateKeyGraphFeatures(GraphControl.KeyGraphFeaturesInfo info)
        {
            if (info == null)
            {
                throw new System.ArgumentNullException(nameof(info));
            }

            KeyGraphFeaturesItems.Clear();
            if (info.AnalysisError != (int)AnalysisErrorType.NoError)
            {
                AnalysisErrorVisible = true;
                switch ((AnalysisErrorType)info.AnalysisError)
                {
                    case AnalysisErrorType.AnalysisCouldNotBePerformed:
                        AnalysisErrorString = Resource("KGFAnalysisCouldNotBePerformed");
                        break;
                    case AnalysisErrorType.AnalysisNotSupported:
                        AnalysisErrorString = Resource("KGFAnalysisNotSupported");
                        break;
                    case AnalysisErrorType.VariableIsNotX:
                        AnalysisErrorString = Resource("KGFVariableIsNotX");
                        break;
                    default:
                        AnalysisErrorString = Resource("KGFAnalysisCouldNotBePerformed");
                        break;
                }
                return;
            }

            AddKeyGraphFeature(Resource("Domain"), info.Domain, Resource("KGFDomainNone"));
            AddKeyGraphFeature(Resource("Range"), info.Range, Resource("KGFRangeNone"));
            AddKeyGraphFeature(Resource("XIntercept"), info.XIntercept, Resource("KGFXInterceptNone"));
            AddKeyGraphFeature(Resource("YIntercept"), info.YIntercept, Resource("KGFYInterceptNone"));
            AddKeyGraphFeature(Resource("Minima"), info.Minima, Resource("KGFMinimaNone"));
            AddKeyGraphFeature(Resource("Maxima"), info.Maxima, Resource("KGFMaximaNone"));
            AddKeyGraphFeature(
                Resource("InflectionPoints"), info.InflectionPoints, Resource("KGFInflectionPointsNone"));
            AddKeyGraphFeature(
                Resource("VerticalAsymptotes"), info.VerticalAsymptotes, Resource("KGFVerticalAsymptotesNone"));
            AddKeyGraphFeature(
                Resource("HorizontalAsymptotes"), info.HorizontalAsymptotes, Resource("KGFHorizontalAsymptotesNone"));
            AddKeyGraphFeature(
                Resource("ObliqueAsymptotes"), info.ObliqueAsymptotes, Resource("KGFObliqueAsymptotesNone"));
            AddParityKeyGraphFeature(info);
            AddPeriodicityKeyGraphFeature(info);
            AddMonotonicityKeyGraphFeature(info);
            AddTooComplexKeyGraphFeature(info);

            AnalysisErrorString = string.Empty;
            AnalysisErrorVisible = false;
        }

        public static string EquationErrorText(GraphControl.ErrorType errorType, int errorCode)
        {
            if (errorType == GraphControl.ErrorType.Evaluation)
            {
                switch ((GraphControl.EvaluationErrorCode)errorCode)
                {
                    case GraphControl.EvaluationErrorCode.Overflow:
                        return Resource("Overflow");
                    case GraphControl.EvaluationErrorCode.RequireRadiansMode:
                        return Resource("RequireRadiansMode");
                    case GraphControl.EvaluationErrorCode.TooComplexToSolve:
                    case GraphControl.EvaluationErrorCode.EquationTooComplexToSolve:
                    case GraphControl.EvaluationErrorCode.EquationTooComplexToSolveSymbolic:
                    case GraphControl.EvaluationErrorCode.EquationTooComplexToPlot:
                    case GraphControl.EvaluationErrorCode.InequalityTooComplexToSolve:
                    case GraphControl.EvaluationErrorCode.GE_TooComplexToSolve:
                        return Resource("TooComplexToSolve");
                    case GraphControl.EvaluationErrorCode.RequireDegreesMode:
                        return Resource("RequireDegreesMode");
                    case GraphControl.EvaluationErrorCode.FactorialInvalidArgument:
                    case GraphControl.EvaluationErrorCode.Factorial2InvalidArgument:
                        return Resource("FactorialInvalidArgument");
                    case GraphControl.EvaluationErrorCode.FactorialCannotPerformOnLargeNumber:
                        return Resource("FactorialCannotPerformOnLargeNumber");
                    case GraphControl.EvaluationErrorCode.ModuloCannotPerformOnFloat:
                        return Resource("ModuloCannotPerformOnFloat");
                    case GraphControl.EvaluationErrorCode.EquationHasNoSolution:
                    case GraphControl.EvaluationErrorCode.InequalityHasNoSolution:
                        return Resource("EquationHasNoSolution");
                    case GraphControl.EvaluationErrorCode.DivideByZero:
                        return Resource("DivideByZero");
                    case GraphControl.EvaluationErrorCode.MutuallyExclusiveConditions:
                        return Resource("MutuallyExclusiveConditions");
                    case GraphControl.EvaluationErrorCode.OutOfDomain:
                        return Resource("OutOfDomain");
                    case GraphControl.EvaluationErrorCode.GE_NotSupported:
                        return Resource("GE_NotSupported");
                    default:
                        return Resource("GeneralError");
                }
            }

            if (errorType == GraphControl.ErrorType.Syntax)
            {
                switch ((GraphControl.SyntaxErrorCode)errorCode)
                {
                    case GraphControl.SyntaxErrorCode.ParenthesisMismatch:
                        return Resource("ParenthesisMismatch");
                    case GraphControl.SyntaxErrorCode.UnmatchedParenthesis:
                        return Resource("UnmatchedParenthesis");
                    case GraphControl.SyntaxErrorCode.TooManyDecimalPoints:
                        return Resource("TooManyDecimalPoints");
                    case GraphControl.SyntaxErrorCode.DecimalPointWithoutDigits:
                        return Resource("DecimalPointWithoutDigits");
                    case GraphControl.SyntaxErrorCode.UnexpectedEndOfExpression:
                        return Resource("UnexpectedEndOfExpression");
                    case GraphControl.SyntaxErrorCode.UnexpectedToken:
                        return Resource("UnexpectedToken");
                    case GraphControl.SyntaxErrorCode.InvalidToken:
                        return Resource("InvalidToken");
                    case GraphControl.SyntaxErrorCode.TooManyEquals:
                        return Resource("TooManyEquals");
                    case GraphControl.SyntaxErrorCode.EqualWithoutGraphVariable:
                        return Resource("EqualWithoutGraphVariable");
                    case GraphControl.SyntaxErrorCode.InvalidEquationSyntax:
                    case GraphControl.SyntaxErrorCode.InvalidEquationFormat:
                        return Resource("InvalidEquationSyntax");
                    case GraphControl.SyntaxErrorCode.EmptyExpression:
                        return Resource("EmptyExpression");
                    case GraphControl.SyntaxErrorCode.EqualWithoutEquation:
                        return Resource("EqualWithoutEquation");
                    case GraphControl.SyntaxErrorCode.ExpectParenthesisAfterFunctionName:
                        return Resource("ExpectParenthesisAfterFunctionName");
                    case GraphControl.SyntaxErrorCode.IncorrectNumParameter:
                        return Resource("IncorrectNumParameter");
                    case GraphControl.SyntaxErrorCode.InvalidVariableNameFormat:
                        return Resource("InvalidVariableNameFormat");
                    case GraphControl.SyntaxErrorCode.BracketMismatch:
                        return Resource("BracketMismatch");
                    case GraphControl.SyntaxErrorCode.UnmatchedBracket:
                        return Resource("UnmatchedBracket");
                    case GraphControl.SyntaxErrorCode.CannotUseIInReal:
                        return Resource("CannotUseIInReal");
                    case GraphControl.SyntaxErrorCode.InvalidNumberDigit:
                        return Resource("InvalidNumberDigit");
                    case GraphControl.SyntaxErrorCode.InvalidNumberBase:
                        return Resource("InvalidNumberBase");
                    case GraphControl.SyntaxErrorCode.InvalidVariableSpecification:
                        return Resource("InvalidVariableSpecification");
                    case GraphControl.SyntaxErrorCode.ExpectingLogicalOperands:
                    case GraphControl.SyntaxErrorCode.ExpectingScalarOperands:
                        return Resource("ExpectingLogicalOperands");
                    case GraphControl.SyntaxErrorCode.CannotUseIndexVarInOpLimits:
                        return Resource("CannotUseIndexVarInOpLimits");
                    case GraphControl.SyntaxErrorCode.CannotUseIndexVarInLimPoint:
                        return Resource("Overflow");
                    case GraphControl.SyntaxErrorCode.CannotUseComplexInfinityInReal:
                        return Resource("CannotUseComplexInfinityInReal");
                    case GraphControl.SyntaxErrorCode.CannotUseIInInequalitySolving:
                        return Resource("CannotUseIInInequalitySolving");
                    default:
                        return Resource("GeneralError");
                }
            }

            return Resource("GeneralError");
        }

        private static string Resource(string key)
        {
            return AppResourceProvider.GetInstance().GetResourceString(key);
        }

        private void AddKeyGraphFeature(string title, string expression, string errorString)
        {
            var item = new KeyGraphFeaturesItem { Title = title };
            if (!string.IsNullOrEmpty(expression))
            {
                item.DisplayItems.Add(expression);
                item.IsText = false;
            }
            else
            {
                item.DisplayItems.Add(errorString);
                item.IsText = true;
            }
            KeyGraphFeaturesItems.Add(item);
        }

        private void AddKeyGraphFeature(
            string title,
            System.Collections.Generic.IEnumerable<string> expressions,
            string errorString)
        {
            var item = new KeyGraphFeaturesItem { Title = title };
            if (expressions != null)
            {
                foreach (var expression in expressions)
                {
                    item.DisplayItems.Add(expression);
                }
            }

            if (item.DisplayItems.Count == 0)
            {
                item.DisplayItems.Add(errorString);
                item.IsText = true;
            }
            KeyGraphFeaturesItems.Add(item);
        }

        private void AddParityKeyGraphFeature(GraphControl.KeyGraphFeaturesInfo info)
        {
            var item = new KeyGraphFeaturesItem
            {
                Title = Resource("Parity"),
                IsText = true
            };
            string key;
            switch (info.Parity)
            {
                case 1:
                    key = "KGFParityOdd";
                    break;
                case 2:
                    key = "KGFParityEven";
                    break;
                case 3:
                    key = "KGFParityNeither";
                    break;
                default:
                    key = "KGFParityUnknown";
                    break;
            }
            item.DisplayItems.Add(Resource(key));
            KeyGraphFeaturesItems.Add(item);
        }

        private void AddPeriodicityKeyGraphFeature(GraphControl.KeyGraphFeaturesInfo info)
        {
            if (info.PeriodicityDirection == 0)
            {
                return;
            }

            var item = new KeyGraphFeaturesItem { Title = Resource("Periodicity") };
            switch (info.PeriodicityDirection)
            {
                case 1:
                    if (string.IsNullOrEmpty(info.PeriodicityExpression))
                    {
                        item.DisplayItems.Add(Resource("KGFPeriodicityUnknown"));
                        item.IsText = true;
                    }
                    else
                    {
                        item.DisplayItems.Add(info.PeriodicityExpression);
                    }
                    break;
                case 2:
                    item.DisplayItems.Add(Resource("KGFPeriodicityNotPeriodic"));
                    break;
                default:
                    item.DisplayItems.Add(Resource("KGFPeriodicityError"));
                    item.IsText = true;
                    break;
            }
            KeyGraphFeaturesItems.Add(item);
        }

        private void AddMonotonicityKeyGraphFeature(GraphControl.KeyGraphFeaturesInfo info)
        {
            var item = new KeyGraphFeaturesItem { Title = Resource("Monotonicity") };
            if (info.Monotonicity != null)
            {
                foreach (var pair in info.Monotonicity)
                {
                    string key;
                    switch (pair.Value)
                    {
                        case "1":
                            key = "KGFMonotonicityIncreasing";
                            break;
                        case "2":
                            key = "KGFMonotonicityDecreasing";
                            break;
                        case "3":
                            key = "KGFMonotonicityConstant";
                            break;
                        case "0":
                            key = "KGFMonotonicityUnknown";
                            break;
                        default:
                            key = "KGFMonotonicityError";
                            break;
                    }
                    item.GridItems.Add(new GridDisplayItems
                    {
                        Expression = pair.Key,
                        Direction = Resource(key)
                    });
                }
            }

            if (item.GridItems.Count == 0)
            {
                item.DisplayItems.Add(Resource("KGFMonotonicityError"));
                item.IsText = true;
            }
            KeyGraphFeaturesItems.Add(item);
        }

        private void AddTooComplexKeyGraphFeature(GraphControl.KeyGraphFeaturesInfo info)
        {
            var flags = (KeyGraphFeaturesFlag)info.TooComplexFeatures;
            if (flags == 0)
            {
                return;
            }

            var names = new System.Collections.Generic.List<string>();
            AddFeatureName(flags, KeyGraphFeaturesFlag.Domain, "Domain", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.Range, "Range", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.Zeros, "XIntercept", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.YIntercept, "YIntercept", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.Parity, "Parity", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.Periodicity, "Periodicity", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.Minima, "Minima", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.Maxima, "Maxima", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.InflectionPoints, "InflectionPoints", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.VerticalAsymptotes, "VerticalAsymptotes", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.HorizontalAsymptotes, "HorizontalAsymptotes", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.ObliqueAsymptotes, "ObliqueAsymptotes", names);
            AddFeatureName(flags, KeyGraphFeaturesFlag.MonotoneIntervals, "Monotonicity", names);

            var item = new KeyGraphFeaturesItem { IsText = true };
            item.DisplayItems.Add(Resource("KGFTooComplexFeaturesError"));
            item.DisplayItems.Add(
                string.Join(LocalizationSettings.GetInstance().GetListSeparator() + " ", names));
            KeyGraphFeaturesItems.Add(item);
        }

        private static void AddFeatureName(
            KeyGraphFeaturesFlag flags,
            KeyGraphFeaturesFlag flag,
            string resourceKey,
            System.Collections.Generic.ICollection<string> names)
        {
            if ((flags & flag) == flag)
            {
                names.Add(Resource(resourceKey));
            }
        }

    }
}
