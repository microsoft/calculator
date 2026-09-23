// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel.Common.Automation;
using Windows.UI.Xaml.Automation.Peers;

namespace Calculator.Tests
{
    [TestClass]
    public class NarratorAnnouncementTests
    {
        private const string TestAnnouncement = "TestAnnouncement";

        [TestMethod]
        public void TestGetDisplayUpdatedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetDisplayUpdatedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("DisplayUpdated", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.Other, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetMaxDigitsReachedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetMaxDigitsReachedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("MaxDigitsReached", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.Other, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetMemoryClearedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetMemoryClearedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("MemoryCleared", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemRemoved, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetMemoryItemChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetMemoryItemChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("MemorySlotChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.MostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetMemoryItemAddedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetMemoryItemAddedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("MemorySlotAdded", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemAdded, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.MostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetHistoryClearedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetHistoryClearedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("HistoryCleared", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemRemoved, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.MostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetHistorySlotClearedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetHistorySlotClearedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("HistorySlotCleared", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemRemoved, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetCategoryNameChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetCategoryNameChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("CategoryNameChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetUpdateCurrencyRatesAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetUpdateCurrencyRatesAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("UpdateCurrencyRates", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetDisplayCopiedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetDisplayCopiedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("DisplayCopied", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetOpenParenthesisCountChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetOpenParenthesisCountChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("OpenParenthesisCountChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetNoRightParenthesisAddedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetNoRightParenthesisAddedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("NoParenthesisAdded", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetGraphModeChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetGraphModeChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("GraphModeChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetGraphViewChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetGraphViewChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("GraphViewChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.CurrentThenMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetFunctionRemovedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetFunctionRemovedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("FunctionRemoved", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ItemRemoved, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetGraphViewBestFitChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetGraphViewBestFitChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("GraphViewBestFitChanged", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.MostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetAlwaysOnTopChangedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetAlwaysOnTopChangedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("AlwaysOnTop", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetBitShiftRadioButtonCheckedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetBitShiftRadioButtonCheckedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("BitShiftRadioButtonContent", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }

        [TestMethod]
        public void TestGetSettingsPageOpenedAnnouncement()
        {
            var announcement = CalculatorAnnouncement.GetSettingsPageOpenedAnnouncement(TestAnnouncement);
            Assert.AreEqual(TestAnnouncement, announcement.Announcement);
            Assert.AreEqual("SettingsPageOpened", announcement.ActivityId);
            Assert.AreEqual(AutomationNotificationKind.ActionCompleted, announcement.Kind);
            Assert.AreEqual(AutomationNotificationProcessing.ImportantMostRecent, announcement.Processing);
        }
    }
}
