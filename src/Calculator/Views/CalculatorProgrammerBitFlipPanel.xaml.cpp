// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorProgrammerBitFlipPanel.xaml.cpp
// Implementation of the CalculatorProgrammerBitFlipPanel class
//

#include "pch.h"
#include "CalculatorProgrammerBitFlipPanel.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "CalcViewModel/Common/LocalizationSettings.h"
#include "Converters/BooleanToVisibilityConverter.h"
#include <CalcViewModel/Common/AppResourceProvider.h>
#include "CalcViewModel/Common/LocalizationStringUtil.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace std;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

CalculatorProgrammerBitFlipPanel::CalculatorProgrammerBitFlipPanel()
    : m_updatingCheckedStates(false)
{
    InitializeComponent();
    AssignFlipButtons();
}

void CalculatorProgrammerBitFlipPanel::OnLoaded(Object ^ sender, RoutedEventArgs ^ e)
{
    SubscribePropertyChanged();
}

void CalculatorProgrammerBitFlipPanel::OnUnloaded(Object ^ sender, RoutedEventArgs ^ e)
{
    UnsubscribePropertyChanged();
}

void CalculatorProgrammerBitFlipPanel::SubscribePropertyChanged()
{
    if (Model != nullptr)
    {
        m_propertyChangedToken = Model->PropertyChanged += ref new PropertyChangedEventHandler(this, &CalculatorProgrammerBitFlipPanel::OnPropertyChanged);
        m_currentValueBitLength = Model->ValueBitLength;
        UpdateCheckedStates(true);
    }
}

void CalculatorProgrammerBitFlipPanel::UnsubscribePropertyChanged()
{
    if (Model != nullptr && m_propertyChangedToken.Value != 0)
    {
        Model->PropertyChanged -= m_propertyChangedToken;
        m_propertyChangedToken.Value = 0;
    }
}

void CalculatorProgrammerBitFlipPanel::OnPropertyChanged(Object ^ sender, PropertyChangedEventArgs ^ e)
{
    if (e->PropertyName == StandardCalculatorViewModel::BinaryDigitsPropertyName)
    {
        UpdateCheckedStates(false);
        m_currentValueBitLength = Model->ValueBitLength;
    }
    else if (
        e->PropertyName == StandardCalculatorViewModel::IsBitFlipCheckedPropertyName
        || e->PropertyName == StandardCalculatorViewModel::IsProgrammerPropertyName)
    {
        if (Model->IsBitFlipChecked && Model->IsProgrammer)
        {
            // OnBitToggle won't update the automation properties when this control isn't displayed
            // We need to update all automation properties names manually when the BitFlipPanel is displayed again
            UpdateAutomationPropertiesNames();
        }
    }
}

StandardCalculatorViewModel ^ CalculatorProgrammerBitFlipPanel::Model::get()
{
    return static_cast<StandardCalculatorViewModel ^>(this->DataContext);
}

void CalculatorProgrammerBitFlipPanel::AssignFlipButtons()
{
    assert(m_flipButtons.size() == 64);

    m_flipButtons[0] = this->Bit0;
    m_flipButtons[1] = this->Bit1;
    m_flipButtons[2] = this->Bit2;
    m_flipButtons[3] = this->Bit3;
    m_flipButtons[4] = this->Bit4;
    m_flipButtons[5] = this->Bit5;
    m_flipButtons[6] = this->Bit6;
    m_flipButtons[7] = this->Bit7;
    m_flipButtons[8] = this->Bit8;
    m_flipButtons[9] = this->Bit9;
    m_flipButtons[10] = this->Bit10;
    m_flipButtons[11] = this->Bit11;
    m_flipButtons[12] = this->Bit12;
    m_flipButtons[13] = this->Bit13;
    m_flipButtons[14] = this->Bit14;
    m_flipButtons[15] = this->Bit15;
    m_flipButtons[16] = this->Bit16;
    m_flipButtons[17] = this->Bit17;
    m_flipButtons[18] = this->Bit18;
    m_flipButtons[19] = this->Bit19;
    m_flipButtons[20] = this->Bit20;
    m_flipButtons[21] = this->Bit21;
    m_flipButtons[22] = this->Bit22;
    m_flipButtons[23] = this->Bit23;
    m_flipButtons[24] = this->Bit24;
    m_flipButtons[25] = this->Bit25;
    m_flipButtons[26] = this->Bit26;
    m_flipButtons[27] = this->Bit27;
    m_flipButtons[28] = this->Bit28;
    m_flipButtons[29] = this->Bit29;
    m_flipButtons[30] = this->Bit30;
    m_flipButtons[31] = this->Bit31;
    m_flipButtons[32] = this->Bit32;
    m_flipButtons[33] = this->Bit33;
    m_flipButtons[34] = this->Bit34;
    m_flipButtons[35] = this->Bit35;
    m_flipButtons[36] = this->Bit36;
    m_flipButtons[37] = this->Bit37;
    m_flipButtons[38] = this->Bit38;
    m_flipButtons[39] = this->Bit39;
    m_flipButtons[40] = this->Bit40;
    m_flipButtons[41] = this->Bit41;
    m_flipButtons[42] = this->Bit42;
    m_flipButtons[43] = this->Bit43;
    m_flipButtons[44] = this->Bit44;
    m_flipButtons[45] = this->Bit45;
    m_flipButtons[46] = this->Bit46;
    m_flipButtons[47] = this->Bit47;
    m_flipButtons[48] = this->Bit48;
    m_flipButtons[49] = this->Bit49;
    m_flipButtons[50] = this->Bit50;
    m_flipButtons[51] = this->Bit51;
    m_flipButtons[52] = this->Bit52;
    m_flipButtons[53] = this->Bit53;
    m_flipButtons[54] = this->Bit54;
    m_flipButtons[55] = this->Bit55;
    m_flipButtons[56] = this->Bit56;
    m_flipButtons[57] = this->Bit57;
    m_flipButtons[58] = this->Bit58;
    m_flipButtons[59] = this->Bit59;
    m_flipButtons[60] = this->Bit60;
    m_flipButtons[61] = this->Bit61;
    m_flipButtons[62] = this->Bit62;
    m_flipButtons[63] = this->Bit63;
}

