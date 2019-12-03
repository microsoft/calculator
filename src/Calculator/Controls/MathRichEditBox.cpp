// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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

// TODO remove when Windows 10 version 2004 SDK is adopted
namespace Windows_2004_Prerelease
{
    constexpr auto RichEditMathMode_MathOnly = 1;

    [uuid("619c20f2-cb3b-4521-981f-2865b1b93f04")] __interface ITextDocument4 : IInspectable
    {
        HRESULT SetMath(HSTRING value);
        HRESULT GetMath(HSTRING * value);
        HRESULT SetMathMode(int32_t mathMode);
    };
}

MathRichEditBox::MathRichEditBox()
{
    static LimitedAccessFeatureStatus m_lafResultStatus;
    String ^ packageName = Package::Current->Id->Name;

    if (packageName == L"Microsoft.WindowsCalculator.Dev")
    {
        m_lafResultStatus = LimitedAccessFeatures::TryUnlockFeature(
                                "com.microsoft.windows.richeditmath",
                                "BeDD/jxKhz/yfVNA11t4uA==", // Microsoft.WindowsCalculator.Dev
                                "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.")
                                ->Status;
    }

    else if (packageName == L"Microsoft.WindowsCalculator")
    {
        m_lafResultStatus = LimitedAccessFeatures::TryUnlockFeature(
                                "com.microsoft.windows.richeditmath",
                                "pfanNuxnzo+mAkBQ3N/rGQ==", // Microsoft.WindowsCalculator
                                "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.")
                                ->Status;
    }

    else if (packageName == L"Microsoft.WindowsCalculator.Graphing")
    {
        m_lafResultStatus = LimitedAccessFeatures::TryUnlockFeature(
                                "com.microsoft.windows.richeditmath",
                                "H6wflFFz3gkOsAHtG/D9Tg==", // Microsoft.WindowsCalculator.Graphing
                                "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.")
                                ->Status;
    }

    // TODO when Windows 10 version 2004 SDK is adopted, replace with:
    // TextDocument->SetMathMode(Windows::UI::Text::RichEditMathMode::MathOnly);
    Microsoft::WRL::ComPtr<Windows_2004_Prerelease::ITextDocument4> textDocument4;
    reinterpret_cast<IInspectable*>(this->TextDocument)->QueryInterface(IID_PPV_ARGS(&textDocument4));
    textDocument4->SetMathMode(Windows_2004_Prerelease::RichEditMathMode_MathOnly);
}

String ^ MathRichEditBox::GetMathTextProperty()
{
    // TODO when Windows 10 version 2004 SDK is adopted, replace with:
    // String ^ text;
    // this->TextDocument->GetMath(&text);
    // return text;

    Microsoft::WRL::ComPtr<Windows_2004_Prerelease::ITextDocument4> textDocument4;
    reinterpret_cast<IInspectable*>(this->TextDocument)->QueryInterface(IID_PPV_ARGS(&textDocument4));
    HSTRING math;
    textDocument4->GetMath(&math);
    return reinterpret_cast<String ^>(math);
}

void MathRichEditBox::SetMathTextProperty(String ^ newValue)
{
    bool readOnlyState = this->IsReadOnly;
    this->IsReadOnly = false;

    // TODO when Windows 10 version 2004 SDK is adopted, replace with:
    // TextDocument->SetMath(newValue);
    Microsoft::WRL::ComPtr<Windows_2004_Prerelease::ITextDocument4> textDocument4;
    reinterpret_cast<IInspectable*>(this->TextDocument)->QueryInterface(IID_PPV_ARGS(&textDocument4));
    textDocument4->SetMath(reinterpret_cast<HSTRING>(newValue));

    this->IsReadOnly = readOnlyState;
}
