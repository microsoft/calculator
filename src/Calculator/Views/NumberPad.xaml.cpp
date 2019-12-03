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
DEPENDENCY_PROPERTY_INITIALIZATION(NumberPad, CurrentRadixType);

NumberPad::NumberPad()
    : m_isErrorVisualState(false)
{
    InitializeComponent();

    const auto& localizationSettings = LocalizationSettings::GetInstance();

    this->DecimalSeparatorButton->Content = localizationSettings.GetDecimalSeparator();
    this->Num0Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('0');
    this->Num1Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('1');
    this->Num2Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('2');
    this->Num3Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('3');
    this->Num4Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('4');
    this->Num5Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('5');
    this->Num6Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('6');
    this->Num7Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('7');
    this->Num8Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('8');
    this->Num9Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('9');
}

void NumberPad::OnCurrentRadixTypePropertyChanged(NumberBase /* oldValue */, NumberBase newValue)
{
    Num0Button->IsEnabled = true;
    Num1Button->IsEnabled = true;
    Num2Button->IsEnabled = true;
    Num3Button->IsEnabled = true;
    Num4Button->IsEnabled = true;
    Num5Button->IsEnabled = true;
    Num6Button->IsEnabled = true;
    Num7Button->IsEnabled = true;
    Num8Button->IsEnabled = true;
    Num9Button->IsEnabled = true;

    if (newValue == NumberBase::BinBase)
    {
        Num2Button->IsEnabled = false;
        Num3Button->IsEnabled = false;
        Num4Button->IsEnabled = false;
        Num5Button->IsEnabled = false;
        Num6Button->IsEnabled = false;
        Num7Button->IsEnabled = false;
        Num8Button->IsEnabled = false;
        Num9Button->IsEnabled = false;
    }
    else if (newValue == NumberBase::OctBase)
    {
        Num8Button->IsEnabled = false;
        Num9Button->IsEnabled = false;
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
        String ^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
    }
}
