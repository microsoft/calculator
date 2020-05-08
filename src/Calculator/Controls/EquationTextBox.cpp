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
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Text;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Media;

DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, EquationColor);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, EquationButtonForegroundColor);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, ColorChooserFlyout);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, EquationButtonContentIndex);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, HasError);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, IsAddEquationMode);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, MathEquation);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, ErrorText);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationTextBox, IsEquationLineDisabled);

EquationTextBox::EquationTextBox()
{
}

void EquationTextBox::OnApplyTemplate()
{
    m_equationButton = dynamic_cast<ToggleButton ^>(GetTemplateChild("EquationButton"));
    m_richEditBox = dynamic_cast<MathRichEditBox ^>(GetTemplateChild("MathRichEditBox"));
    m_deleteButton = dynamic_cast<Button ^>(GetTemplateChild("DeleteButton"));
    m_removeButton = dynamic_cast<Button ^>(GetTemplateChild("RemoveButton"));
    m_functionButton = dynamic_cast<Button ^>(GetTemplateChild("FunctionButton"));
    m_colorChooserButton = dynamic_cast<ToggleButton ^>(GetTemplateChild("ColorChooserButton"));
    m_richEditContextMenu = dynamic_cast<MenuFlyout ^>(GetTemplateChild("MathRichEditContextMenu"));
    m_kgfEquationMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("FunctionAnalysisMenuItem"));
    m_removeMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("RemoveFunctionMenuItem"));
    m_colorChooserMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("ChangeFunctionStyleMenuItem"));
    m_cutMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("CutMenuItem"));
    m_copyMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("CopyMenuItem"));
    m_pasteMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("PasteMenuItem"));
    m_undoMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("UndoMenuItem"));
    m_selectAllMenuItem = dynamic_cast<MenuFlyoutItem ^>(GetTemplateChild("SelectAllMenuItem"));

    auto resProvider = AppResourceProvider::GetInstance();

    if (m_richEditBox != nullptr)
    {
        m_richEditBox->GotFocus += ref new RoutedEventHandler(this, &EquationTextBox::OnRichEditBoxGotFocus);
        m_richEditBox->LostFocus += ref new RoutedEventHandler(this, &EquationTextBox::OnRichEditBoxLostFocus);
        m_richEditBox->TextChanged += ref new RoutedEventHandler(this, &EquationTextBox::OnRichEditTextChanged);
        m_richEditBox->SelectionFlyout = nullptr;
        m_richEditBox->EquationSubmitted += ref new EventHandler<MathRichEditBoxSubmission ^>(this, &EquationTextBox::OnEquationSubmitted);
        m_richEditBox->FormatRequest += ref new EventHandler<MathRichEditBoxFormatRequest ^>(this, &EquationTextBox::OnEquationFormatRequested);
    }

    if (m_equationButton != nullptr)
    {
        m_equationButton->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnEquationButtonClicked);
    }

    if (m_richEditContextMenu != nullptr)
    {
        m_richEditContextMenu->Opened += ref new EventHandler<Platform::Object ^>(this, &EquationTextBox::OnRichEditMenuOpened);
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
        m_kgfEquationMenuItem->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnFunctionMenuButtonClicked);
    }

    if (ColorChooserFlyout != nullptr)
    {
        ColorChooserFlyout->Opened += ref new EventHandler<Object ^>(this, &EquationTextBox::OnColorFlyoutOpened);
        ColorChooserFlyout->Closed += ref new EventHandler<Object ^>(this, &EquationTextBox::OnColorFlyoutClosed);
    }

    if (m_cutMenuItem != nullptr)
    {
        m_cutMenuItem->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnCutClicked);
    }

    if (m_copyMenuItem != nullptr)
    {
        m_copyMenuItem->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnCopyClicked);
    }

    if (m_pasteMenuItem != nullptr)
    {
        m_pasteMenuItem->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnPasteClicked);
    }

    if (m_undoMenuItem != nullptr)
    {
        m_undoMenuItem->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnUndoClicked);
    }

    if (m_selectAllMenuItem != nullptr)
    {
        m_selectAllMenuItem->Click += ref new RoutedEventHandler(this, &EquationTextBox::OnSelectAllClicked);
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

void EquationTextBox::OnRichEditTextChanged(Object ^ sender, RoutedEventArgs ^ e)
{
    UpdateCommonVisualState();
    UpdateButtonsVisualState();
}

void EquationTextBox::OnRichEditBoxGotFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    m_HasFocus = true;
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
}

void EquationTextBox::OnDeleteButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->TextDocument->SetText(::TextSetOptions::None, "");
        if (m_functionButton)
        {
            m_functionButton->IsEnabled = false;
        }
    }
}

void EquationTextBox::OnEquationButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    EquationButtonClicked(this, ref new RoutedEventArgs());

    SetEquationButtonTooltipAndAutomationName();
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
        IsEquationLineDisabled = false;
    }

    TraceLogger::GetInstance()->LogGraphButtonClicked(GraphButton::RemoveFunction, GraphButtonValue::None);

    VisualStateManager::GoToState(this, "Normal", true);
}

void EquationTextBox::OnColorChooserButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (ColorChooserFlyout != nullptr && m_richEditBox != nullptr)
    {
        ColorChooserFlyout->ShowAt(m_richEditBox);
        TraceLogger::GetInstance()->LogGraphButtonClicked(GraphButton::StylePicker, GraphButtonValue::None);
    }
}

