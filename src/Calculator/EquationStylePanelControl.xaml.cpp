// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EquationStylePanelControl.xaml.h"

using namespace CalculatorApp;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;

DEPENDENCY_PROPERTY_INITIALIZATION(EquationStylePanelControl, SelectedColor);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationStylePanelControl, AvailableColors);

EquationStylePanelControl::EquationStylePanelControl()
{
    InitializeComponent();
}

void EquationStylePanelControl::SelectionChanged(Object ^ /*sender */, SelectionChangedEventArgs ^ e)
{
    if (e->AddedItems->Size > 0)
    {
        auto brush = dynamic_cast<SolidColorBrush ^>(e->AddedItems->GetAt(0));
        if (brush == nullptr)
        {
            SelectedColor = Colors::Black;
        }
        else
        {
            SelectedColor = brush->Color;
        }
    }
}

void EquationStylePanelControl::OnSelectedColorPropertyChanged(Color /*oldColor*/, Color newColor)
{
    SelectColor(newColor);
}

void EquationStylePanelControl::ColorChooserLoaded(Object ^ sender, RoutedEventArgs ^ e)
{
    SelectColor(SelectedColor);
}

void EquationStylePanelControl::SelectColor(Color selectedColor)
{
    for (auto item : ColorChooser->Items->GetView())
    {
        auto brush = static_cast<SolidColorBrush ^>(item);
        auto gridViewItem = dynamic_cast<GridViewItem ^>(ColorChooser->ContainerFromItem(brush));

        if (!gridViewItem)
        {
            continue;
        }

        if (Utils::AreColorsEqual(brush->Color, selectedColor))
        {
            gridViewItem->IsSelected = true;
            return;
        }
        else
        {
            gridViewItem->IsSelected = false;
        }
    }
}
