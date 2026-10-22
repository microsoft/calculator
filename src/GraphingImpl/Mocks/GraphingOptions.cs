// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using Graphing;
using Graphing.Renderer;

namespace MockGraphingImpl
{
    public class GraphingOptions : IGraphingOptions
    {
        private bool _markZeros = true;
        private bool _markYIntercept;
        private bool _markMinima;
        private bool _markMaxima;
        private bool _markInflectionPoints;
        private bool _markVerticalAsymptotes;
        private bool _markHorizontalAsymptotes;
        private bool _markObliqueAsymptotes;
        private ulong _maxExecutionTime;
        private List<Color> _colors = new List<Color>();
        private Color _backColor;
        private bool _allowKeyGraphFeaturesForFunctionsWithParameters;
        private Color _zerosColor;
        private Color _extremaColor;
        private Color _inflectionPointsColor;
        private Color _asymptotesColor;
        private Color _axisColor;
        private Color _boxColor;
        private Color _gridColor;
        private Color _fontColor;
        private bool _showAxis = true;
        private bool _showGrid = true;
        private bool _showBox = true;
        private bool _forceProportional;
        private string _aliasX = "x";
        private string _aliasY = "y";
        private LineStyle _lineStyle = LineStyle.Solid;
        private (double Min, double Max) _xRange = (-10, 10);
        private (double Min, double Max) _yRange = (-10, 10);

        public void ResetMarkKeyGraphFeaturesData() { }

        public bool GetMarkZeros() => _markZeros;
        public void SetMarkZeros(bool value) => _markZeros = value;

        public bool GetMarkYIntercept() => _markYIntercept;
        public void SetMarkYIntercept(bool value) => _markYIntercept = value;

        public bool GetMarkMinima() => _markMinima;
        public void SetMarkMinima(bool value) => _markMinima = value;

        public bool GetMarkMaxima() => _markMaxima;
        public void SetMarkMaxima(bool value) => _markMaxima = value;

        public bool GetMarkInflectionPoints() => _markInflectionPoints;
        public void SetMarkInflectionPoints(bool value) => _markInflectionPoints = value;

        public bool GetMarkVerticalAsymptotes() => _markVerticalAsymptotes;
        public void SetMarkVerticalAsymptotes(bool value) => _markVerticalAsymptotes = value;

        public bool GetMarkHorizontalAsymptotes() => _markHorizontalAsymptotes;
        public void SetMarkHorizontalAsymptotes(bool value) => _markHorizontalAsymptotes = value;

        public bool GetMarkObliqueAsymptotes() => _markObliqueAsymptotes;
        public void SetMarkObliqueAsymptotes(bool value) => _markObliqueAsymptotes = value;

        public ulong GetMaxExecutionTime() => _maxExecutionTime;
        public void SetMaxExecutionTime(ulong value) => _maxExecutionTime = value;
        public void ResetMaxExecutionTime() => _maxExecutionTime = 0;

        public IReadOnlyList<Color> GetGraphColors() => _colors;
        public bool SetGraphColors(IReadOnlyList<Color> colors)
        {
            _colors = colors != null ? new List<Color>(colors) : new List<Color>();
            return true;
        }
        public void ResetGraphColors() => _colors.Clear();

        public Color GetBackColor() => _backColor;
        public void SetBackColor(Color value) => _backColor = value;
        public void ResetBackColor() => _backColor = default;

        public void SetAllowKeyGraphFeaturesForFunctionsWithParameters(bool kgf) => _allowKeyGraphFeaturesForFunctionsWithParameters = kgf;
        public bool GetAllowKeyGraphFeaturesForFunctionsWithParameters() => _allowKeyGraphFeaturesForFunctionsWithParameters;
        public void ResetAllowKeyGraphFeaturesForFunctionsWithParameters() => _allowKeyGraphFeaturesForFunctionsWithParameters = true;

        public Color GetZerosColor() => _zerosColor;
        public void SetZerosColor(Color value) => _zerosColor = value;
        public void ResetZerosColor() => _zerosColor = default;

        public Color GetExtremaColor() => _extremaColor;
        public void SetExtremaColor(Color value) => _extremaColor = value;
        public void ResetExtremaColor() => _extremaColor = default;

        public Color GetInflectionPointsColor() => _inflectionPointsColor;
        public void SetInflectionPointsColor(Color value) => _inflectionPointsColor = value;
        public void ResetInflectionPointsColor() => _inflectionPointsColor = default;

        public Color GetAsymptotesColor() => _asymptotesColor;
        public void SetAsymptotesColor(Color value) => _asymptotesColor = value;
        public void ResetAsymptotesColor() => _asymptotesColor = default;

        public Color GetAxisColor() => _axisColor;
        public void SetAxisColor(Color value) => _axisColor = value;
        public void ResetAxisColor() => _axisColor = default;

        public Color GetBoxColor() => _boxColor;
        public void SetBoxColor(Color value) => _boxColor = value;
        public void ResetBoxColor() => _boxColor = default;

        public Color GetGridColor() => _gridColor;
        public void SetGridColor(Color value) => _gridColor = value;
        public void ResetGridColor() => _gridColor = default;

        public Color GetFontColor() => _fontColor;
        public void SetFontColor(Color value) => _fontColor = value;
        public void ResetFontColor() => _fontColor = default;

        public bool GetShowAxis() => _showAxis;
        public void SetShowAxis(bool value) => _showAxis = value;
        public void ResetShowAxis() => _showAxis = true;

        public bool GetShowGrid() => _showGrid;
        public void SetShowGrid(bool value) => _showGrid = value;
        public void ResetShowGrid() => _showGrid = true;

        public bool GetShowBox() => _showBox;
        public void SetShowBox(bool value) => _showBox = value;
        public void ResetShowBox() => _showBox = true;

        public bool GetForceProportional() => _forceProportional;
        public void SetForceProportional(bool value) => _forceProportional = value;
        public void ResetForceProportional() => _forceProportional = false;

        public string GetAliasX() => _aliasX;
        public void SetAliasX(string value) => _aliasX = value;
        public void ResetAliasX() => _aliasX = string.Empty;

        public string GetAliasY() => _aliasY;
        public void SetAliasY(string value) => _aliasY = value;
        public void ResetAliasY() => _aliasY = string.Empty;

        public LineStyle GetLineStyle() => _lineStyle;
        public void SetLineStyle(LineStyle value) => _lineStyle = value;
        public void ResetLineStyle() => _lineStyle = LineStyle.Solid;

        public (double Min, double Max) GetDefaultXRange() => _xRange;
        public bool SetDefaultXRange((double Min, double Max) minmax)
        {
            _xRange = minmax;
            return true;
        }
        public void ResetDefaultXRange() => _xRange = (0, 0);

        public (double Min, double Max) GetDefaultYRange() => _yRange;
        public bool SetDefaultYRange((double Min, double Max) minmax)
        {
            _yRange = minmax;
            return true;
        }
        public void ResetDefaultYRange() => _yRange = (0, 0);
    }
}
