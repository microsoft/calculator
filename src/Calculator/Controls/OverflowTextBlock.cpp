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
DEPENDENCY_PROPERTY_INITIALIZATION(OverflowTextBlock, ScrollButtonsWidth);
DEPENDENCY_PROPERTY_INITIALIZATION(OverflowTextBlock, ScrollButtonsFontSize);
DEPENDENCY_PROPERTY_INITIALIZATION(OverflowTextBlock, ScrollButtonsPlacement);

static constexpr unsigned int SCROLL_BUTTONS_APPROXIMATION_RANGE = 4;

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

    uiElement = GetTemplateChild("ExpressionContent");
    if (uiElement != nullptr)
    {
        m_expressionContent = safe_cast<FrameworkElement ^>(uiElement);
    }

    uiElement = GetTemplateChild("ScrollLeft");
    if (uiElement != nullptr)
    {
        m_scrollLeft = safe_cast<Button ^>(uiElement);
        m_scrollLeftClickEventToken = m_scrollLeft->Click += ref new RoutedEventHandler(this, &OverflowTextBlock::OnScrollLeftClick);
    }

    uiElement = GetTemplateChild("ScrollRight");
    if (uiElement != nullptr)
    {
        m_scrollRight = safe_cast<Button ^>(uiElement);
        m_scrollRightClickEventToken = m_scrollRight->Click += ref new RoutedEventHandler(this, &OverflowTextBlock::OnScrollRightClick);
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
    auto realOffset = m_expressionContainer->HorizontalOffset + m_expressionContainer->Padding.Left + m_expressionContent->Margin.Left;
    if (m_expressionContainer != nullptr && realOffset + m_expressionContainer->ActualWidth < m_expressionContent->ActualWidth)
    {
        m_scrollingRight = true;
        double offset = m_expressionContainer->HorizontalOffset + (scrollRatio * m_expressionContainer->ViewportWidth);
        m_expressionContainer->ChangeView(offset, nullptr, nullptr);
        m_expressionContainer->UpdateLayout();
        UpdateScrollButtons();
    }
}

void OverflowTextBlock::OnScrollLeftClick(_In_ Object ^ sender, _In_ RoutedEventArgs ^)
{
    ScrollLeft();
}

void OverflowTextBlock::OnScrollRightClick(_In_ Object ^ sender, _In_ RoutedEventArgs ^)
{
    ScrollRight();
}

void OverflowTextBlock::UpdateScrollButtons()
{
    if (m_expressionContainer == nullptr || m_scrollLeft == nullptr || m_scrollRight == nullptr)
    {
        return;
    }

    auto realOffset = m_expressionContainer->HorizontalOffset + m_expressionContainer->Padding.Left + m_expressionContent->Margin.Left;
    m_scrollLeft->Visibility = realOffset > SCROLL_BUTTONS_APPROXIMATION_RANGE ? ::Visibility::Visible : ::Visibility::Collapsed;
    m_scrollRight->Visibility = realOffset + m_expressionContainer->ActualWidth + SCROLL_BUTTONS_APPROXIMATION_RANGE < m_expressionContent->ActualWidth
                                    ? ::Visibility::Visible
                                    : ::Visibility::Collapsed;

    if (ScrollButtonsPlacement == OverflowButtonPlacement::Above && m_expressionContainer != nullptr && m_expressionContent != nullptr)
    {
        double left = m_scrollLeft != nullptr && m_scrollLeft->Visibility == ::Visibility::Visible ? ScrollButtonsWidth : 0;
        double right = m_scrollRight != nullptr && m_scrollRight->Visibility == ::Visibility::Visible ? ScrollButtonsWidth : 0;
        if (m_expressionContainer->Padding.Left != left || m_expressionContainer->Padding.Right != right)
        {
            m_expressionContainer->Padding = Thickness(left, 0, right, 0);
            m_expressionContent->Margin = Thickness(-left, 0, -right, 0);
            //   m_ignoreNextViewChangedEvent = true;
        }
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

void OverflowTextBlock::OnScrollButtonsPlacementPropertyChanged(OverflowButtonPlacement /*oldValue*/, OverflowButtonPlacement newValue)
{
    if (newValue == OverflowButtonPlacement::InLine)
    {
        m_expressionContainer->Padding = Thickness(0);
        m_expressionContent->Margin = Thickness(0);
    }
    UpdateScrollButtons();
}
