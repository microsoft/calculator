// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EquationTextBox.h"

using namespace std;
using namespace Platform;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Text;

DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, EquationColor);

void EquationTextBox::OnApplyTemplate()
{
    m_equationButton = dynamic_cast<Button^>(GetTemplateChild("EquationButton"));
    m_richEditBox = dynamic_cast<RichEditBox^>(GetTemplateChild("EquationTextBox"));
    m_deleteButton = dynamic_cast<Button^>(GetTemplateChild("DeleteButton"));
    m_removeButton = dynamic_cast<Button^>(GetTemplateChild("RemoveButton"));
    m_colorChooserButton = dynamic_cast<Button^>(GetTemplateChild("ColorChooserButton"));
    m_functionButton = dynamic_cast<Button^>(GetTemplateChild("FunctionButton"));

    if (m_richEditBox != nullptr)
    {
        m_richEditBox->GotFocus += ref new RoutedEventHandler(this, &EquationTextBox::OnRichEditBoxGotFocus);
        m_richEditBox->LostFocus += ref new RoutedEventHandler(this, &EquationTextBox::OnRichEditBoxLostFocus);
        m_richEditBox->TextChanged += ref new RoutedEventHandler(this, &EquationTextBox::OnRichEditBoxTextChanged);
    }

    if (m_deleteButton != nullptr)
    {
        m_deleteButton->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnDeleteButtonClicked);
    }

    if (m_removeButton != nullptr)
    {

    }

    if (m_colorChooserButton != nullptr)
    {

    }

    if (m_functionButton != nullptr)
    {

    }
}

void EquationTextBox::OnPointerEntered(PointerRoutedEventArgs^ e)
{
    m_isPointerOver = true;
    UpdateCommonVisualState();
}

void EquationTextBox::OnPointerExited(PointerRoutedEventArgs^ e)
{
    m_isPointerOver = false;
    UpdateCommonVisualState();
}

void EquationTextBox::OnPointerCanceled(PointerRoutedEventArgs^ e)
{
    m_isPointerOver = false;
    UpdateCommonVisualState();
}

void EquationTextBox::OnPointerCaptureLost(PointerRoutedEventArgs^ e)
{
    m_isPointerOver = false;
    UpdateCommonVisualState();
}

void EquationTextBox::OnRichEditBoxTextChanged(Object^ sender, RoutedEventArgs^ e)
{
    UpdateDeleteButtonVisualState();
}

void EquationTextBox::OnRichEditBoxGotFocus(Object^ sender, RoutedEventArgs^ e)
{
    m_isFocused = true;
    UpdateCommonVisualState();
    UpdateDeleteButtonVisualState();
}

void EquationTextBox::OnRichEditBoxLostFocus(Object^ sender, RoutedEventArgs^ e)
{
    m_isFocused = false;
    UpdateCommonVisualState();
    UpdateDeleteButtonVisualState();
}

void EquationTextBox::OnDeleteButtonClicked(Object^ sender, RoutedEventArgs^ e)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->TextDocument->SetText(::TextSetOptions::None, L"");
    }
}

void EquationTextBox::UpdateDeleteButtonVisualState()
{
    String^ state;

    if (ShouldDeleteButtonBeVisible())
    {
        state = "ButtonVisible";
    }
    else
    {
        state = "ButtonCollapsed";
    }

    VisualStateManager::GoToState(this, state, true);
}

void EquationTextBox::UpdateCommonVisualState()
{
    String^ state = "Normal";

    if (m_isFocused)
    {
        state = "Focused";
    }
    else if (m_isPointerOver)
    {
        state = "PointerOver";
    }

    VisualStateManager::GoToState(this, state, true);
}


Platform::String^ EquationTextBox::GetEquationText()
{
    String^ text;

    if (m_richEditBox != nullptr)
    {
        m_richEditBox->TextDocument->GetText(::TextGetOptions::NoHidden, &text);
    }

    return text;
}

void EquationTextBox::SetEquationText(Platform::String^ equationText)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->TextDocument->SetText(::TextSetOptions::None, equationText);
    }
}


bool EquationTextBox::ShouldDeleteButtonBeVisible()
{
    return (!GetEquationText()->IsEmpty() && m_isFocused);

}
