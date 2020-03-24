// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp::ViewModel::Utils
{
    public enum class DeviceFamily
    {
        Unknown,
        Desktop,
        Mobile,
        Xbox,
        SurfaceHub,
        HoloLens,
        WindowsCore,
    };

    public ref class DeviceFamilyHelper sealed
    {
    private:
        DeviceFamilyHelper() {}
    public:
        static DeviceFamily GetDeviceFamily();
    private:
        static void InitDeviceFamily();
    private:
        static bool m_isInit;
        static DeviceFamily m_deviceFamily;
    };
}