void CalculatorProgrammerBitFlipPanel::OnBitToggled(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    if (m_updatingCheckedStates)
    {
        return;
    }

    // Handle this the bit toggled event only if it is coming from BitFlip mode.
    // Any input from the Numpad may also result in toggling the bit as their state is bound to the BinaryDisplayValue.
    // Also, if the mode is switched to other Calculator modes when the BitFlip panel is open,
    // a race condition exists in which the IsProgrammerMode property is still true and the UpdatePrimaryResult() is called,
    // which continuously alters the Display Value and the state of the Bit Flip buttons.
    if (Model->IsBitFlipChecked && Model->IsProgrammer)
    {
        auto flipButton = static_cast<FlipButtons ^>(sender);
        int index = static_cast<int>(flipButton->Tag);
        flipButton->SetValue(AutomationProperties::NameProperty, GenerateAutomationPropertiesName(index, flipButton->IsChecked->Value));
        Model->ButtonPressed->Execute(flipButton->ButtonId);
    }
}

void CalculatorProgrammerBitFlipPanel::UpdateCheckedStates(bool updateAutomationPropertiesNames)
{
    assert(!m_updatingCheckedStates);
    assert(m_flipButtons.size() == s_numBits);

    if (Model == nullptr)
    {
        return;
    }

    m_updatingCheckedStates = true;
    auto it = m_flipButtons.begin();
    int index = 0;
    bool mustUpdateTextOfMostSignificantDigits = m_currentValueBitLength != Model->ValueBitLength;
    int previousMSDPosition = GetIndexOfLastBit(m_currentValueBitLength);
    int newMSDPosition = GetIndexOfLastBit(Model->ValueBitLength);
    for (bool val : Model->BinaryDigits)
    {
        FlipButtons ^ flipButton = *it;
        bool hasValueChanged = flipButton->IsChecked->Value != val;
        flipButton->IsChecked = val;
        if (updateAutomationPropertiesNames
            || hasValueChanged
            || (mustUpdateTextOfMostSignificantDigits && (index == previousMSDPosition || index == newMSDPosition)))
        {
            flipButton->SetValue(AutomationProperties::NameProperty, GenerateAutomationPropertiesName(index, val));
        }
        ++it;
        ++index;
    }

    m_updatingCheckedStates = false;
}

void CalculatorProgrammerBitFlipPanel::UpdateAutomationPropertiesNames()
{
    for (FlipButtons ^ flipButton : m_flipButtons)
    {
        int index = static_cast<int>(flipButton->Tag);
        flipButton->SetValue(AutomationProperties::NameProperty, GenerateAutomationPropertiesName(index, flipButton->IsChecked->Value));
    }
}

bool CalculatorProgrammerBitFlipPanel::ShouldEnableBit(BitLength length, int index)
{
    return index <= GetIndexOfLastBit(length);
}

int CalculatorProgrammerBitFlipPanel::GetIndexOfLastBit(BitLength length) const
{
    switch (length)
    {
    case BitLength::BitLengthQWord:
        return 63;
    case BitLength::BitLengthDWord:
        return 31;
    case BitLength::BitLengthWord:
        return 15;
    case BitLength::BitLengthByte:
        return 7;
    }
    return -1;
}

String ^ CalculatorProgrammerBitFlipPanel::GenerateAutomationPropertiesName(int position, bool value)
{
    auto resourceLoader = AppResourceProvider::GetInstance();
    String ^ automationNameTemplate = resourceLoader->GetResourceString(L"BitFlipItemAutomationName");
    String ^ bitPosition;
    if (position == 0)
    {
        bitPosition = resourceLoader->GetResourceString(L"LeastSignificantBit");
    }
    else
    {
        int lastPosition = -1;
        if (Model != nullptr)
        {
            lastPosition = GetIndexOfLastBit(Model->ValueBitLength);
        }

        if (position == lastPosition)
        {
            bitPosition = resourceLoader->GetResourceString(L"MostSignificantBit");
        }
        else
        {
            String ^ indexName = resourceLoader->GetResourceString(ref new Platform::String(to_wstring(position).c_str()));
            String ^ bitPositionTemplate = resourceLoader->GetResourceString(L"BitPosition");
            bitPosition = LocalizationStringUtil::GetLocalizedString(bitPositionTemplate, indexName);
        }
    }
    return LocalizationStringUtil::GetLocalizedString(automationNameTemplate, bitPosition, value ? L"1" : L"0");
}
