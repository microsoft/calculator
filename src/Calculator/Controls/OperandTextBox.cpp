// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "OperandTextBox.h"
#include "regex"

using namespace CalculatorApp;
using namespace CalculatorApp::Controls;

using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Input;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

void OperandTextBox::OnApplyTemplate()
{   
    this->IsEnabled = false;
    this->IsHitTestVisible = false;
    this->IsTapEnabled = false;
    this->MaxLength = 50;
    this->IsRightTapEnabled = false;
    this->IsTabStop = false;
    auto parent = VisualTreeHelper::GetParent(this);
    ListViewItem^ listViewItem;
    ListView^ listView;
    while (parent != nullptr)
    {
        if (listViewItem == nullptr)
        {
            listViewItem = dynamic_cast<ListViewItem^>(parent);
        }

        listView = dynamic_cast<ListView^>(parent);
        if (listView != nullptr)
        {
            break;
        }
        parent = VisualTreeHelper::GetParent(parent);
    }

    if (listView != nullptr)
    {
        listViewItem->IsEnabled = false;
        listViewItem->IsHitTestVisible = false;
        listViewItem->IsTapEnabled = false;
    }
    TextBox::OnApplyTemplate();
}
