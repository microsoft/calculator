// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// FinanceCalculator.xaml.cpp
// Implementation of the FinanceCalculator class
//

#include "pch.h"
#include "CalcViewModel/Common/CopyPasteManager.h"
#include "FinanceCalculator.xaml.h"



using namespace CalculatorApp;

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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

FinanceCalculator::FinanceCalculator()
{
	InitializeComponent();

    CopyMenuItem->Text = AppResourceProvider::GetInstance()->GetResourceString(L"copyMenuItem");
}

void FinanceCalculator::SetDefaultFocus()
{
    FinanceCalculationOption->Focus(::FocusState::Programmatic);
}

double FinanceCalculator::FutureValue()
{
    // Convert Baserate textbox to double
    String ^ BaseRateTextBox = BaseRate->Text;
    std::wstring BaseRateString(BaseRateTextBox->Data());
    double Base = std::stod(BaseRateString);

    // Convert Interest textbox to double
    String ^ InterestTextBox = InterestRate->Text;
    std::wstring InterestString(InterestTextBox->Data());
    double Interest = std::stod(InterestString);

    double InterestDecimal = Interest / 100;

    // Convert Term textbox to double
    String ^ TermTextbox = Term->Text;
    std::wstring TermString(TermTextbox->Data());
    double Years = std::stod(TermString);

    // Convert how many times it is compounded field to double
    String ^ CompoundedTextBox = Compounded->Text;
    std::wstring CompoundedString(CompoundedTextBox->Data());
    double Frequency = std::stod(CompoundedString);

    // Equation for calculating the future value (Formula: BaseRate *(1 + (Interest[decimal] / Frequency))^(Years * Frequency))
    double CompoundedValue = Base * (pow(1 + (InterestDecimal / Frequency), Years * Frequency));

    return CompoundedValue;
}

double FinanceCalculator::InterestEarned()
{
    double CompoundedValue = FutureValue();

    String ^ BaseRateTextBox = BaseRate->Text;
    std::wstring BaseRateString(BaseRateTextBox->Data());
    double Base = std::stod(BaseRateString);

    double Earnings = CompoundedValue - Base;

    return Earnings;
}

void CalculatorApp::FinanceCalculator::CalculateInterestButton_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    // Convert Term textbox to double
    String ^ TermTextbox = Term->Text;
    std::wstring TermString(TermTextbox->Data());
    double Years = std::stod(TermString);

    double CompoundedValue = FutureValue();
    double Earnings = InterestEarned();

    try
    {
        CompoundResults->Text = AppResourceProvider::GetInstance()->GetResourceString(L"CurrencySymbol") + CompoundedValue.ToString();
        CompoundSecondaryResults->Text = AppResourceProvider::GetInstance()->GetResourceString(L"Over") + " " + Years.ToString() + " "
                                         + AppResourceProvider::GetInstance()->GetResourceString(L"Date_Years") + ". "
                                         + AppResourceProvider::GetInstance()->GetResourceString(L"TotalInterestEarned")+" "+Earnings.ToString();
    }
    catch (Platform::InvalidArgumentException ^ ex)
    {
        CompoundResults->Text = AppResourceProvider::GetInstance()->GetResourceString(L"CalculationFailed");
    }
}

void CalculatorApp::FinanceCalculator::OnCopyMenuItemClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    auto result = safe_cast<TextBlock ^>(Finance_ResultsContextMenu->Target);

    CopyPasteManager::CopyToClipboard(result->Text);
}
