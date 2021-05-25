// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "INarratorAnnouncementHost.h"

// Declaration of the NarratorAnnouncementHostFactory class.
// This class exists to hide the construction of a concrete INarratorAnnouncementHost.
// Depending on the version of the OS the app is running on, the factory will return
// an announcement host appropriate for that version.

namespace CalculatorApp::ViewModel::Common::Automation
{
    class NarratorAnnouncementHostFactory
    {
    public:
        static INarratorAnnouncementHost ^ MakeHost();

    private:
        NarratorAnnouncementHostFactory()
        {
        }

        static int Initialize();
        static void RegisterHosts();
        static INarratorAnnouncementHost ^ GetHostProducer();

    private:
        static int s_init;
        static INarratorAnnouncementHost ^ s_hostProducer;
        static std::vector<INarratorAnnouncementHost ^> s_hosts;
    };
}
