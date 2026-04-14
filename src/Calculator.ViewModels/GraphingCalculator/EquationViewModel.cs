// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.ObjectModel;
using System.ComponentModel;
using CalculatorApp.ViewModel.Common;
using Windows.UI;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class GridDisplayItems : INotifyPropertyChanged
    {
        private string _expression;
        private string _direction;

        public event PropertyChangedEventHandler PropertyChanged;

        public GridDisplayItems()
        {
            _expression = string.Empty;
            _direction = string.Empty;
        }

        public string Expression
        {
            get => _expression;
            set
            {
                if (_expression != value)
                {
                    _expression = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Expression)));
                }
            }
        }

        public string Direction
        {
            get => _direction;
            set
            {
                if (_direction != value)
                {
                    _direction = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Direction)));
                }
            }
        }
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class KeyGraphFeaturesItem : INotifyPropertyChanged
    {
        private string _title;
        private ObservableCollection<string> _displayItems;
        private ObservableCollection<GridDisplayItems> _gridItems;
        private bool _isText;

        public event PropertyChangedEventHandler PropertyChanged;

        public KeyGraphFeaturesItem()
        {
            _title = string.Empty;
            _displayItems = new ObservableCollection<string>();
            _gridItems = new ObservableCollection<GridDisplayItems>();
            _isText = false;
        }

        public string Title
        {
            get => _title;
            set { if (_title != value) { _title = value; RaisePropertyChanged(nameof(Title)); } }
        }

        public ObservableCollection<string> DisplayItems
        {
            get => _displayItems;
            set { if (_displayItems != value) { _displayItems = value; RaisePropertyChanged(nameof(DisplayItems)); } }
        }

        public ObservableCollection<GridDisplayItems> GridItems
        {
            get => _gridItems;
            set { if (_gridItems != value) { _gridItems = value; RaisePropertyChanged(nameof(GridItems)); } }
        }

        public bool IsText
        {
            get => _isText;
            set { if (_isText != value) { _isText = value; RaisePropertyChanged(nameof(IsText)); } }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class EquationViewModel : INotifyPropertyChanged
    {
        private readonly GraphControl.Equation _graphEquation;
        private int _functionLabelIndex;
        private bool _isLastItemInList;
        private int _lineColorIndex;
        private string _expression;
        private Color _lineColor;
        private bool _isLineEnabled;
        private string _analysisErrorString;
        private bool _analysisErrorVisible;
        private ObservableCollection<KeyGraphFeaturesItem> _keyGraphFeaturesItems;

        public event PropertyChangedEventHandler PropertyChanged;

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
        }

        public GraphControl.Equation GraphEquation => _graphEquation;

        public int FunctionLabelIndex
        {
            get => _functionLabelIndex;
            set { if (_functionLabelIndex != value) { _functionLabelIndex = value; RaisePropertyChanged(nameof(FunctionLabelIndex)); } }
        }

        public bool IsLastItemInList
        {
            get => _isLastItemInList;
            set { if (_isLastItemInList != value) { _isLastItemInList = value; RaisePropertyChanged(nameof(IsLastItemInList)); } }
        }

        public int LineColorIndex
        {
            get => _lineColorIndex;
            set => _lineColorIndex = value;
        }

        public string Expression
        {
            get => _expression;
            set
            {
                if (_expression != value)
                {
                    _expression = value;
                    RaisePropertyChanged(nameof(Expression));
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
                    RaisePropertyChanged(nameof(LineColor));
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
                    RaisePropertyChanged(nameof(IsLineEnabled));
                }
            }
        }

        public string AnalysisErrorString
        {
            get => _analysisErrorString;
            private set { if (_analysisErrorString != value) { _analysisErrorString = value; RaisePropertyChanged(nameof(AnalysisErrorString)); } }
        }

        public bool AnalysisErrorVisible
        {
            get => _analysisErrorVisible;
            private set { if (_analysisErrorVisible != value) { _analysisErrorVisible = value; RaisePropertyChanged(nameof(AnalysisErrorVisible)); } }
        }

        public ObservableCollection<KeyGraphFeaturesItem> KeyGraphFeaturesItems
        {
            get => _keyGraphFeaturesItems;
            private set { if (_keyGraphFeaturesItems != value) { _keyGraphFeaturesItems = value; RaisePropertyChanged(nameof(KeyGraphFeaturesItems)); } }
        }

        public void PopulateKeyGraphFeatures(object keyGraphFeaturesInfo)
        {
            // Populate key graph features from the graph control info object
            KeyGraphFeaturesItems.Clear();
            // Implementation would extract features from info and add items
        }

        public static string EquationErrorText(GraphControl.ErrorType errorType, int errorCode)
        {
            // Return localized error text based on error type and code
            return AppResourceProvider.GetInstance().GetResourceString($"GraphError_{(int)errorType}_{errorCode}");
        }

        private void AddKeyGraphFeature(string title, string expression, string errorString)
        {
            var item = new KeyGraphFeaturesItem { Title = title, IsText = true };
            if (!string.IsNullOrEmpty(errorString))
            {
                item.DisplayItems.Add(errorString);
            }
            else if (!string.IsNullOrEmpty(expression))
            {
                item.DisplayItems.Add(expression);
            }
            KeyGraphFeaturesItems.Add(item);
        }

        private void AddKeyGraphFeature(string title, System.Collections.Generic.IList<string> expressionVector, string errorString)
        {
            var item = new KeyGraphFeaturesItem { Title = title };
            if (!string.IsNullOrEmpty(errorString))
            {
                item.DisplayItems.Add(errorString);
                item.IsText = true;
            }
            else if (expressionVector != null)
            {
                foreach (var expr in expressionVector)
                {
                    item.DisplayItems.Add(expr);
                }
            }
            KeyGraphFeaturesItems.Add(item);
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
