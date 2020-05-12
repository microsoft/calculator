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
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Text;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Microsoft::WRL;

DEPENDENCY_PROPERTY_INITIALIZATION(MathRichEditBox, MathText);

// TODO remove when Windows 10 version 2004 SDK is adopted
namespace Windows_2004_Prerelease
{
    enum RichEditMathMode : int
    {
        NoMath,
        MathOnly,
    };
    MIDL_INTERFACE("619c20f2-cb3b-4521-981f-2865b1b93f04")
    ITextDocument4 : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetMath(HSTRING value) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetMath(HSTRING * value) = 0;
        virtual HRESULT STDMETHODCALLTYPE SetMathMode(RichEditMathMode mathMode) = 0;
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

    // TODO when Windows 10 version 2004 SDK is adopted, replace with:
    // TextDocument->SetMathMode(Windows::UI::Text::RichEditMathMode::MathOnly);
    ComPtr<Windows_2004_Prerelease::ITextDocument4> textDocument4;
    reinterpret_cast<IInspectable*>(this->TextDocument)->QueryInterface(IID_PPV_ARGS(&textDocument4));
    auto hr = textDocument4->SetMathMode(Windows_2004_Prerelease::RichEditMathMode::MathOnly);
    if (FAILED(hr))
    {
        throw Exception::CreateException(hr);
    }
    this->LosingFocus += ref new TypedEventHandler<UIElement ^,LosingFocusEventArgs ^>(this, &MathRichEditBox::OnLosingFocus);
    this->KeyUp += ref new KeyEventHandler(this, &MathRichEditBox::OnKeyUp);
}

String ^ MathRichEditBox::GetMathTextProperty()
{
    // TODO when Windows 10 version 2004 SDK is adopted, replace with:
    // String ^ text;
    // this->TextDocument->GetMath(&text);
    // return text;

    ComPtr<Windows_2004_Prerelease::ITextDocument4> textDocument4;
    reinterpret_cast<IInspectable*>(this->TextDocument)->QueryInterface(IID_PPV_ARGS(&textDocument4));
    HSTRING math;
    auto hr = textDocument4->GetMath(&math);
    if (FAILED(hr))
    {
        throw Exception::CreateException(hr);
    }
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
    auto hr = textDocument4->SetMath(reinterpret_cast<HSTRING>(newValue));
    if (FAILED(hr))
    {
        throw Exception::CreateException(hr);
    }

    this->IsReadOnly = readOnlyState;
}

void MathRichEditBox::OnLosingFocus(UIElement ^ sender, LosingFocusEventArgs ^ args)
{
    if (this->IsReadOnly || this->ContextFlyout->IsOpen)
    {
        return;
    }

    SubmitEquation(EquationSubmissionSource::FOCUS_LOST);
}

void MathRichEditBox::OnKeyUp(Object ^ sender, KeyRoutedEventArgs ^ e)
{
    if (!this->IsReadOnly && e->Key == VirtualKey::Enter)
    {
        SubmitEquation(EquationSubmissionSource::ENTER_KEY);
    }
}

void MathRichEditBox::OnKeyDown(KeyRoutedEventArgs ^ e)
{
    // suppress control + B to prevent bold input from being entered
    if ((Window::Current->CoreWindow->GetKeyState(VirtualKey::Control) & CoreVirtualKeyStates::Down) != CoreVirtualKeyStates::Down ||
        e->Key != VirtualKey::B)
    {
        Windows::UI::Xaml::Controls::RichEditBox::OnKeyDown(e);
    }
}

void MathRichEditBox::OnMathTextPropertyChanged(Platform::String ^ oldValue, Platform::String ^ newValue)
{
    SetMathTextProperty(newValue);

    // Get the new math text directly from the TextBox since the textbox may have changed its formatting
    SetValue(MathTextProperty, GetMathTextProperty());
}

void MathRichEditBox::InsertText(String ^ text, int cursorOffSet, int selectionLength)
{
    // If the rich edit is empty, the math zone may not exist, and so selection (and thus the resulting text) will not be in a math zone.
    // If the rich edit has content already, then the mathzone will already be created due to mathonly mode being set and the selection will exist inside the
    // math zone. To handle this, we will force a math zone to be created in teh case of the text being empty and then replacing the text inside of the math
    // zone with the newly inserted text.
    if (GetMathTextProperty() == nullptr)
    {
        SetMathTextProperty("<math xmlns=\"http://www.w3.org/1998/Math/MathML\"><mi>x</mi></math>");
        TextDocument->Selection->StartPosition = 0;
        TextDocument->Selection->EndPosition = 1;
    }

    // insert the text in place of selection
    TextDocument->Selection->SetText(Windows::UI::Text::TextSetOptions::FormatRtf, text);

    // Move the cursor to the next logical place for users to enter text.
    TextDocument->Selection->StartPosition += cursorOffSet;
    TextDocument->Selection->EndPosition = TextDocument->Selection->StartPosition + selectionLength;
}

void MathRichEditBox::BackSpace()
{
    // if anything is selected, just delete the selection.  Note: EndPosition can be before start position.
    if (TextDocument->Selection->StartPosition != TextDocument->Selection->EndPosition)
    {
        TextDocument->Selection->SetText(Windows::UI::Text::TextSetOptions::None, L"");
        return;
    }

    // if we are at the start of the string, do nothing
    if (TextDocument->Selection->StartPosition == 0)
    {
        return;
    }

    // Select the previous group.
    TextDocument->Selection->EndPosition = TextDocument->Selection->StartPosition;
    TextDocument->Selection->StartPosition -= 1;

    // If the group contains anything complex, we want to give the user a chance to preview the deletion.
    // If it's a single character, then just delete it.  Otherwise do nothing until the user triggers backspace again.
    auto text = TextDocument->Selection->Text;
    if (text->Length() == 1)
    {
        TextDocument->Selection->SetText(Windows::UI::Text::TextSetOptions::None, L"");
    }
}

void MathRichEditBox::SubmitEquation(EquationSubmissionSource source)
{
    // Clear formatting since the graph control doesn't work with bold/underlines
    auto range = this->TextDocument->GetRange(0, this->TextDocument->Selection->EndPosition);

    if (range != nullptr)
    {
        range->CharacterFormat->Underline = UnderlineType::None;
    }

    auto newVal = GetMathTextProperty();
    if (MathText != newVal)
    {
        // Request the final formatting of the text
        auto formatRequest = ref new MathRichEditBoxFormatRequest(newVal);
        FormatRequest(this, formatRequest);

        if (formatRequest->FormattedText != nullptr && !formatRequest->FormattedText->IsEmpty())
        {
            newVal = formatRequest->FormattedText;
        }

        SetValue(MathTextProperty, newVal);
        EquationSubmitted(this, ref new MathRichEditBoxSubmission(true, source));
    }
    else
    {
        EquationSubmitted(this, ref new MathRichEditBoxSubmission(false, source));
    }
}
