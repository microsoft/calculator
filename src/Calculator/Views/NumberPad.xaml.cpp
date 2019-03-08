// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// NumberPad.xaml.cpp
// Implementation of the NumberPad class
//

#include "pch.h"
#include "NumberPad.xaml.h"
#include "Controls/CalculatorButton.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"
#include "CalcViewModel/Common/LocalizationSettings.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace CalculatorApp::Common;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

DEPENDENCY_PROPERTY_INITIALIZATION(NumberPad, ButtonStyle);

NumberPad::NumberPad() :
    m_isErrorVisualState(false)
{
    InitializeComponent();

    const auto& localizationSettings = LocalizationSettings::GetInstance();
    
    this->decimalSeparatorButton->Content = localizationSettings.GetDecimalSeparator();
    this->num0Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('0');
    this->num1Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('1');
    this->num2Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('2');
    this->num3Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('3');
    this->num4Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('4');
    this->num5Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('5');
    this->num6Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('6');
    this->num7Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('7');
    this->num8Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('8');
    this->num9Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('9');
}

void NumberPad::ProgModeRadixChange()
{
    num0Button->IsEnabled = true;
    num1Button->IsEnabled = true;
    num2Button->IsEnabled = true;
    num3Button->IsEnabled = true;
    num4Button->IsEnabled = true;
    num5Button->IsEnabled = true;
    num6Button->IsEnabled = true;
    num7Button->IsEnabled = true;
    num8Button->IsEnabled = true;
    num9Button->IsEnabled = true;

    auto vm = safe_cast<StandardCalculatorViewModel^>(this->DataContext);
    RADIX_TYPE radixType = vm->GetCurrentRadixType();

    if (radixType == RADIX_TYPE::BIN_RADIX)
    {
        num2Button->IsEnabled = false;
        num3Button->IsEnabled = false;
        num4Button->IsEnabled = false;
        num5Button->IsEnabled = false;
        num6Button->IsEnabled = false;
        num7Button->IsEnabled = false;
        num8Button->IsEnabled = false;
        num9Button->IsEnabled = false;
    }
    else if (radixType == RADIX_TYPE::OCT_RADIX)
    {
        num8Button->IsEnabled = false;
        num9Button->IsEnabled = false;
    }
}

bool NumberPad::IsErrorVisualState::get()
{
    return m_isErrorVisualState;
}

void NumberPad::IsErrorVisualState::set(bool value)
{
    if (m_isErrorVisualState != value)
    {
        m_isErrorVisualState = value;
        String^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
    }
}
