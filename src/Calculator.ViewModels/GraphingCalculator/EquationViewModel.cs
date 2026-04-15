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
        }

        public GraphControl.Equation GraphEquation => _graphEquation;

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

    }
}
