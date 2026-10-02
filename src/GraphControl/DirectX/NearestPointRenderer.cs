// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.Foundation;
using Windows.UI;

namespace GraphControl.DX
{
    public class NearestPointRenderer
    {
        private readonly DeviceResources _deviceResources;
        private Color _color = Colors.Black;
        private float _radius = 3.0f;

        public NearestPointRenderer(DeviceResources deviceResources)
        {
            _deviceResources = deviceResources;
        }

        public void CreateDeviceDependentResources()
        {
        }

        public void ReleaseDeviceDependentResources()
        {
        }

        public void Render(Point location)
        {
        }

        public void SetColor(Color color)
        {
            _color = color;
        }

        public void SetRadius(float radius)
        {
            _radius = radius;
        }
    }
}
