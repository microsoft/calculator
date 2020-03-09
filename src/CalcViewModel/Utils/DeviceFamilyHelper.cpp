// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DeviceFamilyHelper.h"

using namespace CalculatorApp::ViewModel::Utils;
using namespace Windows::System::Profile;

bool DeviceFamilyHelper::m_isInit{ false };
DeviceFamily DeviceFamilyHelper::m_deviceFamily{ DeviceFamily::Unknown };

DeviceFamily DeviceFamilyHelper::GetDeviceFamily()
{
    if (!m_isInit)
    {
        InitDeviceFamily();
        m_isInit = true;
    }
    return m_deviceFamily;
}

void DeviceFamilyHelper::InitDeviceFamily()
{
    auto deviceFamily = AnalyticsInfo::VersionInfo->DeviceFamily;
    if (deviceFamily == L"Windows.Desktop")
    {
        m_deviceFamily = DeviceFamily::Desktop;
    }
    else if (deviceFamily == L"Windows.Core")
    {
        m_deviceFamily = DeviceFamily::WindowsCore;
    }
    else if (deviceFamily == L"Windows.Xbox")
    {
        m_deviceFamily = DeviceFamily::Xbox;
    }
    else if (deviceFamily == L"Windows.Team")
    {
        m_deviceFamily = DeviceFamily::SurfaceHub;
    }
    else if (deviceFamily == L"Windows.Holographic")
    {
        m_deviceFamily = DeviceFamily::HoloLens;
    }
    else if (deviceFamily == L"Windows.Mobile")
    {
        m_deviceFamily = DeviceFamily::Mobile;
    }
    else
    {
        m_deviceFamily = DeviceFamily::Unknown;
    }
}
