// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Graphing;
using Graphing.Renderer;

namespace MockGraphingImpl
{
    public class GraphRenderer : IGraphRenderer
    {
        private double _xMin = -10;
        private double _xMax = 10;
        private double _yMin = -10;
        private double _yMax = 10;

        public int SetGraphSize(uint width, uint height)
        {
            return 0; // S_OK
        }

        public int SetDpi(float dpiX, float dpiY)
        {
            return 0; // S_OK
        }

        public int DrawD2D1(object pDirect2dFactory, object pRenderTarget, out bool hasSomeMissingDataOut)
        {
            hasSomeMissingDataOut = false;
            return 0; // S_OK
        }

        public int GetClosePointData(
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
            out double tValueOut)
        {
            formulaIdOut = 0;
            xScreenPointOut = 0;
            yScreenPointOut = 0;
            xValueOut = 0;
            yValueOut = 0;
            rhoValueOut = 0;
            thetaValueOut = 0;
            tValueOut = 0;
            return 0; // S_OK
        }

        public int ScaleRange(double centerX, double centerY, double scale)
        {
            _xMin = scale * (_xMin - centerX) + centerX;
            _xMax = scale * (_xMax - centerX) + centerX;
            _yMin = scale * (_yMin - centerY) + centerY;
            _yMax = scale * (_yMax - centerY) + centerY;
            return 0; // S_OK
        }

        public int ChangeRange(ChangeRangeAction action)
        {
            return 0; // S_OK
        }

        public int MoveRangeByRatio(double ratioX, double ratioY)
        {
            return 0; // S_OK
        }

        public int ResetRange()
        {
            _xMin = -10;
            _xMax = 10;
            _yMin = -10;
            _yMax = 10;
            return 0; // S_OK
        }

        public int GetDisplayRanges(out double xMin, out double xMax, out double yMin, out double yMax)
        {
            xMin = _xMin;
            xMax = _xMax;
            yMin = _yMin;
            yMax = _yMax;
            return 0; // S_OK
        }

        public int SetDisplayRanges(double xMin, double xMax, double yMin, double yMax)
        {
            _xMin = xMin;
            _xMax = xMax;
            _yMin = yMin;
            _yMax = yMax;
            return 0; // S_OK
        }

        public int PrepareGraph()
        {
            return 0; // S_OK
        }

        public int GetBitmap(out IBitmap bitmapOut, out bool hasSomeMissingDataOut)
        {
            bitmapOut = new Bitmap();
            hasSomeMissingDataOut = false;
            return 0; // S_OK
        }
    }
}
