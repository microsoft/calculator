// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace Graphing.Renderer
{
    public interface IGraphRenderer
    {
        int SetGraphSize(uint width, uint height);
        int SetDpi(float dpiX, float dpiY);

        int DrawD2D1(object pDirect2dFactory, object pRenderTarget, out bool hasSomeMissingDataOut);

        int GetClosePointData(
            double inScreenPointX,
            double inScreenPointY,
            double precision,
            out int formulaIdOut,
            out float xScreenPointOut,
            out float yScreenPointOut,
            out double xValueOut,
            out double yValueOut,
            out double rhoValueOut,
            out double thetaValueOut,
            out double tValueOut);

        int ScaleRange(double centerX, double centerY, double scale);
        int ChangeRange(ChangeRangeAction action);
        int MoveRangeByRatio(double ratioX, double ratioY);
        int ResetRange();
        int GetDisplayRanges(out double xMin, out double xMax, out double yMin, out double yMax);
        int SetDisplayRanges(double xMin, double xMax, double yMin, double yMax);
        int PrepareGraph();

        int GetBitmap(out IBitmap bitmapOut, out bool hasSomeMissingDataOut);
    }
}
