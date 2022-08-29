// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "GraphingInterfaces/IGraphRenderer.h"
#include "GraphingInterfaces/GraphingEnums.h"
#include "Mocks/Bitmap.h"
#include "Mocks/GraphingOptions.h"
#include <winbase.h>

namespace MockGraphingImpl
{
    class GraphRenderer : public Graphing::Renderer::IGraphRenderer
    {
    public:
        GraphRenderer()
            : m_xMin(-10)
            , m_xMax(10)
            , m_yMin(-10)
            , m_yMax(10)
        {
        }

        HRESULT SetGraphSize(unsigned int width, unsigned int height) override
        {
            return S_OK;
        }

        HRESULT SetDpi(float dpiX, float dpiY) override
        {
            return S_OK;
        }

        HRESULT DrawD2D1(ID2D1Factory* pDirect2dFactory, ID2D1RenderTarget* pRenderTarget, bool& hasSomeMissingDataOut) override
        {
            hasSomeMissingDataOut = false;
            return S_OK;
        }

        HRESULT GetClosePointData(
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
            double& tValueOut) override
        {
            formulaIdOut = 0;
            xScreenPointOut = 0;
            yScreenPointOut = 0;
            precision = 0;
            xValueOut = 0;
            yValueOut = 0;
            return S_OK;
        }

        HRESULT ScaleRange(double centerX, double centerY, double scale) override
        {
            m_xMin = scale * (m_xMin - centerX) + centerX;
            m_xMax = scale * (m_xMax - centerX) + centerX;
            m_yMin = scale * (m_yMin - centerY) + centerY;
            m_yMax = scale * (m_yMax - centerY) + centerY;
            return S_OK;
        }

        HRESULT ChangeRange(Graphing::Renderer::ChangeRangeAction action) override
        {
            return S_OK;
        }

        HRESULT MoveRangeByRatio(double ratioX, double ratioY) override
        {
            return S_OK;
        }

        HRESULT ResetRange() override
        {
            return S_OK;
        }

        HRESULT GetDisplayRanges(double& xMin, double& xMax, double& yMin, double& yMax) override
        {
            xMin = m_xMin;
            xMax = m_xMax;
            yMin = m_yMin;
            yMax = m_yMax;
            return S_OK;
        }

        HRESULT SetDisplayRanges(double xMin, double xMax, double yMin, double yMax) override
        {
            m_xMin = xMin;
            m_xMax = xMax;
            m_yMin = yMin;
            m_yMax = yMax;
            return S_OK;
        }

        HRESULT PrepareGraph() override
        {
            return S_OK;
        }

        HRESULT GetBitmap(std::shared_ptr<Graphing::IBitmap>& bitmapOut, bool& hasSomeMissingDataOut) override
        {
            bitmapOut = std::make_shared<Bitmap>();
            hasSomeMissingDataOut = false;
            return S_OK;
        }

    private:
        double m_xMin;
        double m_xMax;
        double m_yMin;
        double m_yMax;
    };
}
