// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GraphingNumPad.xaml.h"
#include "Views/NumberPad.xaml.h"
#include "Controls/CalculatorButton.h"
#include "CalcViewModel/Common/LocalizationSettings.h"
#include "Controls/MathRichEditBox.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;

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

GraphingNumPad::GraphingNumPad()
{
    InitializeComponent();
    const auto& localizationSettings = LocalizationSettings::GetInstance();
    DecimalSeparatorButton->Content = localizationSettings.GetDecimalSeparator();
    Num0Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('0');
    Num1Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('1');
    Num2Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('2');
    Num3Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('3');
    Num4Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('4');
    Num5Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('5');
    Num6Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('6');
    Num7Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('7');
    Num8Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('8');
    Num9Button->Content = localizationSettings.GetDigitSymbolFromEnUsDigit('9');
}

void GraphingNumPad::ShiftButton_Check(_In_ Platform::Object ^ /*sender*/, _In_ RoutedEventArgs ^ /*e*/)
{
    SetOperatorRowVisibility();
}

void GraphingNumPad::ShiftButton_Uncheck(_In_ Platform::Object ^ sender, _In_ RoutedEventArgs ^ /*e*/)
{
    ShiftButton->IsChecked = false;
    SetOperatorRowVisibility();

    GraphingNumPad::Button_Clicked(sender, nullptr);
}

void GraphingNumPad::TrigFlyoutShift_Toggle(_In_ Platform::Object ^ /*sender*/, _In_ RoutedEventArgs ^ /*e*/)
{
    SetTrigRowVisibility();
}

void GraphingNumPad::TrigFlyoutHyp_Toggle(_In_ Platform::Object ^ /*sender*/, _In_ RoutedEventArgs ^ /*e*/)
{
    SetTrigRowVisibility();
}

void GraphingNumPad::FlyoutButton_Clicked(_In_ Platform::Object ^ sender, _In_ RoutedEventArgs ^ /*e*/)
{
    this->HypButton->IsChecked = false;
    this->TrigShiftButton->IsChecked = false;
    this->Trigflyout->Hide();
    this->FuncFlyout->Hide();
    this->InequalityFlyout->Hide();

    GraphingNumPad::Button_Clicked(sender, nullptr);
}

void GraphingNumPad::ShiftButton_IsEnabledChanged(_In_ Platform::Object ^ /*sender*/, _In_ DependencyPropertyChangedEventArgs ^ /*e*/)
{
    SetOperatorRowVisibility();
}

void GraphingNumPad::SetTrigRowVisibility()
{
    bool isShiftChecked = TrigShiftButton->IsChecked->Value;
    bool isHypeChecked = HypButton->IsChecked->Value;

    InverseHyperbolicTrigFunctions->Visibility = ::Visibility::Collapsed;
    InverseTrigFunctions->Visibility = ::Visibility::Collapsed;
    HyperbolicTrigFunctions->Visibility = ::Visibility::Collapsed;
    TrigFunctions->Visibility = ::Visibility::Collapsed;

    if (isShiftChecked && isHypeChecked)
    {
        InverseHyperbolicTrigFunctions->Visibility = ::Visibility::Visible;
    }
    else if (isShiftChecked && !isHypeChecked)
    {
        InverseTrigFunctions->Visibility = ::Visibility::Visible;
    }
    else if (!isShiftChecked && isHypeChecked)
    {
        HyperbolicTrigFunctions->Visibility = ::Visibility::Visible;
    }
    else
    {
        TrigFunctions->Visibility = ::Visibility::Visible;
    }
}

void GraphingNumPad::SetOperatorRowVisibility()
{
    ::Visibility rowVis, invRowVis;
    if (ShiftButton->IsChecked->Value)
    {
        rowVis = ::Visibility::Collapsed;
        invRowVis = ::Visibility::Visible;
    }
    else
    {
        rowVis = ::Visibility::Visible;
        invRowVis = ::Visibility::Collapsed;
    }

    Row1->Visibility = rowVis;
    InvRow1->Visibility = invRowVis;
}

