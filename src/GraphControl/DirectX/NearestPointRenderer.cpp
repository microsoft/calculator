// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NearestPointRenderer.h"
#include "DirectXHelper.h"

using namespace D2D1;
using namespace GraphControl::DX;
using namespace std;
using namespace Windows::Foundation;

namespace
{
    const ColorF c_DefaultPointColor = ColorF::Black;
    constexpr float c_NearestPointRadius = 3;
}

NearestPointRenderer::NearestPointRenderer(DeviceResources* deviceResources)
    : m_deviceResources{ deviceResources }
    , m_color{ c_DefaultPointColor }
    , m_ellipse{ D2D1_POINT_2F{ 0, 0 }, c_NearestPointRadius, c_NearestPointRadius }
{
    CreateDeviceDependentResources();
}

void NearestPointRenderer::CreateDeviceDependentResources()
{
    CreateBrush();
}

void NearestPointRenderer::ReleaseDeviceDependentResources()
{
    m_brush.Reset();
}

void NearestPointRenderer::Render(const Point& location)
{
    if (ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext())
    {
        m_ellipse.point.x = location.X;
        m_ellipse.point.y = location.Y;

        context->BeginDraw();
        context->FillEllipse(m_ellipse, m_brush.Get());

        // Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
        // is lost. It will be handled during the next call to Present.
        HRESULT hr = context->EndDraw();
        if (hr != D2DERR_RECREATE_TARGET)
        {
            ThrowIfFailed(hr);
        }
    }
}

void NearestPointRenderer::SetRadius(float radius)
{
    m_ellipse.radiusX = radius;
    m_ellipse.radiusY = radius;
}

void NearestPointRenderer::SetColor(const ColorF& color)
{
    m_color = color;
    CreateBrush();
}

void NearestPointRenderer::CreateBrush()
{
    m_brush.Reset();
    ThrowIfFailed(m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_color, &m_brush));
}
