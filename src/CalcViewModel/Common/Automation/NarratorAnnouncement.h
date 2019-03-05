// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp::Common::Automation
{
    // These enum types are copied from the types available in
    // Windows::UI::Xaml::Automation::Peers in the RS3 SDK.
    // When this app switches to min version RS3, these custom
    // enums should be removed and the Windows types should be used
    // instead.
    // TODO - MSFT 12735088
    public enum class AutomationNotificationKind
    {
        ItemAdded = 0,
        ItemRemoved = 1,
        ActionCompleted = 2,
        ActionAborted = 3,
        Other = 4
    };

    public enum class AutomationNotificationProcessing
    {
        ImportantAll = 0,
        ImportantMostRecent = 1,
        All = 2,
        MostRecent = 3,
        CurrentThenMostRecent = 4
    };

    public ref class NarratorAnnouncement sealed
    {
    public:
        property Platform::String^ Announcement
        {
            Platform::String^ get();
        }

        property Platform::String^ ActivityId
        {
            Platform::String^ get();
        }

        property AutomationNotificationKind Kind
        {
            AutomationNotificationKind get();
        }

        property AutomationNotificationProcessing Processing
        {
            AutomationNotificationProcessing get();
        }

        static bool IsValid(NarratorAnnouncement^ announcement);

    private:
        // Make CalculatorAnnouncement a friend class so it is the only
        // class that can access the private constructor.
        friend class CalculatorAnnouncement;

        NarratorAnnouncement(
            Platform::String^ announcement,
            Platform::String^ activityId,
            AutomationNotificationKind kind,
            AutomationNotificationProcessing processing);

        Platform::String^ m_announcement;
        Platform::String^ m_activityId;
        AutomationNotificationKind m_kind;
        AutomationNotificationProcessing m_processing;
    };

    // CalculatorAnnouncement is intended to contain only static methods
    // that return announcements made for the Calculator app.
    class CalculatorAnnouncement
    {
    public:
        static NarratorAnnouncement^ GetDisplayUpdatedAnnouncement(Platform::String^ announcement);
        static NarratorAnnouncement^ GetMaxDigitsReachedAnnouncement(Platform::String^ announcement);

        static NarratorAnnouncement^ GetMemoryClearedAnnouncement(Platform::String^ announcement);
        static NarratorAnnouncement^ GetMemoryItemChangedAnnouncement(Platform::String^ announcement);
        static NarratorAnnouncement^ GetMemoryItemAddedAnnouncement(Platform::String^ announcement);

        static NarratorAnnouncement^ GetHistoryClearedAnnouncement(Platform::String^ announcement);

        static NarratorAnnouncement^ GetCategoryNameChangedAnnouncement(Platform::String^ announcement);

        static NarratorAnnouncement^ GetUpdateCurrencyRatesAnnouncement(Platform::String^ announcement);
        
        static NarratorAnnouncement^ GetDisplayCopiedAnnouncement(Platform::String^ announcement);
    };
}
