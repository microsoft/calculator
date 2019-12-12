// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "CalcViewModel/Common/LocalizationStringUtil.h"
#include "EquationTextBox.h"

using namespace std;
using namespace Platform;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Text;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Controls::Primitives;

DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, EquationColor);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, ColorChooserFlyout);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, EquationButtonContentIndex);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, HasError);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, IsAddEquationMode);

EquationTextBox::EquationTextBox()
{
}

void EquationTextBox::OnApplyTemplate()
{
    m_equationButton = dynamic_cast<ToggleButton ^>(GetTemplateChild("EquationButton"));
    m_kgfEquationButton = dynamic_cast<Button ^>(GetTemplateChild("KGFEquationButton"));
    m_richEditBox = dynamic_cast<MathRichEditBox ^>(GetTemplateChild("MathRichEditBox"));
    m_deleteButton = dynamic_cast<Button ^>(GetTemplateChild("DeleteButton"));
    m_removeButton = dynamic_cast<Button ^>(GetTemplateChild("RemoveButton"));
    m_functionButton = dynamic_cast<Button ^>(GetTemplateChild("FunctionButton"));
    m_colorChooserButton = dynamic_cast<ToggleButton ^>(GetTemplateChild("ColorChooserButton"));
    m_richEditContextMenu = dynamic_cast<MenuFlyout ^>(GetTemplateChild("MathRichEditContextMenu"));
    m_kgfEquationMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("FunctionAnalysisMenuItem"));
    m_removeMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("RemoveFunctionMenuItem"));
    m_colorChooserMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("ChangeFunctionStyleMenuItem"));

    auto resProvider = AppResourceProvider::GetInstance();

    if (m_richEditBox != nullptr)
    {
        m_richEditBox->GotFocus += ref new RoutedEventHandler(this, &EquationTextBox::OnRichEditBoxGotFocus);
        m_richEditBox->LostFocus += ref new RoutedEventHandler(this, &EquationTextBox::OnRichEditBoxLostFocus);
        m_richEditBox->TextChanged += ref new RoutedEventHandler(this, &EquationTextBox::OnRichEditBoxTextChanged);
        m_richEditBox->SelectionFlyout = nullptr;
    }

    if (m_equationButton != nullptr)
    {
        m_equationButton->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnEquationButtonClicked);

        auto toolTip = ref new ToolTip();
        auto equationButtonMessage = m_equationButton->IsChecked->Value ? resProvider->GetResourceString(L"showEquationButtonToolTip")
                                                                        : resProvider->GetResourceString(L"hideEquationButtonToolTip");
        toolTip->Content = equationButtonMessage;
        ToolTipService::SetToolTip(m_equationButton, toolTip);
        AutomationProperties::SetName(m_equationButton, equationButtonMessage);
    }

    if (m_richEditContextMenu != nullptr)
    {
        m_richEditContextMenu->Opening += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &EquationTextBox::OnRichEditMenuOpening);
    }

    if (m_kgfEquationButton != nullptr)
    {
        m_kgfEquationButton->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnKGFEquationButtonClicked);
    }

    if (m_deleteButton != nullptr)
    {
        m_deleteButton->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnDeleteButtonClicked);
    }

    if (m_removeButton != nullptr)
    {
        m_removeButton->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnRemoveButtonClicked);
    }

    if (m_removeMenuItem != nullptr)
    {
        m_removeMenuItem->Text = resProvider->GetResourceString(L"removeMenuItem");
        m_removeMenuItem->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnRemoveButtonClicked);
    }

    if (m_colorChooserButton != nullptr)
    {
        m_colorChooserButton->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnColorChooserButtonClicked);
    }

    if (m_colorChooserMenuItem != nullptr)
    {
        m_colorChooserMenuItem->Text = resProvider->GetResourceString(L"colorChooserMenuItem");
        m_colorChooserMenuItem->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnColorChooserButtonClicked);
    }

    if (m_functionButton != nullptr)
    {
        m_functionButton->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnFunctionButtonClicked);
        m_functionButton->IsEnabled = false;
    }

    if (m_kgfEquationMenuItem != nullptr)
    {
        m_kgfEquationMenuItem->Text = resProvider->GetResourceString(L"functionAnalysisMenuItem");
        m_kgfEquationMenuItem->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnFunctionButtonClicked);
    }

    if (ColorChooserFlyout != nullptr)
    {
        ColorChooserFlyout->Opened += ref new EventHandler<Object ^>(this, &EquationTextBox::OnColorFlyoutOpened);
        ColorChooserFlyout->Closed += ref new EventHandler<Object ^>(this, &EquationTextBox::OnColorFlyoutClosed);
    }

    UpdateCommonVisualState();
    UpdateButtonsVisualState();
}

