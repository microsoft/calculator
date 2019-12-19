#include "pch.h"

#include "GraphingSettings.xaml.h"
#include "CalcViewModel\Common\AppResourceProvider.cpp"

using namespace std;
using namespace Graphing;
using namespace GraphControl;

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

GraphingSettings::GraphingSettings()
    : m_ViewModel(ref new GraphingSettingsViewModel())
{
    InitializeComponent();
}

void GraphingSettings::SetGrapher(Grapher ^ grapher)
{
    m_ViewModel->SetGrapher(grapher);
}

Style ^ GraphingSettings::SelectTextBoxStyle(bool incorrectRange, bool error)
{
    if (incorrectRange || error)
    {
        return static_cast<::Style ^>(this->Resources->Lookup(L"ErrorTextBoxStyle"));
    }
    else
    {
        return nullptr;
    }
}

void GraphingSettings::GridSettingsTextBox_PreviewKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e)
{
    if (e->Key == VirtualKey::Enter)
    {
        if (!FocusManager::TryMoveFocusAsync(FocusNavigationDirection::Next))
        {
            FocusManager::TryMoveFocusAsync(FocusNavigationDirection::Previous);
        }
        e->Handled = true;
    }
}

bool GraphingSettings::CanBeClose()
{
    auto focusedElement = FocusManager::GetFocusedElement();

    // Move focus so we are sure all values are in sync with the VM
    if (focusedElement != nullptr)
    {
        if (focusedElement->Equals(SettingsXMin))
        {
            auto textbox = static_cast<TextBox ^>(focusedElement);
            ViewModel->XMin = textbox->Text;
        }
        else if (focusedElement->Equals(SettingsXMax))
        {
            auto textbox = static_cast<TextBox ^>(focusedElement);
            ViewModel->XMax = textbox->Text;
        }
        else if (focusedElement->Equals(SettingsYMin))
        {
            auto textbox = static_cast<TextBox ^>(focusedElement);
            ViewModel->YMin = textbox->Text;
        }
        else if (focusedElement->Equals(SettingsYMax))
        {
            auto textbox = static_cast<TextBox ^>(focusedElement);
            ViewModel->YMax = textbox->Text;
        }
    }
    return ViewModel != nullptr && ViewModel->HasError();
}

void GraphingSettings::RefreshRanges()
{
    ViewModel->InitRanges();
}