const std::tuple<Platform::String ^, int, int> GraphingNumPad::GetButtonOutput(NumbersAndOperatorsEnum id)
{
    // Dictionary of the enum of the button clicked mapped to an object with the string to enter into the rich edit, and the start and end of the selection
    // after
    // text has been entered.
    static const std::unordered_map<NumbersAndOperatorsEnum, std::tuple<Platform::String ^, int, int>> buttonOutput = {
        { NumbersAndOperatorsEnum::Sin, { L"sin()", 4, 0 } },
        { NumbersAndOperatorsEnum::Cos, { L"cos()", 4, 0 } },
        { NumbersAndOperatorsEnum::Tan, { L"tan()", 4, 0 } },
        { NumbersAndOperatorsEnum::Sec, { L"sec()", 4, 0 } },
        { NumbersAndOperatorsEnum::Csc, { L"csc()", 4, 0 } },
        { NumbersAndOperatorsEnum::Cot, { L"cot()", 4, 0 } },
        { NumbersAndOperatorsEnum::InvSin, { L"arcsin()", 7, 0 } },
        { NumbersAndOperatorsEnum::InvCos, { L"arccos()", 7, 0 } },
        { NumbersAndOperatorsEnum::InvTan, { L"arctan()", 7, 0 } },
        { NumbersAndOperatorsEnum::InvSec, { L"arcsec()", 7, 0 } },
        { NumbersAndOperatorsEnum::InvCsc, { L"arccsc()", 7, 0 } },
        { NumbersAndOperatorsEnum::InvCot, { L"arccot()", 7, 0 } },
        { NumbersAndOperatorsEnum::Sinh, { L"sinh()", 5, 0 } },
        { NumbersAndOperatorsEnum::Cosh, { L"cosh()", 5, 0 } },
        { NumbersAndOperatorsEnum::Tanh, { L"tanh()", 5, 0 } },
        { NumbersAndOperatorsEnum::Sech, { L"sech()", 5, 0 } },
        { NumbersAndOperatorsEnum::Csch, { L"csch()", 5, 0 } },
        { NumbersAndOperatorsEnum::Coth, { L"coth()", 5, 0 } },
        { NumbersAndOperatorsEnum::InvSinh, { L"arcsinh()", 8, 0 } },
        { NumbersAndOperatorsEnum::InvCosh, { L"arccosh()", 8, 0 } },
        { NumbersAndOperatorsEnum::InvTanh, { L"arctanh()", 8, 0 } },
        { NumbersAndOperatorsEnum::InvSech, { L"arcsech()", 8, 0 } },
        { NumbersAndOperatorsEnum::InvCsch, { L"arccsch()", 8, 0 } },
        { NumbersAndOperatorsEnum::InvCoth, { L"arccoth()", 8, 0 } },
        { NumbersAndOperatorsEnum::Abs, { L"abs()", 4, 0 } },
        { NumbersAndOperatorsEnum::Floor, { L"floor()", 6, 0 } },
        { NumbersAndOperatorsEnum::Ceil, { L"ceiling()", 8, 0 } },
        { NumbersAndOperatorsEnum::Pi, { L"\u03C0", 1, 0 } },
        { NumbersAndOperatorsEnum::Euler, { L"e", 1, 0 } },
        { NumbersAndOperatorsEnum::XPower2, { L"^2", 2, 0 } },
        { NumbersAndOperatorsEnum::Cube, { L"^3", 2, 0 } },
        { NumbersAndOperatorsEnum::XPowerY, { L"^", 1, 0 } },
        { NumbersAndOperatorsEnum::TenPowerX, { L"10^", 3, 0 } },
        { NumbersAndOperatorsEnum::LogBase10, { L"log()", 4, 0 } },
        { NumbersAndOperatorsEnum::LogBaseE, { L"ln()", 3, 0 } },
        { NumbersAndOperatorsEnum::Sqrt, { L"sqrt()", 5, 0 } },
        { NumbersAndOperatorsEnum::CubeRoot, { L"cbrt()", 5, 0 } },
        { NumbersAndOperatorsEnum::YRootX, { L"root(x" + StringReference(LocalizationSettings::GetInstance().GetListSeparator().data()) + L"n)", 7, 1 } },
        { NumbersAndOperatorsEnum::TwoPowerX, { L"2^", 2, 0 } },
        { NumbersAndOperatorsEnum::LogBaseY, { "log(b" + StringReference(LocalizationSettings::GetInstance().GetListSeparator().data()) + L" x)", 4, 1 } },
        { NumbersAndOperatorsEnum::EPowerX, { L"e^", 4, 0 } },
        { NumbersAndOperatorsEnum::Abs, { L"abs()", 4, 0 } },
        { NumbersAndOperatorsEnum::X, { L"x", 1, 0 } },
        { NumbersAndOperatorsEnum::Y, { L"y", 1, 0 } },
        { NumbersAndOperatorsEnum::OpenParenthesis, { L"(", 1, 0 } },
        { NumbersAndOperatorsEnum::CloseParenthesis, { L")", 1, 0 } },
        { NumbersAndOperatorsEnum::Equals, { L"=", 1, 0 } },
        { NumbersAndOperatorsEnum::Divide, { L"/", 1, 0 } },
        { NumbersAndOperatorsEnum::Multiply, { L"*", 1, 0 } },
        { NumbersAndOperatorsEnum::Subtract, { L"-", 1, 0 } },
        { NumbersAndOperatorsEnum::Add, { L"+", 1, 0 } },
        { NumbersAndOperatorsEnum::Invert, { L"1/", 2, 0 } },
        { NumbersAndOperatorsEnum::Negate, { L"-", 1, 0 } },
        { NumbersAndOperatorsEnum::GreaterThan, { L">", 1, 0 } },
        { NumbersAndOperatorsEnum::GreaterThanOrEqualTo, { L"\u2265", 1, 0 } },
        { NumbersAndOperatorsEnum::LessThan, { L"<", 1, 0 } },
        { NumbersAndOperatorsEnum::LessThanOrEqualTo, { L"\u2264", 1, 0 } },
        { NumbersAndOperatorsEnum::Zero, { L"0", 1, 0 } },
        { NumbersAndOperatorsEnum::One, { L"1", 1, 0 } },
        { NumbersAndOperatorsEnum::Two, { L"2", 1, 0 } },
        { NumbersAndOperatorsEnum::Three, { L"3", 1, 0 } },
        { NumbersAndOperatorsEnum::Four, { L"4", 1, 0 } },
        { NumbersAndOperatorsEnum::Five, { L"5", 1, 0 } },
        { NumbersAndOperatorsEnum::Six, { L"6", 1, 0 } },
        { NumbersAndOperatorsEnum::Seven, { L"7", 1, 0 } },
        { NumbersAndOperatorsEnum::Eight, { L"8", 1, 0 } },
        { NumbersAndOperatorsEnum::Nine, { L"9", 1, 0 } },
        { NumbersAndOperatorsEnum::Decimal, { StringReference(LocalizationSettings::GetInstance().GetDecimalSeparatorStr().data()), 1, 0 } },
    };
    return buttonOutput.find(id)->second;
}

