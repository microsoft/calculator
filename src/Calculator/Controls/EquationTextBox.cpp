// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EquationTextBox.h"

using namespace Platform;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;


EquationTextBox::EquationTextBox()
{
    m_buttonCursor = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Hand, 0);
}

void EquationTextBox::OnApplyTemplate()
{
    TextBox::OnApplyTemplate();

    m_removeButton = dynamic_cast<Button^>(GetTemplateChild("RemoveButton"));
    m_colorChooserButton = dynamic_cast<Button^>(GetTemplateChild("ColorChooserButton"));
    m_functionButton = dynamic_cast<Button^>(GetTemplateChild("FunctionButton"));

    if (m_removeButton != nullptr)
    {
        m_removeButton->PointerEntered += ref new PointerEventHandler(this, &EquationTextBox::OnPointerEnteredButton);
    }

    if (m_colorChooserButton != nullptr)
    {
        m_colorChooserButton->PointerEntered += ref new PointerEventHandler(this, &EquationTextBox::OnPointerEnteredButton);
    }

    if (m_functionButton != nullptr)
    {
        m_functionButton->PointerEntered += ref new PointerEventHandler(this, &EquationTextBox::OnPointerEnteredButton);
    }
}



void EquationTextBox::OnPointerEnteredButton(_In_ Object^ sender, _In_ PointerRoutedEventArgs^ e)
{
    m_cursorBeforePointerEntered = Window::Current->CoreWindow->PointerCursor;
    Window::Current->CoreWindow->PointerCursor = m_buttonCursor;
}

void EquationTextBox::OnPointerExitedButton(_In_ Object^ sender, _In_ PointerRoutedEventArgs^ e)
{

}
