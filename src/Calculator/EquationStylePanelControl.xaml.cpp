// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EquationStylePanelControl.xaml.h"
#include "CalcViewModel/Common/AppResourceProvider.h"

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
DEPENDENCY_PROPERTY_INITIALIZATION(EquationStylePanelControl, EnableLineStylePicker);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationStylePanelControl, SelectedColorIndex);
DEPENDENCY_PROPERTY_INITIALIZATION(EquationStylePanelControl, AvailableColors);

EquationStylePanelControl::EquationStylePanelControl()
{
    InitializeComponent();

    auto allStyles = ref new Vector<EquationLineStyle>();
    allStyles->Append(::EquationLineStyle::Solid);
    allStyles->Append(::EquationLineStyle::Dash);
    allStyles->Append(::EquationLineStyle::Dot);

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

        TraceLogger::GetInstance()->LogGraphLineStyleChanged(LineStyleType::Color);
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
    for (unsigned int i = 0; i < ColorChooser->Items->Size; i++)
    {
        auto item = ColorChooser->Items->GetAt(i);
        auto brush = static_cast<SolidColorBrush ^>(item);
        auto gridViewItem = dynamic_cast<GridViewItem ^>(ColorChooser->ContainerFromItem(brush));

        if (!gridViewItem)
        {
            continue;
        }

        if (Utils::AreColorsEqual(brush->Color, selectedColor))
        {
            gridViewItem->IsSelected = true;
            SelectedColorIndex = i;
            return;
        }
        else
        {
            gridViewItem->IsSelected = false;
        }
    }
}

void EquationStylePanelControl::OnSelectedStylePropertyChanged(EquationLineStyle oldStyle, EquationLineStyle newStyle)
{
    if (oldStyle != newStyle)
    {
        SelectStyle(newStyle);
        TraceLogger::GetInstance()->LogGraphLineStyleChanged(LineStyleType::Pattern);
    }
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

void EquationStylePanelControl::StyleChooserBox_SelectionChanged(Object ^ sender, SelectionChangedEventArgs ^ e)
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

String ^ EquationStylePanelControl::GetColorAutomationName(Brush ^ brush)
{
    auto resourceLoader = AppResourceProvider::GetInstance();
    auto color = static_cast<SolidColorBrush ^>(brush);

    auto lightDictionary = static_cast<ResourceDictionary ^>(Application::Current->Resources->ThemeDictionaries->Lookup(L"Light"));
    auto darkDictionary = static_cast<ResourceDictionary ^>(Application::Current->Resources->ThemeDictionaries->Lookup(L"Default"));
    auto highContrast = static_cast<ResourceDictionary ^>(Application::Current->Resources->ThemeDictionaries->Lookup(L"HighContrast"));

    if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush1"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush1")))
    {
        return resourceLoader->GetResourceString("equationColor1AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush2"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush2")))
    {
        return resourceLoader->GetResourceString("equationColor2AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush3"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush3")))
    {
        return resourceLoader->GetResourceString("equationColor3AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush4"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush4")))
    {
        return resourceLoader->GetResourceString("equationColor4AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush5"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush5")))
    {
        return resourceLoader->GetResourceString("equationColor5AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush6"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush6")))
    {
        return resourceLoader->GetResourceString("equationColor6AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush7"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush7")))
    {
        return resourceLoader->GetResourceString("equationColor7AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush8"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush8")))
    {
        return resourceLoader->GetResourceString("equationColor8AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush9"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush9")))
    {
        return resourceLoader->GetResourceString("equationColor9AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush10"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush10")))
    {
        return resourceLoader->GetResourceString("equationColor10AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush11"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush11")))
    {
        return resourceLoader->GetResourceString("equationColor11AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush12"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush12")))
    {
        return resourceLoader->GetResourceString("equationColor12AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush13"))
        || color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush13")))
    {
        return resourceLoader->GetResourceString("equationColor13AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(darkDictionary->Lookup(L"EquationBrush14")))
    {
        return resourceLoader->GetResourceString("equationColor14WhiteAutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(lightDictionary->Lookup(L"EquationBrush14")))
    {
        return resourceLoader->GetResourceString("equationColor14BlackAutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(highContrast->Lookup(L"EquationBrush1")))
    {
        return resourceLoader->GetResourceString("equationHighContrastColor1AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(highContrast->Lookup(L"EquationBrush2")))
    {
        return resourceLoader->GetResourceString("equationHighContrastColor2AutomationName");
    }
    else if (color == safe_cast<SolidColorBrush ^>(highContrast->Lookup(L"EquationBrush3")))
    {
        return resourceLoader->GetResourceString("equationHighContrastColor3AutomationName");
    }
    else
    {
        return resourceLoader->GetResourceString("equationHighContrastColor4AutomationName");
    }

}

String ^ EquationStylePanelControl::GetLineAutomationName(Object ^ line)
{
    auto resourceLoader = AppResourceProvider::GetInstance();
    auto lineStyle = static_cast<Box<EquationLineStyle> ^>(line)->Value;

    switch (lineStyle)
    {
    case ::EquationLineStyle::Dot:
        return resourceLoader->GetResourceString("dotLineStyleAutomationName");
        break;
    case ::EquationLineStyle::Dash:
        return resourceLoader->GetResourceString("dashLineStyleAutomationName");
        break;
    case ::EquationLineStyle::Solid:
    default:
        return resourceLoader->GetResourceString("solidLineStyleAutomationName");
        break;
    }
}

DoubleCollection ^ EquationStylePanelControl::GetLinePattern(Object ^ line)
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
    default:
        break;
    }

    return linePattern;
}