void EquationTextBox::OnFunctionMenuButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    // Submit the equation before trying to analyze it if invoked from context menu
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->SubmitEquation(::EquationSubmissionSource::FOCUS_LOST);
    }

    KeyGraphFeaturesButtonClicked(this, ref new RoutedEventArgs());
}

void EquationTextBox::OnFunctionButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    KeyGraphFeaturesButtonClicked(this, ref new RoutedEventArgs());
}

void EquationTextBox::OnCutClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->TextDocument->Selection->Cut();
    }
}

void EquationTextBox::OnCopyClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->TextDocument->Selection->Copy();
    }
}

void EquationTextBox::OnPasteClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->TextDocument->Selection->Paste(0);
    }
}

void EquationTextBox::OnSelectAllClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->TextDocument->Selection->SetRange(0, m_richEditBox->TextDocument->Selection->EndPosition);
    }
}

void EquationTextBox::OnUndoClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    if (m_richEditBox != nullptr)
    {
        m_richEditBox->TextDocument->Undo();
    }
}

void EquationTextBox::UpdateButtonsVisualState()
{
    String ^ state;

    if (m_HasFocus && RichEditHasContent())
    {
        state = "ButtonVisible";
    }
    else if (IsAddEquationMode)
    {
        state = "ButtonHideRemove";
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
    bool richEditHasContent = RichEditHasContent();

    if (m_HasFocus && HasError)
    {
        state = "FocusedError";
    }
    else if (IsAddEquationMode && m_HasFocus && !richEditHasContent)
    {
        state = "AddEquationFocused";
    }
    else if (m_HasFocus)
    {
        state = "Focused";
    }
    else if (IsAddEquationMode && m_isPointerOver && !richEditHasContent)
    {
        state = "AddEquation";
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
        m_richEditBox->TextDocument->GetText(Windows::UI::Text::TextGetOptions::NoHidden, &text);
    }
    return !text->IsEmpty();
}

void EquationTextBox::OnRichEditMenuOpened(Object ^ /*sender*/, Object ^ /*args*/)
{
    if (m_removeMenuItem != nullptr)
    {
        m_removeMenuItem->IsEnabled = !IsAddEquationMode;
    }

    if (m_kgfEquationMenuItem != nullptr)
    {
        m_kgfEquationMenuItem->IsEnabled = m_HasFocus && !HasError && RichEditHasContent();
    }

    if (m_colorChooserMenuItem != nullptr)
    {
        m_colorChooserMenuItem->IsEnabled = !HasError && !IsAddEquationMode;
    }

    if (m_richEditBox != nullptr && m_cutMenuItem != nullptr)
    {
        m_cutMenuItem->IsEnabled = m_richEditBox->TextDocument->CanCopy();
    }

    if (m_richEditBox != nullptr && m_copyMenuItem != nullptr)
    {
        m_copyMenuItem->IsEnabled = m_richEditBox->TextDocument->CanCopy();
    }

    if (m_richEditBox != nullptr && m_pasteMenuItem != nullptr)
    {
        m_pasteMenuItem->IsEnabled = m_richEditBox->TextDocument->CanPaste();
    }

    if (m_richEditBox != nullptr && m_undoMenuItem != nullptr)
    {
        m_undoMenuItem->IsEnabled = m_richEditBox->TextDocument->CanUndo();
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
        FocusManager::TryFocusAsync(m_richEditBox, ::FocusState::Programmatic);
    }
}

void EquationTextBox::OnEquationSubmitted(Platform::Object ^ sender, MathRichEditBoxSubmission ^ args)
{
    if (args->HasTextChanged)
    {
        if (m_functionButton && m_richEditBox->MathText != L"")
        {
            m_functionButton->IsEnabled = true;
        }
    }

    EquationSubmitted(this, args);
}

void EquationTextBox::OnEquationFormatRequested(Object ^ sender, MathRichEditBoxFormatRequest ^ args)
{
    EquationFormatRequested(this, args);
}

void EquationTextBox::SetEquationButtonTooltipAndAutomationName()
{
    auto toolTip = ref new ToolTip();
    auto resProvider = AppResourceProvider::GetInstance();

    auto equationButtonMessage = LocalizationStringUtil::GetLocalizedString(
        IsEquationLineDisabled ? resProvider->GetResourceString(L"showEquationButtonAutomationName")
                               : resProvider->GetResourceString(L"hideEquationButtonAutomationName"),
        EquationButtonContentIndex);

    auto equationButtonTooltip = LocalizationStringUtil::GetLocalizedString(
        IsEquationLineDisabled ? resProvider->GetResourceString(L"showEquationButtonToolTip") : resProvider->GetResourceString(L"hideEquationButtonToolTip"));

    toolTip->Content = equationButtonTooltip;
    ToolTipService::SetToolTip(m_equationButton, toolTip);
    AutomationProperties::SetName(m_equationButton, equationButtonMessage);
}

void EquationTextBox::OnEquationButtonContentIndexPropertyChanged(String ^ /*oldValue*/, String ^ newValue)
{
    SetEquationButtonTooltipAndAutomationName();
}
