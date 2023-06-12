// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "LiveRegionHost.h"

using namespace CalculatorApp::ViewModel::Common::Automation;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml::Controls;

LiveRegionHost::LiveRegionHost()
    : m_host(nullptr)
{
}

bool LiveRegionHost::IsHostAvailable()
{
    // LiveRegion is always available.
    return true;
}

INarratorAnnouncementHost ^ LiveRegionHost::MakeHost()
{
    return ref new LiveRegionHost();
}

void LiveRegionHost::Announce(NarratorAnnouncement ^ announcement)
{
    if (m_host == nullptr)
    {
        m_host = ref new TextBlock();
        AutomationProperties::SetLiveSetting(m_host, AutomationLiveSetting::Assertive);
    }

    AutomationProperties::SetName(m_host, announcement->Announcement);
    AutomationPeer ^ peer = FrameworkElementAutomationPeer::FromElement(m_host);
    if (peer != nullptr)
    {
        peer->RaiseAutomationEvent(AutomationEvents::LiveRegionChanged);
    }
}