void EquationTextBox::OnPointerEntered(PointerRoutedEventArgs ^ e)
{
    m_isPointerOver = true;
    UpdateCommonVisualState();
}

void EquationTextBox::OnPointerExited(PointerRoutedEventArgs ^ e)
{
    m_isPointerOver = false;
    UpdateCommonVisualState();
}

void EquationTextBox::OnPointerCanceled(PointerRoutedEventArgs ^ e)
{
    m_isPointerOver = false;
    UpdateCommonVisualState();
}

void EquationTextBox::OnPointerCaptureLost(PointerRoutedEventArgs ^ e)
{
    m_isPointerOver = false;
    UpdateCommonVisualState();
}

void EquationTextBox::OnKeyDown(KeyRoutedEventArgs ^ e)
{
    if (e->Key == VirtualKey::Enter)
    {
        m_sourceSubmission = EquationSubmissionSource::ENTER_KEY;
        // We will rely on OnLostFocus to submit the equation to prevent the launch of 2 events
        if (!m_HasFocus || !FocusManager::TryMoveFocusAsync(::FocusNavigationDirection::Next))
        {
            m_sourceSubmission = EquationSubmissionSource::FOCUS_LOST;
            EquationSubmitted(this, EquationSubmissionSource::ENTER_KEY);
            if (m_functionButton && m_richEditBox->MathText != L"")
            {
                m_functionButton->IsEnabled = true;
            }
        }
    }
}

void EquationTextBox::OnLostFocus(RoutedEventArgs ^ e)
{
    if (m_richEditBox == nullptr || m_richEditBox->ContextFlyout->IsOpen)
    {
        return;
    }

    if (m_functionButton && m_richEditBox->MathText != L"")
    {
        m_functionButton->IsEnabled = true;
    }
}

void EquationTextBox::OnColorFlyoutOpened(Object ^ sender, Object ^ e)
{
    m_isColorChooserFlyoutOpen = true;
    UpdateCommonVisualState();
}

void EquationTextBox::OnColorFlyoutClosed(Object ^ sender, Object ^ e)
{
    m_colorChooserButton->IsChecked = false;
    m_isColorChooserFlyoutOpen = false;
    UpdateCommonVisualState();
}

void EquationTextBox::OnRichEditBoxTextChanged(Object ^ sender, RoutedEventArgs ^ e)
{
    UpdateButtonsVisualState();
}

void EquationTextBox::OnRichEditBoxGotFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    m_HasFocus = true;
    m_sourceSubmission = EquationSubmissionSource::FOCUS_LOST;
    UpdateCommonVisualState();
    UpdateButtonsVisualState();
}

void EquationTextBox::OnRichEditBoxLostFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    if (!m_richEditBox->ContextFlyout->IsOpen)
    {
        m_HasFocus = false;
    }

    UpdateCommonVisualState();
    UpdateButtonsVisualState();

    EquationSubmitted(this, m_sourceSubmission);
}

void EquationTextBox::OnDeleteButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->MathText = L"";
        if (m_functionButton)
        {
            m_functionButton->IsEnabled = false;
        }
    }
}