void GraphingNumPad::Button_Clicked(Platform::Object ^ sender, DependencyPropertyChangedEventArgs ^ /*e*/)
{
    auto mathRichEdit = GetActiveRichEdit();
    auto button = dynamic_cast<CalculatorApp::Controls::CalculatorButton ^>(sender);
    if (mathRichEdit != nullptr && sender != nullptr)
    {
        auto id = button->ButtonId;
        TraceLogger::GetInstance()->UpdateButtonUsage(id, CalculatorApp::Common::ViewMode::Graphing);
        std::tuple<Platform::String ^, int, int> output = GraphingNumPad::GetButtonOutput(id);
        mathRichEdit->InsertText(std::get<0>(output), std::get<1>(output), std::get<2>(output));
    }
}

void GraphingNumPad::SubmitButton_Clicked(Platform::Object ^ /*sender*/, RoutedEventArgs ^ /*e*/)
{
    auto mathRichEdit = GetActiveRichEdit();
    if (mathRichEdit != nullptr)
    {
        mathRichEdit->SubmitEquation(CalculatorApp::Controls::EquationSubmissionSource::ENTER_KEY);
        TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::Submit, CalculatorApp::Common::ViewMode::Graphing);
    }
}

void GraphingNumPad::ClearButton_Clicked(Platform::Object ^ /*sender*/, RoutedEventArgs ^ /*e*/)
{
    auto mathRichEdit = GetActiveRichEdit();
    if (mathRichEdit != nullptr)
    {
        String ^ text;
        mathRichEdit->TextDocument->GetText(Windows::UI::Text::TextGetOptions::NoHidden, &text);

        if (!text->IsEmpty())
        {
            mathRichEdit->MathText = L"<math xmlns=\"http://www.w3.org/1998/Math/MathML\"></math>";
            mathRichEdit->SubmitEquation(CalculatorApp::Controls::EquationSubmissionSource::PROGRAMMATIC);
        }

        TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::Clear, CalculatorApp::Common::ViewMode::Graphing);
    }
}

void GraphingNumPad::BackSpaceButton_Clicked(Platform::Object ^ /*sender*/, RoutedEventArgs ^ /*e*/)
{
    auto mathRichEdit = GetActiveRichEdit();
    if (mathRichEdit != nullptr)
    {
        mathRichEdit->BackSpace();
        TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::Backspace, CalculatorApp::Common::ViewMode::Graphing);
    }
}

// To avoid focus moving when the space between buttons is clicked, handle click events that make it through the keypad.
void GraphingNumPad::GraphingNumPad_PointerPressed(Platform::Object ^ /*sender*/, PointerRoutedEventArgs ^ e)
{
    e->Handled = true;
}

CalculatorApp::Controls::MathRichEditBox ^ GraphingNumPad::GetActiveRichEdit()
{
    return dynamic_cast<Controls::MathRichEditBox ^>(FocusManager::GetFocusedElement());
}

// Adding event because the ShowMode property is ignored in xaml.
void GraphingNumPad::Flyout_Opening(Platform::Object ^ sender, Platform::Object ^ /*e*/)
{
    auto flyout = dynamic_cast<Flyout ^>(sender);
    if (flyout)
    {
        flyout->ShowMode = FlyoutShowMode::Transient;
    }
}
