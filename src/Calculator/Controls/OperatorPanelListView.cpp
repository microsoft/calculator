// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "OperatorPanelListView.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Controls;

using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Devices::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Controls;

OperatorPanelListView::OperatorPanelListView()
{
}

void OperatorPanelListView::OnApplyTemplate()
{
    m_scrollViewer = dynamic_cast<ScrollViewer^>(GetTemplateChild("ScrollViewer"));
    m_scrollLeft = dynamic_cast<Button^>(GetTemplateChild("ScrollLeft"));
    m_scrollRight = dynamic_cast<Button^>(GetTemplateChild("ScrollRight"));
    m_content = dynamic_cast<ItemsPresenter^>(GetTemplateChild("Content"));

    if (m_scrollLeft != nullptr)
    {
        m_scrollLeft->Click += ref new RoutedEventHandler(this, &OperatorPanelListView::OnScrollClick);
        m_scrollLeft->PointerExited += ref new PointerEventHandler(this, &OperatorPanelListView::OnButtonPointerExited);
    }

    if (m_scrollRight != nullptr)
    {
        m_scrollRight->Click += ref new RoutedEventHandler(this, &OperatorPanelListView::OnScrollClick);
        m_scrollRight->PointerExited += ref new PointerEventHandler(this, &OperatorPanelListView::OnButtonPointerExited);
    }

    if (m_scrollViewer != nullptr)
    {
        m_scrollViewer->ViewChanged += ref new EventHandler<ScrollViewerViewChangedEventArgs^>(this, &OperatorPanelListView::ScrollViewChanged);
    }

    this->PointerEntered += ref new PointerEventHandler(this, &OperatorPanelListView::OnPointerEntered);
    this->PointerExited += ref new PointerEventHandler(this, &OperatorPanelListView::OnPointerExited);

    ListView::OnApplyTemplate();
}

void OperatorPanelListView::ScrollViewChanged(_In_ Object^, _In_ ScrollViewerViewChangedEventArgs^ e)
{
    if (m_isPointerEntered && !e->IsIntermediate)
    {
        UpdateScrollButtons();
    }
}

void OperatorPanelListView::OnScrollClick(_In_ Object^ sender, _In_ RoutedEventArgs^)
{
    auto clicked = dynamic_cast<Button^>(sender);
    if (clicked == m_scrollLeft)
    {
        ScrollLeft();
    }
    else
    {
        ScrollRight();
    }
}

void OperatorPanelListView::OnButtonPointerExited(_In_ Object^ sender, _In_ PointerRoutedEventArgs^ e)
{
    auto button = dynamic_cast<Button^>(sender);

    // Do not bubble up the pointer exit event to the control if the button being exited was not visible
    if (button->Visibility == ::Visibility::Collapsed)
    {
        e->Handled = true;
    }
}

void OperatorPanelListView::OnPointerEntered(_In_ Object^ sender, _In_ PointerRoutedEventArgs^ e)
{
    if (e->Pointer->PointerDeviceType == PointerDeviceType::Mouse)
    {
        UpdateScrollButtons();
        m_isPointerEntered = true;
    }
}

void OperatorPanelListView::OnPointerExited(_In_ Object^ sender, _In_ PointerRoutedEventArgs^ e)
{
    m_scrollLeft->Visibility = ::Visibility::Collapsed;
    m_scrollRight->Visibility = ::Visibility::Collapsed;
    m_isPointerEntered = false;
}

void OperatorPanelListView::ScrollLeft()
{
    double offset = m_scrollViewer->HorizontalOffset - (scrollRatio * m_scrollViewer->ViewportWidth);
    m_scrollViewer->ChangeView(offset, nullptr, nullptr);
}

void OperatorPanelListView::ScrollRight()
{
    double offset = m_scrollViewer->HorizontalOffset + (scrollRatio * m_scrollViewer->ViewportWidth);
    m_scrollViewer->ChangeView(offset, nullptr, nullptr);
}

void OperatorPanelListView::UpdateScrollButtons()
{
    if (m_content == nullptr || m_scrollViewer == nullptr)
    {
        return;
    }

    // When the width is smaller than the container, don't show any
    if (m_content->ActualWidth <= m_scrollViewer->ActualWidth)
    {
        ShowHideScrollButtons(::Visibility::Collapsed, ::Visibility::Collapsed);
    }
    // We have more number on both side. Show both arrows
    else if ((m_scrollViewer->HorizontalOffset > 0) && (m_scrollViewer->HorizontalOffset < (m_scrollViewer->ExtentWidth - m_scrollViewer->ViewportWidth)))
    {
        ShowHideScrollButtons(::Visibility::Visible, ::Visibility::Visible);
    }
    // Width is larger than the container and left most part of the number is shown. Should be able to scroll left.
    else if (m_scrollViewer->HorizontalOffset == 0)
    {
        ShowHideScrollButtons(::Visibility::Collapsed, ::Visibility::Visible);
    }
    else // Width is larger than the container and right most part of the number is shown. Should be able to scroll left.
    {
        ShowHideScrollButtons(::Visibility::Visible, ::Visibility::Collapsed);
    }
}

void OperatorPanelListView::ShowHideScrollButtons(::Visibility vLeft, ::Visibility vRight)
{
    if (m_scrollLeft != nullptr && m_scrollRight != nullptr)
    {
        m_scrollLeft->Visibility = vLeft;
        m_scrollRight->Visibility = vRight;
    }
}
