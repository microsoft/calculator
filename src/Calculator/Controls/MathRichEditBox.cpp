#include "pch.h"
#include "MathRichEditBox.h"

using namespace Platform;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace std;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation::Collections;

DEPENDENCY_PROPERTY_INITIALIZATION(MathRichEditBox, MathText);

MathRichEditBox::MathRichEditBox()
{
    static LimitedAccessFeatureStatus m_lafResultStatus;
    String ^ packageName = Package::Current->Id->Name;

    if (packageName == L"Microsoft.WindowsCalculator.Dev")
    {
        m_lafResultStatus = LimitedAccessFeatures::TryUnlockFeature(
            "com.microsoft.windows.richeditmath",
            "BeDD/jxKhz/yfVNA11t4uA==", // Microsoft.WindowsCalculator.Dev
            "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.")->Status;
    }

    else if (packageName == L"Microsoft.WindowsCalculator")
    {
        m_lafResultStatus = LimitedAccessFeatures::TryUnlockFeature(
            "com.microsoft.windows.richeditmath",
            "pfanNuxnzo+mAkBQ3N/rGQ==", // Microsoft.WindowsCalculator
            "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.")->Status;
    }

    else if (packageName == L"Microsoft.WindowsCalculator.Graphing")
    {
        m_lafResultStatus = LimitedAccessFeatures::TryUnlockFeature(
                                "com.microsoft.windows.richeditmath",
                                "H6wflFFz3gkOsAHtG/D9Tg==", // Microsoft.WindowsCalculator.Graphing
                                "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.")->Status;
    }

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
