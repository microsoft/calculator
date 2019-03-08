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

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace std;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

CalculatorProgrammerBitFlipPanel::CalculatorProgrammerBitFlipPanel() :
    m_updatingCheckedStates(false)
{
    InitializeComponent();
    auto booleanToVisibilityConverter = ref new Converters::BooleanToVisibilityConverter;
    SetVisibilityBinding(BitFlipPanel, L"IsBinaryBitFlippingEnabled", booleanToVisibilityConverter);

    AssignFlipButtons();
}

void CalculatorProgrammerBitFlipPanel::OnLoaded(Object^ sender, RoutedEventArgs^ e)
{
    UnsubscribePropertyChanged();
    SubscribePropertyChanged();
}

void CalculatorProgrammerBitFlipPanel::OnUnloaded(Object^ sender, RoutedEventArgs^ e)
{
    UnsubscribePropertyChanged();
}

void CalculatorProgrammerBitFlipPanel::SubscribePropertyChanged()
{
    if (Model != nullptr)
    {
        m_propertyChangedToken =
            Model->PropertyChanged += ref new PropertyChangedEventHandler(this, &CalculatorProgrammerBitFlipPanel::OnPropertyChanged);

        UpdateCheckedStates();
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

void CalculatorProgrammerBitFlipPanel::OnPropertyChanged(Object^ sender, PropertyChangedEventArgs^ e)
{
    if (e->PropertyName == CalculatorViewModelProperties::BinaryDisplayValue)
    {
        UpdateCheckedStates();
    }
}

StandardCalculatorViewModel^ CalculatorProgrammerBitFlipPanel::Model::get()
{
    return static_cast<StandardCalculatorViewModel^>(this->DataContext);
}

void CalculatorProgrammerBitFlipPanel::AssignFlipButtons()
{
    assert(m_flipButtons.size() == 64);

    m_flipButtons[0]  = this->Bit0;
    m_flipButtons[1]  = this->Bit1;
    m_flipButtons[2]  = this->Bit2;
    m_flipButtons[3]  = this->Bit3;
    m_flipButtons[4]  = this->Bit4;
    m_flipButtons[5]  = this->Bit5;
    m_flipButtons[6]  = this->Bit6;
    m_flipButtons[7]  = this->Bit7;
    m_flipButtons[8]  = this->Bit8;
    m_flipButtons[9]  = this->Bit9;
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

void CalculatorProgrammerBitFlipPanel::SetVisibilityBinding(_In_ FrameworkElement^ element, _In_ String^ path, _In_ IValueConverter^ converter)
{
    Binding^ commandBinding = ref new Binding();
    commandBinding->Path = ref new PropertyPath(path);
    commandBinding->Converter = converter;
    element->SetBinding(VisibilityProperty, commandBinding);
}

void CalculatorProgrammerBitFlipPanel::OnBitToggled(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    if (m_updatingCheckedStates) { return; }

    // Handle this the bit toggled event only if it is coming from BitFlip mode.
    // Any input from the Numpad may also result in toggling the bit as their state is bound to the BinaryDisplayValue.
    // Also, if the mode is switched to other Calculator modes when the BitFlip panel is open,
    // a race condition exists in which the IsProgrammerMode property is still true and the UpdatePrimaryResult() is called,
    // which continuously alters the Display Value and the state of the Bit Flip buttons.
    if ((Model->IsBitFlipChecked)
        && Model->IsProgrammer)
    {
        TraceLogger::GetInstance().LogBitFlipUsed();

        auto flipButton = static_cast<FlipButtons^>(sender);
        Model->ButtonPressed->Execute(flipButton->ButtonId);
    }
}

void CalculatorProgrammerBitFlipPanel::UpdateCheckedStates()
{
    assert(!m_updatingCheckedStates);
    assert(m_flipButtons.size() == s_numBits);

    if (Model == nullptr) { return; }

    static const wchar_t ch0 = LocalizationSettings::GetInstance().GetDigitSymbolFromEnUsDigit(L'0');

    // Filter any unwanted characters from the displayed string.
    static constexpr array<wchar_t, 4> unwantedChars = {
        L' ',
        Utils::LRE,
        Utils::PDF,
        Utils::LRO
    };

    wstringstream stream;
    wstring displayValue = Model->BinaryDisplayValue->Data();
    for (const wchar_t& c : displayValue)
    {
        if (find(begin(unwantedChars), end(unwantedChars), c) == unwantedChars.end())
        {
            stream << c;
        }
    }

    wstring rawDisplay = stream.str();
    size_t paddingCount = s_numBits - rawDisplay.length();
    wstring setBits = wstring(paddingCount, ch0) + rawDisplay;
    assert(setBits.length() == s_numBits);

    m_updatingCheckedStates = true;
    for (unsigned int bitIndex = 0; bitIndex < s_numBits; bitIndex++)
    {
        // Highest bit (64) is at index 0 in bit string.
        // To get bit 0, grab from opposite end of string.
        wchar_t bit = setBits[s_numBits - bitIndex - 1];

        m_flipButtons[bitIndex]->IsChecked = (bit != ch0);
    }

    m_updatingCheckedStates = false;
}
