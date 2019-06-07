// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// HorizontalNoOverflowStackPanel.xaml.cpp
// Implementation of the HorizontalNoOverflowStackPanel class
//

#include "pch.h"
#include "HorizontalNoOverflowStackPanel.h"

using namespace std;
using namespace CalculatorApp::Controls;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;

Size HorizontalNoOverflowStackPanel::MeasureOverride(Size availableSize)
{
    float maxHeight = 0;
    float width = 0;
    for (auto child : Children)
    {
        child->Measure(Size(numeric_limits<float>::infinity(), numeric_limits<float>::infinity()));
        maxHeight = max(maxHeight, child->DesiredSize.Height);
        width += child->DesiredSize.Width;
    }
    return Size(min(width, availableSize.Width), min(availableSize.Height, maxHeight));
}

bool HorizontalNoOverflowStackPanel::ShouldPrioritizeLastItem()
{
    return false;
}

Size HorizontalNoOverflowStackPanel::ArrangeOverride(Size finalSize)
{
    if (Children->Size == 0)
    {
        return finalSize;
    }

    float posX = 0;
    auto lastChild = Children->GetAt(Children->Size - 1);
    float lastChildWidth = 0;
    if (Children->Size > 2 && ShouldPrioritizeLastItem())
    {
        lastChildWidth = lastChild->DesiredSize.Width;
    }
    for (auto item : Children)
    {
        auto widthAvailable = finalSize.Width - posX;
        if (item != lastChild)
        {
            widthAvailable -= lastChildWidth;
        }
        float itemWidth = item->DesiredSize.Width;
        if (widthAvailable > 0 && itemWidth <= widthAvailable)
        {
            // stack the items horizontally (left to right)
            item->Arrange(Rect(posX, 0, itemWidth, finalSize.Height));
            AutomationProperties::SetAccessibilityView(item, ::AccessibilityView::Content);
            posX += item->RenderSize.Width;
        }
        else
        {
            // Not display the item
            item->Arrange(Rect(0, 0, 0, 0));
            AutomationProperties::SetAccessibilityView(item, ::AccessibilityView::Raw);
        }
    }
    return finalSize;
}
