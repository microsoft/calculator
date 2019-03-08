// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// UnitConverter.xaml.cpp
// Implementation of the UnitConverter class

#include "pch.h"
#include "UnitConverter.xaml.h"
#include "CalcViewModel/UnitConverterViewModel.h"
#include "Controls/CalculationResult.h"
#include "Controls/CalculatorButton.h"
#include "CalcViewModel/Common/CopyPasteManager.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "CalcViewModel/Common/LocalizationService.h"
#include "CalcViewModel/Common/LocalizationSettings.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace concurrency;
using namespace Windows::System;
using namespace Platform;
using namespace ViewModel;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Text;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

// Calculate number of 100-nanosecond intervals in 500 milliseconds.
// There are 10,000 intervals in 1 ms.
static const long long DURATION_500_MS = 10000 * 500;

UnitConverter::UnitConverter() :
    m_layoutDirection(::FlowDirection::LeftToRight),
    m_meteredConnectionOverride(false),
    m_isAnimationEnabled(false)
{
    InitializeComponent();

    //adding ESC key shortcut binding to clear button
    clearEntryButtonPos0->SetValue(Common::KeyboardShortcutManager::VirtualKeyProperty, Common::MyVirtualKey::Escape);

    m_layoutDirection = LocalizationService::GetInstance()->GetFlowDirection();
    if (m_layoutDirection == ::FlowDirection::RightToLeft)
    {
        Units1->HorizontalContentAlignment = ::HorizontalAlignment::Right;
        Units2->HorizontalContentAlignment = ::HorizontalAlignment::Right;
    }

    // Is currency symbol preference set to right side
    bool preferRight = LocalizationSettings::GetInstance().GetCurrencySymbolPrecedence() == 0;
    if (preferRight)
    {
        // Currency symbol should appear on the right. Reverse the order of children.
        Grid::SetColumn(Value1, 0);
        Grid::SetColumn(CurrencySymbol1Block, 1);
        
        Grid::SetColumn(Value2, 0);
        Grid::SetColumn(CurrencySymbol2Block, 1);
    }

    auto userSettings = ref new UISettings();
    m_isAnimationEnabled = userSettings->AnimationsEnabled;

    auto resLoader = AppResourceProvider::GetInstance();
    m_chargesMayApplyText = resLoader.GetResourceString(L"DataChargesMayApply");
    m_failedToRefreshText = resLoader.GetResourceString(L"FailedToRefresh");

    InitializeOfflineStatusTextBlock();

    m_resultsFlyout = static_cast<MenuFlyout^>(Resources->Lookup(L"CalculationResultContextMenu"));
    CopyMenuItem->Text = resLoader.GetResourceString(L"copyMenuItem");
    PasteMenuItem->Text = resLoader.GetResourceString(L"pasteMenuItem");
}

void UnitConverter::OnPropertyChanged(_In_ Object^ sender, _In_ PropertyChangedEventArgs^ e)
{
    String^ propertyName = e->PropertyName;
    if (propertyName->Equals(UnitConverterViewModelProperties::NetworkBehavior) ||
        propertyName->Equals(UnitConverterViewModelProperties::CurrencyDataLoadFailed))
    {
        OnNetworkBehaviorChanged();
    }
    else if (propertyName->Equals(UnitConverterViewModelProperties::CurrencyDataIsWeekOld))
    {
        SetCurrencyTimestampFontWeight();
    }
    else if (propertyName->Equals(UnitConverterViewModelProperties::IsCurrencyLoadingVisible))
    {
        OnIsDisplayVisibleChanged();
    }
}

void UnitConverter::OnNetworkBehaviorChanged()
{
    switch (Model->NetworkBehavior)
    {
    case NetworkAccessBehavior::Normal:
        OnNormalNetworkAccess();
        break;
    case NetworkAccessBehavior::OptIn:
        OnOptInNetworkAccess();
        break;
    case NetworkAccessBehavior::Offline:
        OnOfflineNetworkAccess();
        break;
    }
}

void UnitConverter::OnNormalNetworkAccess()
{
    CurrencyRefreshBlockControl->Visibility = ::Visibility::Visible;
    OfflineBlock->Visibility = ::Visibility::Collapsed;

    if (Model->CurrencyDataLoadFailed)
    {
        SetFailedToRefreshStatus();
    }
    else
    {
        SetNormalCurrencyStatus();
    }
}

