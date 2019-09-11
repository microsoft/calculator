//
// StylePanel.xaml.cpp
// Implementation of the StylePanel class
//

#include "pch.h"
#include "StylePanel.xaml.h"

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

DEPENDENCY_PROPERTY_INITIALIZATION(StylePanel, SelectedColor);

StylePanel::StylePanel()
{
    InitializeComponent();
}

void StylePanel::SelectionChanged(Object ^ /*sender */, SelectionChangedEventArgs ^ e)
{
    if (e->AddedItems->Size > 0)
    {
        SelectedColor = static_cast<SolidColorBrush ^>(e->AddedItems->GetAt(0));
    }
}

void StylePanel::OnSelectedColorPropertyChanged(SolidColorBrush ^ /*oldColor*/, SolidColorBrush ^ newColor)
{
    SelectColor(newColor);
}

void StylePanel::ColorChooserLoaded(Object ^ sender, RoutedEventArgs ^ e)
{
    SelectColor(SelectedColor);
}

void StylePanel::SelectColor(SolidColorBrush ^ selectedColor)
{
    for (auto item : ColorChooser->Items->GetView())
    {
        auto brush = static_cast<SolidColorBrush ^>(item);
        auto gridViewItem = dynamic_cast<GridViewItem ^>(ColorChooser->ContainerFromItem(brush));

        if (!gridViewItem)
        {
            continue;
        }

        auto ellipse = GetEllipse(gridViewItem);

        if (brush->Color == selectedColor->Color)
        {
            ellipse->StrokeThickness = 3;
            gridViewItem->IsSelected = true;
        }
        else
        {
            ellipse->StrokeThickness = 0;
            gridViewItem->IsSelected = false;
        }
    }
}

Ellipse ^ StylePanel::GetEllipse(GridViewItem ^ gridViewItem)
{
    return dynamic_cast<Ellipse ^>(static_cast<Grid ^>(gridViewItem->ContentTemplateRoot)->Children->GetAt(0));
}
