// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorProgrammerRadixOperators.xaml.cpp
// Implementation of the CalculatorProgrammerRadixOperators class
//

#include "pch.h"
#include "CalculatorProgrammerRadixOperators.xaml.h"
#include "Controls/CalculatorButton.h"
#include "Converters/BooleanToVisibilityConverter.h"
#include "Views/NumberPad.xaml.h"
#include <CalcViewModel\Common\AppResourceProvider.h>

using namespace std;
using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace CalculatorApp::Common;
using namespace Windows::UI::Xaml::Media;

CalculatorProgrammerRadixOperators::CalculatorProgrammerRadixOperators()
    : m_isErrorVisualState(false)
{
    InitializeComponent();
    LoadResourceStrings();
}

void CalculatorProgrammerRadixOperators::LoadResourceStrings()
{
    auto resProvider = AppResourceProvider::GetInstance();
    m_arithmeticShiftButtonContent = resProvider->GetResourceString(L"arithmeticShiftButtonSelected");
    m_logicalShiftButtonContent = resProvider->GetResourceString(L"logicalShiftButtonSelected");
    m_rotateCircularButtonContent = resProvider->GetResourceString(L"rotateCircularButtonSelected");
    m_rotateCarryShiftButtonContent = resProvider->GetResourceString(L"rotateCarryShiftButtonSelected");
}

void CalculatorProgrammerRadixOperators::FlyoutButton_Clicked(_In_ Platform::Object ^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs ^ /*e*/)
{
    this->BitwiseFlyout->Hide();
}

void CalculatorProgrammerRadixOperators::checkDefaultBitShift()
{
    LoadDeferredLoadButtons();

    if (IsButtonLoaded())
    {
        return;
    }

    CollapseBitshiftButtons();

    m_selectedShiftButtonMode = BitShiftMode::Arithmetic;
    LshButton->Visibility = ::Visibility::Visible;
    RshButton->Visibility = ::Visibility::Visible;
    LshButton->IsEnabled = true;
    RshButton->IsEnabled = true;
}

bool CalculatorApp::CalculatorProgrammerRadixOperators::IsButtonLoaded()
{
    // Since arithmeticShiftButton defaults to IsChecked = true, this event an fire before we can load the deferred loaded controls. If that is the case, just
    // return and do nothing.
    return RolButton == nullptr || RorButton == nullptr || RolCarryButton == nullptr || RorCarryButton == nullptr || LshLogicalButton == nullptr
           || RshLogicalButton == nullptr;
}

void CalculatorApp::CalculatorProgrammerRadixOperators::LoadDeferredLoadButtons()
{
    // Load deferred load buttons
    if (RolButton == nullptr)
    {
        FindName("RolButton");
        FindName("RorButton");
        FindName("RolCarryButton");
        FindName("RorCarryButton");
        FindName("LshLogicalButton");
        FindName("RshLogicalButton");
    }
}

void CalculatorProgrammerRadixOperators::BitshiftFlyout_Checked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    // Load deferred load buttons
    LoadDeferredLoadButtons();

    if (IsButtonLoaded())
    {
        return;
    }

    CollapseBitshiftButtons();

    auto radioButton = static_cast<RadioButton ^>(sender);
    Platform::String ^ announcementString = L"";
    BitShiftMode selectedButtonMode = m_selectedShiftButtonMode;

    if (radioButton == ArithmeticShiftButton)
    {
        LshButton->Visibility = ::Visibility::Visible;
        RshButton->Visibility = ::Visibility::Visible;
        LshButton->IsEnabled = true;
        RshButton->IsEnabled = true;
        announcementString = m_arithmeticShiftButtonContent;
        selectedButtonMode = BitShiftMode::Arithmetic;
    }
    else if (radioButton == LogicalShiftButton)
    {
        LshLogicalButton->Visibility = ::Visibility::Visible;
        RshLogicalButton->Visibility = ::Visibility::Visible;
        LshLogicalButton->IsEnabled = true;
        RshLogicalButton->IsEnabled = true;
        announcementString = m_logicalShiftButtonContent;
        selectedButtonMode = BitShiftMode::LogicalShift;
    }
    else if (radioButton == RotateCircularButton)
    {
        RolButton->Visibility = ::Visibility::Visible;
        RorButton->Visibility = ::Visibility::Visible;
        RolButton->IsEnabled = true;
        RorButton->IsEnabled = true;
        announcementString = m_rotateCircularButtonContent;
        selectedButtonMode = BitShiftMode::RotateCircular;
    }
    else if (radioButton == RotateCarryShiftButton)
    {
        RolCarryButton->Visibility = ::Visibility::Visible;
        RorCarryButton->Visibility = ::Visibility::Visible;
        RolCarryButton->IsEnabled = true;
        RorCarryButton->IsEnabled = true;
        announcementString = m_rotateCarryShiftButtonContent;
        selectedButtonMode = BitShiftMode::RotateCarry;
    }

    if (selectedButtonMode != m_selectedShiftButtonMode)
    {
        this->BitShiftFlyout->Hide();
        m_selectedShiftButtonMode = selectedButtonMode;
    }

    Model->SetBitshiftRadioButtonCheckedAnnouncement(announcementString);
}