void UnitConverter::OnOptInNetworkAccess()
{
    CurrencyRefreshBlockControl->Visibility = ::Visibility::Visible;
    OfflineBlock->Visibility = ::Visibility::Collapsed;

    if (m_meteredConnectionOverride && Model->CurrencyDataLoadFailed)
    {
        SetFailedToRefreshStatus();
    }
    else
    {
        SetChargesMayApplyStatus();
    }
}

void UnitConverter::OnOfflineNetworkAccess()
{
    CurrencyRefreshBlockControl->Visibility = ::Visibility::Collapsed;
    OfflineBlock->Visibility = ::Visibility::Visible;
}

void UnitConverter::SetNormalCurrencyStatus()
{
    CurrencySecondaryStatus->Text = L"";
}

void UnitConverter::SetChargesMayApplyStatus()
{
    VisualStateManager::GoToState(this, L"ChargesMayApplyCurrencyStatus", false);
    CurrencySecondaryStatus->Text = m_chargesMayApplyText;
}

void UnitConverter::SetFailedToRefreshStatus()
{
    VisualStateManager::GoToState(this, L"FailedCurrencyStatus", false);
    CurrencySecondaryStatus->Text = m_failedToRefreshText;
}

void UnitConverter::InitializeOfflineStatusTextBlock()
{
    auto resProvider = AppResourceProvider::GetInstance();
    std::wstring offlineStatusHyperlinkText = static_cast<String^>(resProvider.GetResourceString(L"OfflineStatusHyperlinkText"))->Data();

    // The resource string has the 'NetworkSettings' hyperlink wrapped with '%HL%'.
    // Break the string and assign pieces appropriately.
    static const std::wstring delimiter{ L"%HL%" };
    static const size_t delimiterLength{ delimiter.length() };

    // Find the delimiters.
    size_t firstSplitPosition = offlineStatusHyperlinkText.find(delimiter, 0);
    assert(firstSplitPosition != std::wstring::npos);
    size_t secondSplitPosition = offlineStatusHyperlinkText.find(delimiter, firstSplitPosition + 1);
    assert(secondSplitPosition != std::wstring::npos);
    size_t hyperlinkTextLength = secondSplitPosition - (firstSplitPosition + delimiterLength);

    // Assign pieces.
    auto offlineStatusTextBeforeHyperlink = ref new String(offlineStatusHyperlinkText.substr(0, firstSplitPosition).c_str());
    auto offlineStatusTextLink = ref new String(offlineStatusHyperlinkText.substr(firstSplitPosition + delimiterLength, hyperlinkTextLength).c_str());
    auto offlineStatusTextAfterHyperlink = ref new String(offlineStatusHyperlinkText.substr(secondSplitPosition + delimiterLength).c_str());

    OfflineRunBeforeLink->Text = offlineStatusTextBeforeHyperlink;
    OfflineRunLink->Text = offlineStatusTextLink;
    OfflineRunAfterLink->Text = offlineStatusTextAfterHyperlink;

    AutomationProperties::SetName(OfflineBlock,
        offlineStatusTextBeforeHyperlink + L" " +
        offlineStatusTextLink + L" " +
        offlineStatusTextAfterHyperlink);
}

void UnitConverter::SetCurrencyTimestampFontWeight()
{
    if (Model->CurrencyDataIsWeekOld)
    {
        VisualStateManager::GoToState(this, L"WeekOldTimestamp", false);
    }
    else
    {
        VisualStateManager::GoToState(this, L"DefaultTimestamp", false);
    }
}

void UnitConverter::OnValueKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
    if (e->Key == VirtualKey::Space)
    {
        OnValueSelected(sender);
    }
}

void UnitConverter::OnContextRequested(UIElement^ sender, ContextRequestedEventArgs^ e)
{
    OnValueSelected(sender);
    auto requestedElement = safe_cast<FrameworkElement^>(sender);

    PasteMenuItem->IsEnabled = CopyPasteManager::HasStringToPaste();

    Point point;
    if (e->TryGetPosition(requestedElement, &point))
    {
        m_resultsFlyout->ShowAt(requestedElement, point);
    }
    else
    {
        // Not invoked via pointer, so let XAML choose a default location.
        m_resultsFlyout->ShowAt(requestedElement);
    }

    e->Handled = true;
}

