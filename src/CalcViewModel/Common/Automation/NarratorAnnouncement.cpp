// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NarratorAnnouncement.h"

using namespace CalculatorApp::Common::Automation;
using namespace Platform;
using namespace Windows::UI::Xaml::Automation::Peers;

namespace CalculatorApp::Common::Automation
{
    namespace CalculatorActivityIds
    {
        StringReference DisplayUpdated(L"DisplayUpdated");
        StringReference MaxDigitsReached(L"MaxDigitsReached");
        StringReference MemoryCleared(L"MemoryCleared");
        StringReference MemoryItemChanged(L"MemorySlotChanged");
        StringReference MemoryItemAdded(L"MemorySlotAdded");
        StringReference HistoryCleared(L"HistoryCleared");
        StringReference HistorySlotCleared(L"HistorySlotCleared");
        StringReference CategoryNameChanged(L"CategoryNameChanged");
        StringReference UpdateCurrencyRates(L"UpdateCurrencyRates");
        StringReference DisplayCopied(L"DisplayCopied");
        StringReference OpenParenthesisCountChanged(L"OpenParenthesisCountChanged");
        StringReference NoParenthesisAdded(L"NoParenthesisAdded");
        StringReference GraphModeChanged(L"GraphModeChanged");
        StringReference GraphViewChanged(L"GraphViewChanged");
        StringReference FunctionRemoved(L"FunctionRemoved");
        StringReference GraphViewBestFitChanged(L"GraphViewBestFitChanged");
        StringReference AlwaysOnTop(L"AlwaysOnTop");
        StringReference BitShiftRadioButtonContent(L"BitShiftRadioButtonContent");
    }
}

NarratorAnnouncement::NarratorAnnouncement(
    String ^ announcement,
    String ^ activityId,
    AutomationNotificationKind kind,
    AutomationNotificationProcessing processing)
    : m_announcement(announcement)
    , m_activityId(activityId)
    , m_kind(kind)
    , m_processing(processing)
{
}

String ^ NarratorAnnouncement::Announcement::get()
{
    return m_announcement;
}

String ^ NarratorAnnouncement::ActivityId::get()
{
    return m_activityId;
}

AutomationNotificationKind NarratorAnnouncement::Kind::get()
{
    return m_kind;
}

AutomationNotificationProcessing NarratorAnnouncement::Processing::get()
{
    return m_processing;
}

bool NarratorAnnouncement::IsValid(NarratorAnnouncement ^ announcement)
{
    return announcement != nullptr && announcement->Announcement != nullptr && !announcement->Announcement->IsEmpty();
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetDisplayUpdatedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement, CalculatorActivityIds::DisplayUpdated, AutomationNotificationKind::Other, AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetMaxDigitsReachedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement, CalculatorActivityIds::MaxDigitsReached, AutomationNotificationKind::Other, AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetMemoryClearedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement, CalculatorActivityIds::MemoryCleared, AutomationNotificationKind::ItemRemoved, AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetMemoryItemChangedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement, CalculatorActivityIds::MemoryItemChanged, AutomationNotificationKind::ActionCompleted, AutomationNotificationProcessing::MostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetMemoryItemAddedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement, CalculatorActivityIds::MemoryItemAdded, AutomationNotificationKind::ItemAdded, AutomationNotificationProcessing::MostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetHistoryClearedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement, CalculatorActivityIds::HistoryCleared, AutomationNotificationKind::ItemRemoved, AutomationNotificationProcessing::MostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetHistorySlotClearedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement,
        CalculatorActivityIds::HistorySlotCleared,
        AutomationNotificationKind::ItemRemoved,
        AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetCategoryNameChangedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement,
        CalculatorActivityIds::CategoryNameChanged,
        AutomationNotificationKind::ActionCompleted,
        AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetUpdateCurrencyRatesAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement,
        CalculatorActivityIds::UpdateCurrencyRates,
        AutomationNotificationKind::ActionCompleted,
        AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetDisplayCopiedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement, CalculatorActivityIds::DisplayCopied, AutomationNotificationKind::ActionCompleted, AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetOpenParenthesisCountChangedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement,
        CalculatorActivityIds::OpenParenthesisCountChanged,
        AutomationNotificationKind::ActionCompleted,
        AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetNoRightParenthesisAddedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement,
        CalculatorActivityIds::NoParenthesisAdded,
        AutomationNotificationKind::ActionCompleted,
        AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetGraphModeChangedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement,
        CalculatorActivityIds::GraphModeChanged,
        AutomationNotificationKind::ActionCompleted,
        AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetGraphViewChangedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement,
        CalculatorActivityIds::GraphViewChanged,
        AutomationNotificationKind::ActionCompleted,
        AutomationNotificationProcessing::CurrentThenMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetFunctionRemovedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement, CalculatorActivityIds::FunctionRemoved, AutomationNotificationKind::ItemRemoved, AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetGraphViewBestFitChangedAnnouncement(Platform::String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement,
        CalculatorActivityIds::GraphViewBestFitChanged,
        AutomationNotificationKind::ActionCompleted,
        AutomationNotificationProcessing::MostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetAlwaysOnTopChangedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement, CalculatorActivityIds::AlwaysOnTop, AutomationNotificationKind::ActionCompleted, AutomationNotificationProcessing::ImportantMostRecent);
}

NarratorAnnouncement ^ CalculatorAnnouncement::GetBitShiftRadioButtonCheckedAnnouncement(String ^ announcement)
{
    return ref new NarratorAnnouncement(
        announcement,
        CalculatorActivityIds::BitShiftRadioButtonContent,
        AutomationNotificationKind::ActionCompleted,
        AutomationNotificationProcessing::ImportantMostRecent);
}
