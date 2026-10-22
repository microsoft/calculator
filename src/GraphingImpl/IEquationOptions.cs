// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Graphing.Renderer;

namespace Graphing
{
    public interface IEquationOptions
    {
        Color GetGraphColor();
        void SetGraphColor(Color color);
        void ResetGraphColor();

        LineStyle GetLineStyle();
        void SetLineStyle(LineStyle value);
        void ResetLineStyle();

        float GetLineWidth();
        void SetLineWidth(float value);
        void ResetLineWidth();

        float GetSelectedEquationLineWidth();
        void SetSelectedEquationLineWidth(float value);
        void ResetSelectedEquationLineWidth();

        float GetPointRadius();
        void SetPointRadius(float value);
        void ResetPointRadius();

        float GetSelectedEquationPointRadius();
        void SetSelectedEquationPointRadius(float value);
        void ResetSelectedEquationPointRadius();
    }
}
