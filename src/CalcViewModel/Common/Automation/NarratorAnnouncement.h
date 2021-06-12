// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp::Common::Automation
{
public
    ref class NarratorAnnouncement sealed
    {
    public:
        property Platform::String
            ^ Announcement { Platform::String ^ get(); }

            property Platform::String
            ^ ActivityId { Platform::String ^ get(); }

            property Windows::UI::Xaml::Automation::Peers::AutomationNotificationKind Kind
        {
            Windows::UI::Xaml::Automation::Peers::AutomationNotificationKind get();
        }

        property Windows::UI::Xaml::Automation::Peers::AutomationNotificationProcessing Processing
        {
            Windows::UI::Xaml::Automation::Peers::AutomationNotificationProcessing get();
        }

        static bool IsValid(NarratorAnnouncement ^ announcement);

    private:
        // Make CalculatorAnnouncement a friend class so it is the only
        // class that can access the private constructor.
        friend class CalculatorAnnouncement;

        NarratorAnnouncement(
            Platform::String ^ announcement,
            Platform::String ^ activityId,
            Windows::UI::Xaml::Automation::Peers::AutomationNotificationKind kind,
            Windows::UI::Xaml::Automation::Peers::AutomationNotificationProcessing processing);

        Platform::String ^ m_announcement;
        Platform::String ^ m_activityId;
        Windows::UI::Xaml::Automation::Peers::AutomationNotificationKind m_kind;
        Windows::UI::Xaml::Automation::Peers::AutomationNotificationProcessing m_processing;
    };

    // CalculatorAnnouncement is intended to contain only static methods
    // that return announcements made for the Calculator app.
    class CalculatorAnnouncement
    {
    public:
        static NarratorAnnouncement ^ GetDisplayUpdatedAnnouncement(Platform::String ^ announcement);
        static NarratorAnnouncement ^ GetMaxDigitsReachedAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetMemoryClearedAnnouncement(Platform::String ^ announcement);
        static NarratorAnnouncement ^ GetMemoryItemChangedAnnouncement(Platform::String ^ announcement);
        static NarratorAnnouncement ^ GetMemoryItemAddedAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetHistoryClearedAnnouncement(Platform::String ^ announcement);
        static NarratorAnnouncement ^ GetHistorySlotClearedAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetCategoryNameChangedAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetUpdateCurrencyRatesAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetDisplayCopiedAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetOpenParenthesisCountChangedAnnouncement(Platform::String ^ announcement);
        static NarratorAnnouncement ^ GetNoRightParenthesisAddedAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetGraphModeChangedAnnouncement(Platform::String ^ announcement);
        static NarratorAnnouncement ^ GetGraphViewChangedAnnouncement(Platform::String ^ announcement);
        static NarratorAnnouncement ^ GetGraphViewBestFitChangedAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetFunctionRemovedAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetAlwaysOnTopChangedAnnouncement(Platform::String ^ announcement);

        static NarratorAnnouncement ^ GetBitShiftRadioButtonCheckedAnnouncement(Platform::String ^ announcement);
    };
}