void EquationTextBox::OnEquationButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    EquationButtonClicked(this, ref new RoutedEventArgs());

    auto toolTip = ref new ToolTip();
    auto resProvider = AppResourceProvider::GetInstance();
    auto equationButtonMessage = m_equationButton->IsChecked->Value ? resProvider->GetResourceString(L"showEquationButtonToolTip")
                                                                    : resProvider->GetResourceString(L"hideEquationButtonToolTip");
    toolTip->Content = equationButtonMessage;
    ToolTipService::SetToolTip(m_equationButton, toolTip);
    AutomationProperties::SetName(m_equationButton, equationButtonMessage);
}

void EquationTextBox::OnKGFEquationButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    EquationButtonClicked(this, ref new RoutedEventArgs());
}

void EquationTextBox::OnRemoveButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (IsAddEquationMode)
    {
        // Don't remove the last equation
        return;
    }

    if (m_richEditBox != nullptr)
    {
        m_richEditBox->MathText = L"";
    }

    RemoveButtonClicked(this, ref new RoutedEventArgs());

    if (m_functionButton)
    {
        m_functionButton->IsEnabled = false;
    }

    if (m_equationButton)
    {
        m_equationButton->IsChecked = false;
    }

    VisualStateManager::GoToState(this, "Normal", true);
}

void EquationTextBox::OnColorChooserButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (ColorChooserFlyout != nullptr && m_richEditBox != nullptr)
    {
        ColorChooserFlyout->ShowAt(m_richEditBox);
    }
}

void EquationTextBox::OnFunctionButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    KeyGraphFeaturesButtonClicked(this, ref new RoutedEventArgs());
}

void EquationTextBox::UpdateButtonsVisualState()
{
    String ^ state;

    if (IsAddEquationMode)
    {
        state = "ButtonHideRemove";
    }
    else if (RichEditHasContent())
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
    String ^ state = nullptr;

    if (m_HasFocus && HasError)
    {
        state = "FocusedError";
    }
    else if (m_HasFocus)
    {
        state = "Focused";
    }
    else if (HasError && (m_isPointerOver || m_isColorChooserFlyoutOpen))
    {
        state = "PointerOverError";
    }
    else if (m_isPointerOver || m_isColorChooserFlyoutOpen)
    {
        state = "PointerOver";
    }
    else if (HasError)
    {
        state = "Error";
    }
    else if (IsAddEquationMode)
    {
        state = "AddEquation";
    }
    else
    {
        state = "Normal";
    }
    VisualStateManager::GoToState(this, state, false);
}

void EquationTextBox::OnHasErrorPropertyChanged(bool, bool)
{
    UpdateCommonVisualState();
}

Platform::String ^ EquationTextBox::GetEquationText()
{
    String ^ text;
    if (m_richEditBox != nullptr)
    {
        // Clear formatting since the graph control doesn't work with bold/underlines
        ITextRange ^ range = m_richEditBox->TextDocument->GetRange(0, m_richEditBox->TextDocument->Selection->EndPosition);

        if (range != nullptr)
        {
            range->CharacterFormat->Bold = FormatEffect::Off;
            range->CharacterFormat->Underline = UnderlineType::None;
        }

        text = m_richEditBox->MathText;
    }

    return text;
}

void EquationTextBox::SetEquationText(Platform::String ^ equationText)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->MathText = equationText;
    }
}

bool EquationTextBox::RichEditHasContent()
{
    String ^ text;

    if (m_richEditBox != nullptr)
    {
        text = m_richEditBox->MathText;
    }
    return (!text->IsEmpty() && m_HasFocus);
}

void EquationTextBox::OnRichEditMenuOpening(Object ^ /*sender*/, Object ^ /*args*/)
{
    if (m_kgfEquationMenuItem != nullptr)
    {
        m_kgfEquationMenuItem->IsEnabled = RichEditHasContent();
    }

    if (m_colorChooserMenuItem != nullptr)
    {
        m_colorChooserMenuItem->IsEnabled = !HasError;
    }
}

void EquationTextBox::OnIsAddEquationModePropertyChanged(bool /*oldValue*/, bool /*newValue*/)
{
    UpdateCommonVisualState();
    UpdateButtonsVisualState();
}

void EquationTextBox::FocusTextBox()
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->Focus(::FocusState::Programmatic);
    }
}
