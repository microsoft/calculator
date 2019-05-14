// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalculatorApp.Common.Automation
{
    // These enum types are copied from the types available in
    // Windows.UI.Xaml.Automation.Peers in the RS3 SDK.
    // When this app switches to min version RS3, these custom
    // enums should be removed and the Windows types should be used
    // instead.
    // TODO - MSFT 12735088
    public enum AutomationNotificationKind
    {
        ItemAdded = 0,
        ItemRemoved = 1,
        ActionCompleted = 2,
        ActionAborted = 3,
        Other = 4
    };

    public enum AutomationNotificationProcessing
    {
        ImportantAll = 0,
        ImportantMostRecent = 1,
        All = 2,
        MostRecent = 3,
        CurrentThenMostRecent = 4
    };

    public class NarratorAnnouncement
    {
        private string m_announcement;
        private string m_activityId;
        private AutomationNotificationKind m_kind;
        private AutomationNotificationProcessing m_processing;

        public NarratorAnnouncement(
            string announcement,
            string activityId,
            AutomationNotificationKind kind,
            AutomationNotificationProcessing processing)
        {
            m_announcement = announcement;
            m_activityId = activityId;
            m_kind = kind;
            m_processing = processing;
        }

        public string Announcement => m_announcement;

        public string ActivityId => m_activityId;

        public AutomationNotificationKind Kind => m_kind;

        public AutomationNotificationProcessing Processing => m_processing;

        public bool IsValid(NarratorAnnouncement announcement) 
            => announcement != null && announcement.Announcement != null && !string.IsNullOrEmpty(announcement.Announcement);
    };

    // CalculatorAnnouncement is intended to contain only static methods
    // that return announcements made for the Calculator app.
    public class CalculatorAnnouncement
    {
        const string DisplayUpdated = "DisplayUpdated";
        const string MaxDigitsReached = "MaxDigitsReached";
        const string MemoryCleared = "MemoryCleared";
        const string MemoryItemChanged = "MemorySlotChanged";
        const string MemoryItemAdded = "MemorySlotAdded";
        const string HistoryCleared = "HistoryCleared";
        const string CategoryNameChanged = "CategoryNameChanged";
        const string UpdateCurrencyRates = "UpdateCurrencyRates";
        const string DisplayCopied = "DisplayCopied";
        const string OpenParenthesisCountChanged = "OpenParenthesisCountChanged";
        const string NoParenthesisAdded = "NoParenthesisAdded";

        public static NarratorAnnouncement GetDisplayUpdatedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, DisplayUpdated, AutomationNotificationKind.Other, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetMaxDigitsReachedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, MaxDigitsReached, AutomationNotificationKind.Other, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetMemoryClearedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, MemoryCleared, AutomationNotificationKind.ItemRemoved, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetMemoryItemChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, MemoryItemChanged, AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.MostRecent);
        }

        public static NarratorAnnouncement GetMemoryItemAddedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, MemoryItemAdded, AutomationNotificationKind.ItemAdded, AutomationNotificationProcessing.MostRecent);
        }

        public static NarratorAnnouncement GetHistoryClearedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, HistoryCleared, AutomationNotificationKind.ItemRemoved, AutomationNotificationProcessing.MostRecent);
        }

        public static NarratorAnnouncement GetCategoryNameChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement,
                CategoryNameChanged,
                AutomationNotificationKind.ActionCompleted,
                AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetUpdateCurrencyRatesAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement,
                UpdateCurrencyRates,
                AutomationNotificationKind.ActionCompleted,
                AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetDisplayCopiedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, DisplayCopied, AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetOpenParenthesisCountChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement,
                OpenParenthesisCountChanged,
                AutomationNotificationKind.ActionCompleted,
                AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetNoRightParenthesisAddedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement,
                NoParenthesisAdded,
                AutomationNotificationKind.ActionCompleted,
                AutomationNotificationProcessing.ImportantMostRecent);
        }
    }
}
