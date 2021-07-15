// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Declaration of the NarratorNotifier class.

#pragma once
#include "NarratorAnnouncement.h"

namespace CalculatorApp::ViewModel::Common::Automation
{
public
    ref class NarratorNotifier sealed : public Windows::UI::Xaml::DependencyObject
    {
    public:
        NarratorNotifier();

        void Announce(NarratorAnnouncement ^ announcement);

        property NarratorAnnouncement^ Announcement
        {
            NarratorAnnouncement^ get() { return GetAnnouncement(this); }
            void set(NarratorAnnouncement^ value)
            {
                SetAnnouncement(this, value);
            }
        }

        static void RegisterDependencyProperties();

        static property Windows::UI::Xaml::DependencyProperty
            ^ AnnouncementProperty { Windows::UI::Xaml::DependencyProperty ^ get() { return s_announcementProperty; } }

            static NarratorAnnouncement
            ^ GetAnnouncement(
                Windows::UI::Xaml::DependencyObject ^ element) { return safe_cast<NarratorAnnouncement ^>(element->GetValue(s_announcementProperty)); }

            static void SetAnnouncement(Windows::UI::Xaml::DependencyObject ^ element, NarratorAnnouncement ^ value)
        {
            element->SetValue(s_announcementProperty, value);
        }

    private:
        static void OnAnnouncementChanged(
            _In_ Windows::UI::Xaml::DependencyObject ^ dependencyObject,
            _In_ Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ eventArgs);

        static Windows::UI::Xaml::DependencyProperty ^ s_announcementProperty;

    private:
        Windows::UI::Xaml::UIElement ^ m_announcementElement;
    };
}