void CalculatorProgrammerRadixOperators::CollapseBitshiftButtons()
{
    RolButton->Visibility = ::Visibility::Collapsed;
    RorButton->Visibility = ::Visibility::Collapsed;
    RolCarryButton->Visibility = ::Visibility::Collapsed;
    RorCarryButton->Visibility = ::Visibility::Collapsed;
    LshButton->Visibility = ::Visibility::Collapsed;
    RshButton->Visibility = ::Visibility::Collapsed;
    LshLogicalButton->Visibility = ::Visibility::Collapsed;
    RshLogicalButton->Visibility = ::Visibility::Collapsed;

    // We need to set the collapsed buttons to disabled so that the KeyboardShortcutManager can skip the keybinds for the disabled buttons
    RolButton->IsEnabled = false;
    RorButton->IsEnabled = false;
    RolCarryButton->IsEnabled = false;
    RorCarryButton->IsEnabled = false;
    LshButton->IsEnabled = false;
    RshButton->IsEnabled = false;
    LshLogicalButton->IsEnabled = false;
    RshLogicalButton->IsEnabled = false;
}

bool CalculatorProgrammerRadixOperators::IsErrorVisualState::get()
{
    return m_isErrorVisualState;
}

void CalculatorProgrammerRadixOperators::IsErrorVisualState::set(bool value)
{
    if (m_isErrorVisualState != value)
    {
        m_isErrorVisualState = value;
        String ^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
        NumberPad->IsErrorVisualState = m_isErrorVisualState;
    }
}

String ^ CalculatorProgrammerRadixOperators::ParenthesisCountToString(unsigned int count)
{
    return (count == 0) ? ref new String() : ref new String(to_wstring(count).data());
}

void CalculatorProgrammerRadixOperators::CalculatorProgrammerRadixOperators::OpenParenthesisButton_GotFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    Model->SetOpenParenthesisCountNarratorAnnouncement();
}

void CalculatorProgrammerRadixOperators::ClearEntryButton_LostFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    if (ClearEntryButton->Visibility == ::Visibility::Collapsed && ClearButton->Visibility == ::Visibility::Visible)
    {
        ClearButton->Focus(::FocusState::Programmatic);
    }
}

void CalculatorProgrammerRadixOperators::ClearButton_LostFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    if (ClearEntryButton->Visibility == ::Visibility::Visible && ClearButton->Visibility == ::Visibility::Collapsed)
    {
        ClearEntryButton->Focus(::FocusState::Programmatic);
    }
}

void CalculatorApp::CalculatorProgrammerRadixOperators::BitShiftFlyout_Opened(Platform::Object ^ sender, Platform::Object ^ e)
{
    if (m_selectedShiftButtonMode == BitShiftMode::Arithmetic)
    {
        ArithmeticShiftButton->IsChecked = true;
    }
    else if (m_selectedShiftButtonMode == BitShiftMode::LogicalShift)
    {
        LogicalShiftButton->IsChecked = true;
    }
    else if (m_selectedShiftButtonMode == BitShiftMode::RotateCircular)
    {
        RotateCircularButton->IsChecked = true;
    }
    else if (m_selectedShiftButtonMode == BitShiftMode::RotateCarry)
    {
        RotateCarryShiftButton->IsChecked = true;
    }
}
