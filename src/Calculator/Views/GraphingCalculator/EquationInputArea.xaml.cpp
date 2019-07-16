#include "pch.h"
#include "EquationInputArea.xaml.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "Controls/EquationTextBox.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace CalculatorApp::Controls;
using namespace Platform;
using namespace std;
using namespace Windows::System;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Input;

namespace
{
    const Color accentColor = (ref new UISettings())->GetColorValue(UIColorType::Accent);
    const Color lineColors[] = {
        accentColor,
        Colors::DarkOrange,
        Colors::MediumPurple,
        Colors::ForestGreen,
        Colors::BlueViolet,
        Colors::DarkRed,
        Colors::LightGoldenrodYellow,
        Colors::DarkOliveGreen
    };
    const size_t lineColorsSize = std::size(lineColors);

    StringReference EquationsPropertyName(L"Equations");
}

EquationInputArea::EquationInputArea()
    : m_lastLineColorIndex{ -1 }
{
	InitializeComponent();
}

void EquationInputArea::OnPropertyChanged(String^ propertyName)
{
    if (propertyName == EquationsPropertyName)
    {
        OnEquationsPropertyChanged();
    }
}

void EquationInputArea::OnEquationsPropertyChanged()
{
    if (Equations != nullptr && Equations->Size == 0)
    {
        AddNewEquation();
    }
}

void EquationInputArea::AddEquationButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    AddNewEquation();
}

void EquationInputArea::AddNewEquation()
{
    auto eq = ref new EquationViewModel();
    eq->LineColor = GetNextLineColor();

    Equations->Append(eq);
}

void EquationInputArea::InputTextBox_GotFocus(Object^ sender, RoutedEventArgs^ e)
{
    KeyboardShortcutManager::HonorShortcuts(false);
}

void EquationInputArea::InputTextBox_LostFocus(Object^ sender, RoutedEventArgs^ e)
{
    KeyboardShortcutManager::HonorShortcuts(true);
}

void EquationInputArea::InputTextBox_Submitted(Object ^ sender, RoutedEventArgs ^ e)
{
    auto tb = static_cast<EquationTextBox^>(sender);
    auto eq = static_cast<EquationViewModel^>(tb->DataContext);
    eq->Expression = tb->GetEquationText();
}

Color EquationInputArea::GetNextLineColor()
{
    m_lastLineColorIndex = (m_lastLineColorIndex + 1) % lineColorsSize;
    return lineColors[m_lastLineColorIndex];
}


void EquationInputArea::EquationTextBox_RemoveButtonClicked(Object^ sender, RoutedEventArgs^ e)
{
    auto tb = static_cast<EquationTextBox^>(sender);
    auto eq = static_cast<EquationViewModel^>(tb->DataContext);
    unsigned int index;
    if (Equations->IndexOf(eq, &index))
    {
        Equations->RemoveAt(index);
    }
}
