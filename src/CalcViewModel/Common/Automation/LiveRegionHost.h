// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "INarratorAnnouncementHost.h"

// Declaration of the LiveRegionHost class.
// This class announces NarratorAnnouncements using the LiveRegionChanged event.
// This event is unreliable and should be deprecated in favor of the new
// RaiseNotification API in RS3.

namespace CalculatorApp::Common::Automation
{
    // This class exists so that the app can run on RS2 and use LiveRegions
    // to host notifications on those builds.
    // When the app switches to min version RS3, this class can be removed
    // and the app will switch to using the Notification API.
    // TODO - MSFT 12735088
    public ref class LiveRegionHost sealed : public INarratorAnnouncementHost
    {
    public:
        LiveRegionHost();

        virtual bool IsHostAvailable();
        virtual INarratorAnnouncementHost^ MakeHost();

        virtual void Announce(NarratorAnnouncement^ announcement);

    private:
        Windows::UI::Xaml::UIElement^ m_host;
    };
}
