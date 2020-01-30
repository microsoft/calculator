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
using namespace GraphControl;

DEPENDENCY_PROPERTY_INITIALIZATION(EquationStylePanelControl, SelectedColor);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationStylePanelControl, SelectedStyle);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationStylePanelControl, AvailableColors);

EquationStylePanelControl::EquationStylePanelControl()
{
    InitializeComponent();

    auto allStyles = ref new Vector<EquationLineStyle>();
    allStyles->Append(::EquationLineStyle::Solid);
    allStyles->Append(::EquationLineStyle::Dash);
    allStyles->Append(::EquationLineStyle::Dot);
    allStyles->Append(::EquationLineStyle::DashDot);
    allStyles->Append(::EquationLineStyle::DashDotDot);

    StyleChooserBox->ItemsSource = allStyles;
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

void CalculatorApp::EquationStylePanelControl::OnSelectedStylePropertyChanged(
    GraphControl::EquationLineStyle,
    GraphControl::EquationLineStyle newStyle)
{
    SelectStyle(newStyle);
}

void EquationStylePanelControl::SelectStyle(EquationLineStyle selectedStyle)
{
    for (auto item : StyleChooserBox->Items->GetView())
    {
        auto style = static_cast<EquationLineStyle>(item);
        auto comboBoxItem = dynamic_cast<ComboBoxItem ^>(StyleChooserBox->ContainerFromItem(style));

        if (!comboBoxItem)
        {
            continue;
        }

        if (style == selectedStyle)
        {
            comboBoxItem->IsSelected = true;
            return;
        }
        else
        {
            comboBoxItem->IsSelected = false;
        }
    }
}

void EquationStylePanelControl::StyleChooser_SelectionChanged(Object ^ sender, SelectionChangedEventArgs ^ e)
{
    if (e->AddedItems->Size > 0)
    {
        SelectedStyle = static_cast<EquationLineStyle>(e->AddedItems->GetAt(0));
    }
}

void EquationStylePanelControl::StyleChooserBox_SelectionChanged(
    Object ^ sender,
    SelectionChangedEventArgs ^ e)
{
    if (e->AddedItems->Size > 0)
    {
        SelectedStyle = static_cast<EquationLineStyle>(e->AddedItems->GetAt(0));
    }
}

void EquationStylePanelControl::StyleChooserBox_Loaded(Object ^ sender, RoutedEventArgs ^ e)
{
    SelectStyle(SelectedStyle);
}

DoubleCollection ^ EquationStylePanelControl::GetLinePattern(Object ^line)
{
    auto lineStyle = static_cast<Box<EquationLineStyle> ^>(line)->Value;

    auto linePattern = ref new DoubleCollection();
    switch (lineStyle)
    {
    case ::EquationLineStyle::Dot:
        linePattern->Append(1);
        break;
    case ::EquationLineStyle::Dash:
        linePattern->Append(2);
        linePattern->Append(1);
        break;
    case ::EquationLineStyle::DashDot:
        linePattern->Append(2.5);
        linePattern->Append(1);
        linePattern->Append(1);
        linePattern->Append(1);
        linePattern->Append(2.5);
        linePattern->Append(1);
        linePattern->Append(1);
        linePattern->Append(1);
        break;
    case ::EquationLineStyle::DashDotDot:
        linePattern->Append(3);
        linePattern->Append(1);
        linePattern->Append(1);
        linePattern->Append(1);
        linePattern->Append(1);
        linePattern->Append(1);
        break;
    default:
        break;
    }

    return linePattern;
}
