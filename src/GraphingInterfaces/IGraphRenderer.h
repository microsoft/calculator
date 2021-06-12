// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common.h"

struct ID2D1Factory;
struct ID2D1RenderTarget;

namespace Graphing
{
    struct IBitmap;
}

namespace Graphing::Renderer
{
    struct IGraphRenderer : public NonCopyable, public NonMoveable
    {
        virtual ~IGraphRenderer() = default;

        virtual HRESULT SetGraphSize(unsigned int width, unsigned int height) = 0;
        virtual HRESULT SetDpi(float dpiX, float dpiY) = 0;

        virtual HRESULT DrawD2D1(ID2D1Factory* pDirect2dFactory, ID2D1RenderTarget* pRenderTarget, bool& hasSomeMissingDataOut) = 0;
        virtual HRESULT GetClosePointData(
            double inScreenPointX,
            double inScreenPointY,
            double precision,
            int& formulaIdOut,
            float& xScreenPointOut,
            float& yScreenPointOut,
            double& xValueOut,
            double& yValueOut,
            double& rhoValueOut,
            double& thetaValueOut,
            double& tValueOut) = 0;

        virtual HRESULT ScaleRange(double centerX, double centerY, double scale) = 0;
        virtual HRESULT ChangeRange(ChangeRangeAction action) = 0;
        virtual HRESULT MoveRangeByRatio(double ratioX, double ratioY) = 0;
        virtual HRESULT ResetRange() = 0;
        virtual HRESULT GetDisplayRanges(double& xMin, double& xMax, double& yMin, double& yMax) = 0;
        virtual HRESULT SetDisplayRanges(double xMin, double xMax, double yMin, double yMax) = 0;
        virtual HRESULT PrepareGraph() = 0;

        virtual HRESULT GetBitmap(std::shared_ptr<Graphing::IBitmap>& bitmapOut, bool& hasSomeMissingDataOut) = 0;
    };
}
