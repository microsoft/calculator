// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.UI.Xaml.Automation.Peers;

namespace CalculatorApp.ViewModel.Common.Automation
{
    public sealed class NarratorAnnouncement
    {
        public string Announcement { get; }
        public string ActivityId { get; }
        public AutomationNotificationKind Kind { get; }
        public AutomationNotificationProcessing Processing { get; }

        internal NarratorAnnouncement(
            string announcement,
            string activityId,
            AutomationNotificationKind kind,
            AutomationNotificationProcessing processing)
        {
            Announcement = announcement;
            ActivityId = activityId;
            Kind = kind;
            Processing = processing;
        }

        public static bool IsValid(NarratorAnnouncement announcement)
        {
            return announcement != null && !string.IsNullOrEmpty(announcement.Announcement);
        }
    }

    internal static class CalculatorActivityIds
    {
        public const string DisplayUpdated = "DisplayUpdated";
        public const string MaxDigitsReached = "MaxDigitsReached";
        public const string MemoryCleared = "MemoryCleared";
        public const string MemoryItemChanged = "MemorySlotChanged";
        public const string MemoryItemAdded = "MemorySlotAdded";
        public const string HistoryCleared = "HistoryCleared";
        public const string HistorySlotCleared = "HistorySlotCleared";
        public const string CategoryNameChanged = "CategoryNameChanged";
        public const string UpdateCurrencyRates = "UpdateCurrencyRates";
        public const string DisplayCopied = "DisplayCopied";
        public const string OpenParenthesisCountChanged = "OpenParenthesisCountChanged";
        public const string NoParenthesisAdded = "NoParenthesisAdded";
        public const string GraphModeChanged = "GraphModeChanged";
        public const string GraphViewChanged = "GraphViewChanged";
        public const string FunctionRemoved = "FunctionRemoved";
        public const string GraphViewBestFitChanged = "GraphViewBestFitChanged";
        public const string AlwaysOnTop = "AlwaysOnTop";
        public const string BitShiftRadioButtonContent = "BitShiftRadioButtonContent";
        public const string SettingsPageOpened = "SettingsPageOpened";
    }

    /// <summary>
    /// Factory class containing static methods that return announcements for the Calculator app.
    /// </summary>
    public sealed class CalculatorAnnouncement
    {
        public static NarratorAnnouncement GetDisplayUpdatedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.DisplayUpdated,
                AutomationNotificationKind.Other, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetMaxDigitsReachedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.MaxDigitsReached,
                AutomationNotificationKind.Other, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetMemoryClearedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.MemoryCleared,
                AutomationNotificationKind.ItemRemoved, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetMemoryItemChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.MemoryItemChanged,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.MostRecent);
        }

        public static NarratorAnnouncement GetMemoryItemAddedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.MemoryItemAdded,
                AutomationNotificationKind.ItemAdded, AutomationNotificationProcessing.MostRecent);
        }

        public static NarratorAnnouncement GetHistoryClearedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.HistoryCleared,
                AutomationNotificationKind.ItemRemoved, AutomationNotificationProcessing.MostRecent);
        }

        public static NarratorAnnouncement GetHistorySlotClearedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.HistorySlotCleared,
                AutomationNotificationKind.ItemRemoved, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetCategoryNameChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.CategoryNameChanged,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetUpdateCurrencyRatesAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.UpdateCurrencyRates,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetDisplayCopiedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.DisplayCopied,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetOpenParenthesisCountChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.OpenParenthesisCountChanged,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetNoRightParenthesisAddedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.NoParenthesisAdded,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetGraphModeChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.GraphModeChanged,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetGraphViewChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.GraphViewChanged,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.CurrentThenMostRecent);
        }

        public static NarratorAnnouncement GetGraphViewBestFitChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.GraphViewBestFitChanged,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.MostRecent);
        }

        public static NarratorAnnouncement GetFunctionRemovedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.FunctionRemoved,
                AutomationNotificationKind.ItemRemoved, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetAlwaysOnTopChangedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.AlwaysOnTop,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetBitShiftRadioButtonCheckedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.BitShiftRadioButtonContent,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }

        public static NarratorAnnouncement GetSettingsPageOpenedAnnouncement(string announcement)
        {
            return new NarratorAnnouncement(
                announcement, CalculatorActivityIds.SettingsPageOpened,
                AutomationNotificationKind.ActionCompleted, AutomationNotificationProcessing.ImportantMostRecent);
        }
    }
}
