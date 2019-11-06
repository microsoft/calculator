#include "pch.h"
#include "MathRichEditBox.h"

using namespace Platform;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace std;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation::Collections;

DEPENDENCY_PROPERTY_INITIALIZATION(MathRichEditBox, MathText);

MathRichEditBox::MathRichEditBox()
{
    Windows::ApplicationModel::LimitedAccessFeatures::TryUnlockFeature(
        "com.microsoft.windows.richeditmath",
        "H6wflFFz3gkOsAHtG/D9Tg==",
        "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.");

    TextDocument->SetMathMode(Windows::UI::Text::RichEditMathMode::MathOnly);
}

String ^ MathRichEditBox::GetMathTextProperty()
{
    String ^ text;
    this->TextDocument->GetMath(&text);
    return text;
}

void MathRichEditBox::SetMathTextProperty(String ^ newValue)
{
    bool readOnlyState = this->IsReadOnly;
    this->IsReadOnly = false;

    TextDocument->SetMath(newValue);

    this->IsReadOnly = readOnlyState;
}

void MathRichEditBox::OnLostFocus(RoutedEventArgs ^ e)
{
    this->Document->Selection->SetRange(0, 0);

}
