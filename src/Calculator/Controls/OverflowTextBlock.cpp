// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "OverflowTextBlock.h"
#include "OverflowTextBlockAutomationPeer.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;

using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

DEPENDENCY_PROPERTY_INITIALIZATION(OverflowTextBlock, IsActive);
DEPENDENCY_PROPERTY_INITIALIZATION(OverflowTextBlock, TextStyle);
DEPENDENCY_PROPERTY_INITIALIZATION(OverflowTextBlock, TokensUpdated);

void OverflowTextBlock::OnApplyTemplate()
{
    UnregisterEventHandlers();

    auto uiElement = GetTemplateChild("ExpressionContainer");
    if (uiElement != nullptr)
    {
        m_expressionContainer = safe_cast<ScrollViewer ^>(uiElement);
        m_expressionContainer->ChangeView(m_expressionContainer->ExtentWidth - m_expressionContainer->ViewportWidth, nullptr, nullptr);
        m_containerViewChangedToken = m_expressionContainer->ViewChanged +=
            ref new EventHandler<ScrollViewerViewChangedEventArgs ^>(this, &OverflowTextBlock::OnViewChanged);
    }

    uiElement = GetTemplateChild("ScrollLeft");
    if (uiElement != nullptr)
    {
        m_scrollLeft = safe_cast<Button ^>(uiElement);
        m_scrollLeftClickEventToken = m_scrollLeft->Click += ref new RoutedEventHandler(this, &OverflowTextBlock::OnScrollClick);
    }

    uiElement = GetTemplateChild("ScrollRight");
    if (uiElement != nullptr)
    {
        m_scrollRight = safe_cast<Button ^>(uiElement);
        m_scrollRightClickEventToken = m_scrollRight->Click += ref new RoutedEventHandler(this, &OverflowTextBlock::OnScrollClick);
    }

    m_scrollingLeft = false;
    m_scrollingRight = false;

    uiElement = GetTemplateChild("TokenList");
    if (uiElement != nullptr)
    {
        m_itemsControl = safe_cast<ItemsControl ^>(uiElement);
    }

    UpdateAllState();
}

AutomationPeer ^ OverflowTextBlock::OnCreateAutomationPeer()
{
    return ref new OverflowTextBlockAutomationPeer(this);
}

void OverflowTextBlock::OnTokensUpdatedPropertyChanged(bool /*oldValue*/, bool newValue)
{
    if (m_expressionContainer != nullptr && newValue)
    {
        m_expressionContainer->UpdateLayout();
        m_expressionContainer->ChangeView(m_expressionContainer->ScrollableWidth, nullptr, nullptr, true);
    }
    auto newIsAccessibilityViewControl = m_itemsControl != nullptr && m_itemsControl->Items->Size > 0;
    if (m_isAccessibilityViewControl != newIsAccessibilityViewControl)
    {
        m_isAccessibilityViewControl = newIsAccessibilityViewControl;
        AutomationProperties::SetAccessibilityView(this, newIsAccessibilityViewControl ? AccessibilityView::Control : AccessibilityView::Raw);
    }
    UpdateScrollButtons();
}

void OverflowTextBlock::UpdateAllState()
{
    UpdateVisualState();
}

void OverflowTextBlock::UpdateVisualState()
{
    if (IsActive)
    {
        VisualStateManager::GoToState(this, "Active", true);
    }
    else
    {
        VisualStateManager::GoToState(this, "Normal", true);
    }
}

void OverflowTextBlock::ScrollLeft()
{
    if (m_expressionContainer != nullptr && m_expressionContainer->HorizontalOffset > 0)
    {
        m_scrollingLeft = true;
        double offset = m_expressionContainer->HorizontalOffset - (scrollRatio * m_expressionContainer->ViewportWidth);
        m_expressionContainer->ChangeView(offset, nullptr, nullptr);
        m_expressionContainer->UpdateLayout();
        UpdateScrollButtons();
    }
}

void OverflowTextBlock::ScrollRight()
{
    if (m_expressionContainer != nullptr && m_expressionContainer->HorizontalOffset < m_expressionContainer->ExtentWidth - m_expressionContainer->ViewportWidth)
    {
        m_scrollingRight = true;
        double offset = m_expressionContainer->HorizontalOffset + (scrollRatio * m_expressionContainer->ViewportWidth);
        m_expressionContainer->ChangeView(offset, nullptr, nullptr);
        m_expressionContainer->UpdateLayout();
        UpdateScrollButtons();
    }
}

void OverflowTextBlock::OnScrollClick(_In_ Object ^ sender, _In_ RoutedEventArgs ^)
{
    auto clicked = safe_cast<Button ^>(sender);
    if (clicked == m_scrollLeft)
    {
        ScrollLeft();
    }
    else
    {
        ScrollRight();
    }
}

void OverflowTextBlock::UpdateScrollButtons()
{
    if (m_itemsControl == nullptr || m_expressionContainer == nullptr)
    {
        return;
    }

    // When the width is smaller than the container, don't show any
    if (m_itemsControl->ActualWidth <= m_expressionContainer->ActualWidth)
    {
        ShowHideScrollButtons(::Visibility::Collapsed, ::Visibility::Collapsed);
    }
    // We have more number on both side. Show both arrows
    else if (
        (m_expressionContainer->HorizontalOffset > 0)
        && (m_expressionContainer->HorizontalOffset < (m_expressionContainer->ExtentWidth - m_expressionContainer->ViewportWidth)))
    {
        ShowHideScrollButtons(::Visibility::Visible, ::Visibility::Visible);
    }
    // Width is larger than the container and left most part of the number is shown. Should be able to scroll left.
    else if (m_expressionContainer->HorizontalOffset == 0)
    {
        ShowHideScrollButtons(::Visibility::Collapsed, ::Visibility::Visible);
        if (m_scrollingLeft)
        {
            m_scrollingLeft = false;
            if (m_scrollRight != nullptr)
            {
                m_scrollRight->Focus(::FocusState::Programmatic);
            }
        }
    }
    else // Width is larger than the container and right most part of the number is shown. Should be able to scroll left.
    {
        ShowHideScrollButtons(::Visibility::Visible, ::Visibility::Collapsed);
        if (m_scrollingRight)
        {
            m_scrollingRight = false;
            if (m_scrollLeft != nullptr)
            {
                m_scrollLeft->Focus(::FocusState::Programmatic);
            }
        }
    }
}

void OverflowTextBlock::ShowHideScrollButtons(::Visibility vLeft, ::Visibility vRight)
{
    if (m_scrollLeft != nullptr && m_scrollRight != nullptr)
    {
        m_scrollLeft->Visibility = vLeft;
        m_scrollRight->Visibility = vRight;
    }
}

void OverflowTextBlock::UnregisterEventHandlers()
{
    // Unregister the event handlers
    if (m_scrollLeft != nullptr)
    {
        m_scrollLeft->Click -= m_scrollLeftClickEventToken;
    }

    if (m_scrollRight != nullptr)
    {
        m_scrollRight->Click -= m_scrollRightClickEventToken;
    }

    if (m_expressionContainer != nullptr)
    {
        m_expressionContainer->ViewChanged -= m_containerViewChangedToken;
    }
}

void OverflowTextBlock::OnViewChanged(_In_opt_ Object ^ /*sender*/, _In_opt_ ScrollViewerViewChangedEventArgs ^ /*args*/)
{
    UpdateScrollButtons();
}
