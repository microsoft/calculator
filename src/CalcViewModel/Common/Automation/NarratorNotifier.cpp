// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Implementation of the NarratorNotifier class.

#include "pch.h"
#include "NarratorNotifier.h"

using namespace CalculatorApp::ViewModel::Common::Automation;
using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;

DependencyProperty ^ NarratorNotifier::s_announcementProperty;

NarratorNotifier::NarratorNotifier()
{
}

void NarratorNotifier::Announce(NarratorAnnouncement ^ announcement)
{
    if (NarratorAnnouncement::IsValid(announcement))
    {
        if (m_announcementElement == nullptr)
        {
            m_announcementElement = ref new Windows::UI::Xaml::Controls::TextBlock();
        }

        auto peer = FrameworkElementAutomationPeer::FromElement(m_announcementElement);
        if (peer != nullptr)
        {
            peer->RaiseNotificationEvent(announcement->Kind, announcement->Processing, announcement->Announcement, announcement->ActivityId);
        }
    }
}

void NarratorNotifier::RegisterDependencyProperties()
{
    s_announcementProperty = DependencyProperty::Register(
        L"Announcement",              // The name of the dependency property.
        NarratorAnnouncement::typeid, // The type of the dependency property.
        NarratorNotifier::typeid,     // The owner of the dependency property.
        ref new PropertyMetadata(
            nullptr, // Default value of the dependency property.
            ref new PropertyChangedCallback(OnAnnouncementChanged)));
}

void NarratorNotifier::OnAnnouncementChanged(_In_ DependencyObject ^ dependencyObject, _In_ DependencyPropertyChangedEventArgs ^ e)
{
    auto instance = safe_cast<NarratorNotifier ^>(dependencyObject);
    if (instance != nullptr)
    {
        instance->Announce(safe_cast<NarratorAnnouncement ^>(e->NewValue));
    }
}
