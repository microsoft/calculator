// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using Graphing.Renderer;

namespace Graphing
{
    public interface IGraphingOptions
    {
        void ResetMarkKeyGraphFeaturesData();

        bool GetMarkZeros();
        void SetMarkZeros(bool value);

        bool GetMarkYIntercept();
        void SetMarkYIntercept(bool value);

        bool GetMarkMinima();
        void SetMarkMinima(bool value);

        bool GetMarkMaxima();
        void SetMarkMaxima(bool value);

        bool GetMarkInflectionPoints();
        void SetMarkInflectionPoints(bool value);

        bool GetMarkVerticalAsymptotes();
        void SetMarkVerticalAsymptotes(bool value);

        bool GetMarkHorizontalAsymptotes();
        void SetMarkHorizontalAsymptotes(bool value);

        bool GetMarkObliqueAsymptotes();
        void SetMarkObliqueAsymptotes(bool value);

        ulong GetMaxExecutionTime();
        void SetMaxExecutionTime(ulong value);
        void ResetMaxExecutionTime();

        IReadOnlyList<Color> GetGraphColors();
        bool SetGraphColors(IReadOnlyList<Color> colors);
        void ResetGraphColors();

        Color GetBackColor();
        void SetBackColor(Color value);
        void ResetBackColor();

        void SetAllowKeyGraphFeaturesForFunctionsWithParameters(bool kgf);
        bool GetAllowKeyGraphFeaturesForFunctionsWithParameters();
        void ResetAllowKeyGraphFeaturesForFunctionsWithParameters();

        Color GetZerosColor();
        void SetZerosColor(Color value);
        void ResetZerosColor();

        Color GetExtremaColor();
        void SetExtremaColor(Color value);
        void ResetExtremaColor();

        Color GetInflectionPointsColor();
        void SetInflectionPointsColor(Color value);
        void ResetInflectionPointsColor();

        Color GetAsymptotesColor();
        void SetAsymptotesColor(Color value);
        void ResetAsymptotesColor();

        Color GetAxisColor();
        void SetAxisColor(Color value);
        void ResetAxisColor();

        Color GetBoxColor();
        void SetBoxColor(Color value);
        void ResetBoxColor();

        Color GetGridColor();
        void SetGridColor(Color value);
        void ResetGridColor();

        Color GetFontColor();
        void SetFontColor(Color value);
        void ResetFontColor();

        bool GetShowAxis();
        void SetShowAxis(bool value);
        void ResetShowAxis();

        bool GetShowGrid();
        void SetShowGrid(bool value);
        void ResetShowGrid();

        bool GetShowBox();
        void SetShowBox(bool value);
        void ResetShowBox();

        bool GetForceProportional();
        void SetForceProportional(bool value);
        void ResetForceProportional();

        string GetAliasX();
        void SetAliasX(string value);
        void ResetAliasX();

        string GetAliasY();
        void SetAliasY(string value);
        void ResetAliasY();

        LineStyle GetLineStyle();
        void SetLineStyle(LineStyle value);
        void ResetLineStyle();

        (double Min, double Max) GetDefaultXRange();
        bool SetDefaultXRange((double Min, double Max) minmax);
        void ResetDefaultXRange();

        (double Min, double Max) GetDefaultYRange();
        bool SetDefaultYRange((double Min, double Max) minmax);
        void ResetDefaultYRange();
    }
}
