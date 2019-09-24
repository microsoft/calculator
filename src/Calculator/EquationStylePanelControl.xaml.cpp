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

EquationStylePanelControl::EquationStylePanelControl()
{
    InitializeComponent();
    InitializeAvailableColors();
}

void EquationStylePanelControl::InitializeAvailableColors()
{
    // TODO: Handle dynamically switching these to high contrast equivalents
    m_AvailableColors = ref new Vector<SolidColorBrush ^>();
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush1")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush2")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush3")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush4")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush5")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush6")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush7")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush8")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush9")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush10")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush11")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush12")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush13")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush14")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush15")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush16")));
}

void EquationStylePanelControl::SelectionChanged(Object ^ /*sender */, SelectionChangedEventArgs ^ e)
{
    if (e->AddedItems->Size > 0)
    {
        SelectedColor = static_cast<SolidColorBrush ^>(e->AddedItems->GetAt(0));
    }
}

void EquationStylePanelControl::OnSelectedColorPropertyChanged(SolidColorBrush ^ /*oldColor*/, SolidColorBrush ^ newColor)
{
    SelectColor(newColor);
}

void EquationStylePanelControl::ColorChooserLoaded(Object ^ sender, RoutedEventArgs ^ e)
{
    SelectColor(SelectedColor);
}

void EquationStylePanelControl::SelectColor(SolidColorBrush ^ selectedColor)
{
    for (auto item : ColorChooser->Items->GetView())
    {
        auto brush = static_cast<SolidColorBrush ^>(item);
        auto gridViewItem = dynamic_cast<GridViewItem ^>(ColorChooser->ContainerFromItem(brush));

        if (!gridViewItem)
        {
            continue;
        }

        if (brush->Color == selectedColor->Color)
        {
            gridViewItem->IsSelected = true;
        }
        else
        {
            gridViewItem->IsSelected = false;
        }
    }
}
