// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "INarratorAnnouncementHost.h"

// Declaration of the NotificationHost class.
// This class announces NarratorAnnouncements using the RaiseNotification API
// available in RS3.

namespace CalculatorApp::ViewModel::Common::Automation
{
public
    ref class NotificationHost sealed : public INarratorAnnouncementHost
    {
    public:
        NotificationHost();

        virtual bool IsHostAvailable();
        virtual INarratorAnnouncementHost ^ MakeHost();

        virtual void Announce(NarratorAnnouncement ^ announcement);

    private:
        static Windows::UI::Xaml::Automation::Peers::AutomationNotificationKind
        GetWindowsNotificationKind(CalculatorApp::ViewModel::Common::Automation::AutomationNotificationKind customKindType);

        static Windows::UI::Xaml::Automation::Peers::AutomationNotificationProcessing
        GetWindowsNotificationProcessing(CalculatorApp::ViewModel::Common::Automation::AutomationNotificationProcessing customProcessingType);

    private:
        Windows::UI::Xaml::UIElement ^ m_host;
    };
}
