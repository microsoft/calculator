// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "NarratorAnnouncement.h"

// Declaration of the INarratorAnnouncementHost interface.
// This interface exists to hide the concrete announcement host
// being used. Depending on the version of the OS the app is running on,
// the app may need a host that uses LiveRegionChanged or RaiseNotification.

namespace CalculatorApp::ViewModel::Common::Automation
{
public
    interface class INarratorAnnouncementHost
    {
    public:
        // Is the host available on this OS.
        bool IsHostAvailable();

        // Make a new instance of a concrete host.
        INarratorAnnouncementHost ^ MakeHost();

        // Make an announcement using the concrete host's preferred method.
        void Announce(NarratorAnnouncement ^ announcement);
    };
}
