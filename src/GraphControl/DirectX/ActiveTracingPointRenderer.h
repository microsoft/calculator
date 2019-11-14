// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace GraphControl::DX
{
    class DeviceResources;

    class ActiveTracingPointRenderer
    {
    public:
        ActiveTracingPointRenderer(DeviceResources* deviceResources);

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Render(const Windows::Foundation::Point& location);
        
        void SetColor(const D2D1::ColorF& color);
        
    private:
        void CreateBrush();
        
    private:
        DeviceResources* const m_deviceResources;
        
        D2D1::ColorF m_color;
        D2D1_ROUNDED_RECT m_RoundedRect;
        int m_width;
        int m_height;
        
        // Resources related to rendering.
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;
    };
}
