// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NotificationHost.h"

using namespace CalculatorApp::ViewModel::Common::Automation;
using namespace Windows::Foundation::Metadata;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml::Controls;

NotificationHost::NotificationHost()
    : m_host(nullptr)
{
}

bool NotificationHost::IsHostAvailable()
{
    return ApiInformation::IsMethodPresent(L"Windows.UI.Xaml.Automation.Peers.AutomationPeer", L"RaiseNotificationEvent");
}

INarratorAnnouncementHost ^ NotificationHost::MakeHost()
{
    return ref new NotificationHost();
}

void NotificationHost::Announce(NarratorAnnouncement ^ announcement)
{
    if (m_host == nullptr)
    {
        m_host = ref new TextBlock();
    }

    auto peer = FrameworkElementAutomationPeer::FromElement(m_host);
    if (peer != nullptr)
    {
        peer->RaiseNotificationEvent(
            GetWindowsNotificationKind(announcement->Kind),
            GetWindowsNotificationProcessing(announcement->Processing),
            announcement->Announcement,
            announcement->ActivityId);
    }
}

StandardPeers::AutomationNotificationKind NotificationHost::GetWindowsNotificationKind(CustomPeers::AutomationNotificationKind customKindType)
{
    switch (customKindType)
    {
    case CustomPeers::AutomationNotificationKind::ItemAdded:
        return StandardPeers::AutomationNotificationKind::ItemAdded;

    case CustomPeers::AutomationNotificationKind::ItemRemoved:
        return StandardPeers::AutomationNotificationKind::ItemRemoved;

    case CustomPeers::AutomationNotificationKind::ActionCompleted:
        return StandardPeers::AutomationNotificationKind::ActionCompleted;

    case CustomPeers::AutomationNotificationKind::ActionAborted:
        return StandardPeers::AutomationNotificationKind::ActionAborted;

    case CustomPeers::AutomationNotificationKind::Other:
        return StandardPeers::AutomationNotificationKind::Other;

    default:
        assert(false && L"Unexpected AutomationNotificationKind");
    }

    return StandardPeers::AutomationNotificationKind::Other;
}

StandardPeers::AutomationNotificationProcessing
NotificationHost::GetWindowsNotificationProcessing(CustomPeers::AutomationNotificationProcessing customProcessingType)
{
    switch (customProcessingType)
    {
    case CustomPeers::AutomationNotificationProcessing::ImportantAll:
        return StandardPeers::AutomationNotificationProcessing::ImportantAll;

    case CustomPeers::AutomationNotificationProcessing::ImportantMostRecent:
        return StandardPeers::AutomationNotificationProcessing::ImportantMostRecent;

    case CustomPeers::AutomationNotificationProcessing::All:
        return StandardPeers::AutomationNotificationProcessing::All;

    case CustomPeers::AutomationNotificationProcessing::MostRecent:
        return StandardPeers::AutomationNotificationProcessing::MostRecent;

    case CustomPeers::AutomationNotificationProcessing::CurrentThenMostRecent:
        return StandardPeers::AutomationNotificationProcessing::CurrentThenMostRecent;

    default:
        assert(false && L"Unexpected AutomationNotificationProcessing");
    }

    return StandardPeers::AutomationNotificationProcessing::ImportantMostRecent;
}
