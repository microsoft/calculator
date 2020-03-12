// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "VisualTree.h"

using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Platform;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Calculator::Utils;

FrameworkElement ^ VisualTree::FindDescendantByName(DependencyObject ^ element, String ^ name)
{
    if (element == nullptr || name == nullptr || name->Length() == 0)
    {
        return nullptr;
    }

    auto frameworkElement = dynamic_cast<FrameworkElement ^>(element);
    if (frameworkElement != nullptr && name->Equals(frameworkElement->Name))
    {
        return frameworkElement;
    }

    auto childCount = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childCount; i++)
    {
        auto result = FindDescendantByName(VisualTreeHelper::GetChild(element, i), name);
        if (result != nullptr)
        {
            return result;
        }
    }

    return nullptr;
}

DependencyObject ^ VisualTree::FindDescendant(DependencyObject ^ element, TypeName typeName)
{
    DependencyObject ^ retValue = nullptr;
    auto childrenCount = VisualTreeHelper::GetChildrenCount(element);

    for (auto i = 0; i < childrenCount; i++)
    {
        auto child = VisualTreeHelper::GetChild(element, i);
        if (child->GetType() == typeName)
        {
            retValue = child;
            break;
        }

        retValue = FindDescendant(child, typeName);

        if (retValue != nullptr)
        {
            break;
        }
    }

    return retValue;
}

FrameworkElement ^ VisualTree::FindAscendantByName(DependencyObject ^ element, String ^ name)
{
    if (element == nullptr || name == nullptr || name->Length() == 0)
    {
        return nullptr;
    }

    auto parent = VisualTreeHelper::GetParent(element);

    if (parent == nullptr)
    {
        return nullptr;
    }
    auto frameworkElement = dynamic_cast<FrameworkElement ^>(parent);
    if (frameworkElement != nullptr && name->Equals(frameworkElement->Name))
    {
        return frameworkElement;
    }

    return FindAscendantByName(parent, name);
}

Object ^ VisualTree::FindAscendant(DependencyObject ^ element, TypeName typeName)
{
    auto parent = VisualTreeHelper::GetParent(element);

    if (parent == nullptr)
    {
        return nullptr;
    }

    if (parent->GetType() == typeName)
    {
        return parent;
    }

    return FindAscendant(parent, typeName);
}
