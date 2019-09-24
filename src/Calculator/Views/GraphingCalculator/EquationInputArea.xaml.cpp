#include "pch.h"
#include "EquationInputArea.xaml.h"

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
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Input;
using namespace GraphControl;

namespace
{
    StringReference EquationsPropertyName(L"Equations");
    const int numberOfEquationColors = 16;
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
    Equations->Append(eq);
}

void EquationInputArea::InputTextBox_GotFocus(Object^ sender, RoutedEventArgs^ e)
{
    KeyboardShortcutManager::HonorShortcuts(false);
}

void EquationInputArea::InputTextBox_LostFocus(Object^ sender, RoutedEventArgs^ e)
{
    KeyboardShortcutManager::HonorShortcuts(true);
    
    auto tb = static_cast<EquationTextBox^>(sender);
    auto eq = static_cast<EquationViewModel^>(tb->DataContext);
    tb->SetEquationText(eq->Expression);
}

void EquationInputArea::InputTextBox_KeyUp(Object^ sender, KeyRoutedEventArgs^ e)
{
    if (e->Key == VirtualKey::Enter)
    {
        auto tb = static_cast<EquationTextBox^>(sender);
        auto eq = static_cast<EquationViewModel^>(tb->DataContext);
        eq->Expression = tb->GetEquationText();
        FocusManager::TryMoveFocus(::FocusNavigationDirection::Left);
        e->Handled = true;
    }
}

SolidColorBrush^ EquationInputArea::GetNextLineColor()
{
    m_lastLineColorIndex = (m_lastLineColorIndex + 1) % numberOfEquationColors;

    // TODO: Is there a better way to do this?
    return ref new SolidColorBrush(static_cast<Color>(Application::Current->Resources->Lookup("EquationColor" + (m_lastLineColorIndex + 1))));
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

void EquationInputArea::EquationTextBoxLoaded(Object ^ sender, RoutedEventArgs ^ e)
{
    auto tb = static_cast<EquationTextBox ^>(sender);
    auto eq = static_cast<EquationViewModel ^>(tb->DataContext);

    auto colorChooser = static_cast<EquationStylePanelControl ^>(tb->ColorChooserFlyout->Content);

    m_lastLineColorIndex = (m_lastLineColorIndex + 1) % colorChooser->AvailableColors->Size;

    eq->LineColor = colorChooser->AvailableColors->GetAt(m_lastLineColorIndex);
}
