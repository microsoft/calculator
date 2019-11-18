// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ActiveTracingPointRenderer.h"
#include "DirectXHelper.h"

using namespace D2D1;
using namespace GraphControl::DX;
using namespace std;
using namespace Windows::Foundation;

namespace
{
    const ColorF c_DefaultPointColor = ColorF::Red;
    constexpr float c_ActiveTracingPointRadius = 2;
}

ActiveTracingPointRenderer::ActiveTracingPointRenderer(DeviceResources* deviceResources)
    : m_deviceResources{ deviceResources }
    , m_color{ c_DefaultPointColor }
    
{
    m_RoundedRect.rect.bottom = 0;
    m_RoundedRect.rect.left = 0;
    m_RoundedRect.rect.right = 10;
    m_RoundedRect.rect.top = 10;
    m_width = (int)(m_RoundedRect.rect.right - m_RoundedRect.rect.left);
    m_height = (int)(m_RoundedRect.rect.top - m_RoundedRect.rect.bottom);
    m_RoundedRect.radiusX = c_ActiveTracingPointRadius;
    m_RoundedRect.radiusY = c_ActiveTracingPointRadius;

    CreateDeviceDependentResources();
}

void ActiveTracingPointRenderer::CreateDeviceDependentResources()
{
    CreateBrush();
}

void ActiveTracingPointRenderer::ReleaseDeviceDependentResources()
{
    m_brush.Reset();
}

void ActiveTracingPointRenderer::Render(const Point& location)
{
    // We want to center this around the location
    if (ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext())
    {
        m_RoundedRect.rect.bottom = location.Y - m_height / 2;
        m_RoundedRect.rect.left = location.X - m_width / 2;
        m_RoundedRect.rect.top = m_RoundedRect.rect.bottom + m_height;
        m_RoundedRect.rect.right = m_RoundedRect.rect.left + m_width;

        context->BeginDraw();
        context->FillRoundedRectangle(m_RoundedRect, m_brush.Get());

        // Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
        // is lost. It will be handled during the next call to Present.
        HRESULT hr = context->EndDraw();
        if (hr != D2DERR_RECREATE_TARGET)
        {
            ThrowIfFailed(hr);
        }
    }
}

void ActiveTracingPointRenderer::SetColor(const ColorF& color)
{
    m_color = color;
    CreateBrush();
}

void ActiveTracingPointRenderer::CreateBrush()
{
    m_brush.Reset();
    ThrowIfFailed(
        m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(m_color, &m_brush)
    );
}