void UnitConverter::OnContextCanceled(UIElement^ sender, RoutedEventArgs^ e)
{
    m_resultsFlyout->Hide();
}

void UnitConverter::OnCopyMenuItemClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    auto calcResult = safe_cast<CalculationResult^>(m_resultsFlyout->Target);
    CopyPasteManager::CopyToClipboard(calcResult->GetRawDisplayValue());
}

void UnitConverter::OnPasteMenuItemClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    CopyPasteManager::GetStringToPaste(Model->Mode, CategoryGroupType::Converter).then([this](String^ pastedString)
    {
        Model->OnPaste(pastedString, Model->Mode);
    });
}

void UnitConverter::AnimateConverter()
{
    
    if (App::IsAnimationEnabled())
    {
        AnimationStory->Begin();
    }
}

void UnitConverter::OnValueSelected(_In_ Platform::Object^ sender)
{
    auto value = safe_cast<CalculationResult^>(sender);
    //update the font size since the font is changed to bold
    value->UpdateTextState();
    safe_cast<UnitConverterViewModel^>(this->DataContext)->OnValueActivated(AsActivatable(value));
}

void UnitConverter::UpdateDropDownState(_In_ Platform::Object^ sender, _In_ Platform::Object^ e)
{
    safe_cast<UnitConverterViewModel^>(this->DataContext)->IsDropDownOpen = (Units1->IsDropDownOpen) || (Units2->IsDropDownOpen);
    KeyboardShortcutManager::UpdateDropDownState((Units1->IsDropDownOpen) || (Units2->IsDropDownOpen));
}

void UnitConverter::OnLoaded(_In_ Object^, _In_ RoutedEventArgs^)
{
}

void UnitConverter::SetDefaultFocus()
{
    const std::vector<Control^> focusPrecedence = {
        Value1,
        CurrencyRefreshBlockControl,
        OfflineBlock,
        clearEntryButtonPos0
    };

    for (Control^ control : focusPrecedence)
    {
        if (control->Focus(::FocusState::Programmatic))
        {
            break;
        }
    }
}

void UnitConverter::CurrencyRefreshButton_Click(_In_ Object^ /*sender*/, _In_ RoutedEventArgs^ /*e*/)
{
    if (Model->NetworkBehavior == NetworkAccessBehavior::OptIn)
    {
        m_meteredConnectionOverride = true;
    }

    Model->RefreshCurrencyRatios();
}

void UnitConverter::OnDataContextChanged(_In_ FrameworkElement^ sender, _In_ DataContextChangedEventArgs^ args)
{
    Model->PropertyChanged -= m_propertyChangedToken;

    m_propertyChangedToken =
        Model->PropertyChanged += ref new PropertyChangedEventHandler(this, &UnitConverter::OnPropertyChanged);

    OnNetworkBehaviorChanged();
}

void UnitConverter::Units1_IsEnabledChanged(Object^ sender, DependencyPropertyChangedEventArgs^ e)
{
    if ((Units1->Visibility == ::Visibility::Visible) && Units1->IsEnabled)
    {
        SetDefaultFocus();
    }
}

void UnitConverter::OnIsDisplayVisibleChanged()
{
    if (Model->IsCurrencyLoadingVisible)
    {
        StartProgressRingWithDelay();
    }
    else
    {
        HideProgressRing();

        if (m_isAnimationEnabled && Model->IsCurrencyCurrentCategory && !Model->CurrencyTimestamp->IsEmpty())
        {
            TimestampFadeInAnimation->Begin();
        }
    }
}

void UnitConverter::StartProgressRingWithDelay()
{
    HideProgressRing();

    TimeSpan delay{};
    delay.Duration = DURATION_500_MS;

    m_delayTimer = ref new DispatcherTimer();
    m_delayTimer->Interval = delay;
    m_delayTimer->Tick += ref new EventHandler<Object^>(this, &UnitConverter::OnDelayTimerTick);

    m_delayTimer->Start();
}

void UnitConverter::OnDelayTimerTick(Object^ /*sender*/, Object^ /*e*/)
{
    CurrencyLoadingProgressRing->IsActive = true;
    m_delayTimer->Stop();
}

void UnitConverter::HideProgressRing()
{
    if (m_delayTimer != nullptr)
    {
        m_delayTimer->Stop();
    }

    CurrencyLoadingProgressRing->IsActive = false;
}
