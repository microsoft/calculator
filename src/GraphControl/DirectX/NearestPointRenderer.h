// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace GraphControl::DX
{
    class DeviceResources;

    class NearestPointRenderer
    {
    public:
        NearestPointRenderer(DeviceResources* deviceResources);

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Render(const Windows::Foundation::Point& location);

        void SetColor(const D2D1::ColorF& color);
        void SetRadius(float radius);

    private:
        void CreateBrush();

    private:
        DeviceResources* const m_deviceResources;

        D2D1::ColorF m_color;
        D2D1_ELLIPSE m_ellipse;

        // Resources related to rendering.
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;
    };
}
