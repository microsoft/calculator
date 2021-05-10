// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include <CppUnitTest.h>

using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CalculatorApp::Common::Automation;

namespace CalculatorUnitTests
{
    TEST_CLASS(NarratorAnnouncementUnitTests)
    {
    public:
        TEST_METHOD(TestGetDisplayUpdatedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetDisplayUpdatedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"DisplayUpdated");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::Other);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetMaxDigitsReachedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetMaxDigitsReachedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"MaxDigitsReached");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::Other);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetMemoryClearedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetMemoryClearedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"MemoryCleared");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ItemRemoved);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetMemoryItemChangedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetMemoryItemChangedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"MemorySlotChanged");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::MostRecent);
        }

        TEST_METHOD(TestGetMemoryItemAddedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetMemoryItemAddedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"MemorySlotAdded");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ItemAdded);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::MostRecent);
        }

        TEST_METHOD(TestGetHistoryClearedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetHistoryClearedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"HistoryCleared");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ItemRemoved);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::MostRecent);
        }

        TEST_METHOD(TestGetHistorySlotClearedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetHistorySlotClearedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"HistorySlotCleared");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ItemRemoved);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetCategoryNameChangedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetCategoryNameChangedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"CategoryNameChanged");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetUpdateCurrencyRatesAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetUpdateCurrencyRatesAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"UpdateCurrencyRates");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetDisplayCopiedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetDisplayCopiedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"DisplayCopied");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetOpenParenthesisCountChangedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetOpenParenthesisCountChangedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"OpenParenthesisCountChanged");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetNoRightParenthesisAddedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetNoRightParenthesisAddedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"NoParenthesisAdded");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetGraphModeChangedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetGraphModeChangedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"GraphModeChanged");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetGraphViewChangedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetGraphViewChangedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"GraphViewChanged");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::CurrentThenMostRecent);
        }

        TEST_METHOD(TestGetFunctionRemovedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetFunctionRemovedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"FunctionRemoved");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ItemRemoved);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetGraphViewBestFitChangedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetGraphViewBestFitChangedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"GraphViewBestFitChanged");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::MostRecent);
        }

        TEST_METHOD(TestGetAlwaysOnTopChangedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetAlwaysOnTopChangedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"AlwaysOnTop");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

        TEST_METHOD(TestGetBitShiftRadioButtonCheckedAnnouncement)
        {
            auto annoucement = CalculatorAnnouncement::GetBitShiftRadioButtonCheckedAnnouncement(m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->Announcement, m_testAnnouncement);
            VERIFY_ARE_EQUAL(annoucement->ActivityId, L"BitShiftRadioButtonContent");
            VERIFY_ARE_EQUAL(annoucement->Kind, AutomationNotificationKind::ActionCompleted);
            VERIFY_ARE_EQUAL(annoucement->Processing, AutomationNotificationProcessing::ImportantMostRecent);
        }

    private:
        static const Platform::StringReference m_testAnnouncement;
    };

    const Platform::StringReference NarratorAnnouncementUnitTests::m_testAnnouncement(L"TestAnnouncement");
}
