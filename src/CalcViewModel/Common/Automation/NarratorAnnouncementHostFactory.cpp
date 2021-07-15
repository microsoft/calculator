// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NarratorAnnouncementHostFactory.h"
#include "NotificationHost.h"
#include "LiveRegionHost.h"

using namespace CalculatorApp::ViewModel::Common::Automation;
using namespace std;

INarratorAnnouncementHost ^ NarratorAnnouncementHostFactory::s_hostProducer;
vector<INarratorAnnouncementHost ^> NarratorAnnouncementHostFactory::s_hosts;

// This static variable is used only to call the initialization function, to initialize the other static variables.
int NarratorAnnouncementHostFactory::s_init = NarratorAnnouncementHostFactory::Initialize();
int NarratorAnnouncementHostFactory::Initialize()
{
    RegisterHosts();
    NarratorAnnouncementHostFactory::s_hostProducer = GetHostProducer();

    return 0;
}

// For now, there are two type of announcement hosts.
// We'd prefer to use Notification if it's available and fall back to LiveRegion
// if not.  The availability of the host depends on the version of the OS the app is running on.
// When the app switches to min version RS3, the LiveRegionHost can be removed and we will always
// use NotificationHost.
// TODO - MSFT 12735088
void NarratorAnnouncementHostFactory::RegisterHosts()
{
    // The host that will be used is the first available host,
    // therefore, order of hosts is important here.
    NarratorAnnouncementHostFactory::s_hosts = { ref new NotificationHost(), ref new LiveRegionHost() };
}

INarratorAnnouncementHost ^ NarratorAnnouncementHostFactory::GetHostProducer()
{
    for (INarratorAnnouncementHost ^ host : NarratorAnnouncementHostFactory::s_hosts)
    {
        if (host->IsHostAvailable())
        {
            return host;
        }
    }

    assert(false && L"No suitable AnnouncementHost was found.");
    return nullptr;
}

INarratorAnnouncementHost ^ NarratorAnnouncementHostFactory::MakeHost()
{
    if (NarratorAnnouncementHostFactory::s_hostProducer == nullptr)
    {
        assert(false && L"No host producer has been assigned.");
        return nullptr;
    }

    return NarratorAnnouncementHostFactory::s_hostProducer->MakeHost();
}
