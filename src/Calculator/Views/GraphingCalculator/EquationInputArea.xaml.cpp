// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EquationInputArea.xaml.h"
#include "Utils/VisualTree.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace GraphControl;
using namespace CalculatorApp::ViewModel;
using namespace CalculatorApp::Controls;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Input;
using namespace GraphControl;
using namespace Calculator::Utils;

namespace
{
    inline constexpr auto maxEquationSize = 14;
    inline constexpr std::array<int, 14> colorAssignmentMapping = { 0, 3, 7, 10, 1, 4, 8, 11, 2, 5, 9, 12, 6, 13 };

    StringReference EquationsPropertyName(L"Equations");
}

EquationInputArea::EquationInputArea()
    : m_lastLineColorIndex{ -1 }
    , m_AvailableColors{ ref new Vector<SolidColorBrush ^>() }
    , m_accessibilitySettings{ ref new AccessibilitySettings() }
    , m_equationToFocus{ nullptr }
{
    m_accessibilitySettings->HighContrastChanged +=
        ref new TypedEventHandler<AccessibilitySettings ^, Object ^>(this, &EquationInputArea::OnHighContrastChanged);

    ReloadAvailableColors(m_accessibilitySettings->HighContrast);

    InitializeComponent();
}

void EquationInputArea::OnPropertyChanged(String ^ propertyName)
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

void EquationInputArea::AddNewEquation()
{
    if (Equations->Size > 0)
    {
        Equations->GetAt(Equations->Size - 1)->IsLastItemInList = false;
    }

    // Cap equations at 14
    if (Equations->Size >= maxEquationSize)
    {
        return;
    }

    m_lastLineColorIndex = (m_lastLineColorIndex + 1) % AvailableColors->Size;

    int colorIndex;

    if (m_accessibilitySettings->HighContrast)
    {
        colorIndex = m_lastLineColorIndex;
    }
    else
    {
        colorIndex = colorAssignmentMapping[m_lastLineColorIndex];
    }

    auto eq = ref new EquationViewModel(ref new Equation(), ++m_lastFunctionLabelIndex, AvailableColors->GetAt(colorIndex)->Color);
    eq->IsLastItemInList = true;
    m_equationToFocus = eq;
    Equations->Append(eq);
}

void EquationInputArea::EquationTextBox_GotFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    KeyboardShortcutManager::HonorShortcuts(false);
}

void EquationInputArea::EquationTextBox_LostFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    KeyboardShortcutManager::HonorShortcuts(true);
}

void EquationInputArea::EquationTextBox_Submitted(Object ^ sender, MathRichEditBoxSubmission ^ submission)
{
    auto tb = static_cast<EquationTextBox ^>(sender);
    if (tb == nullptr)
    {
        return;
    }
    auto eq = static_cast<EquationViewModel ^>(tb->DataContext);
    if (eq == nullptr)
    {
        return;
    }

    if (submission->Source == EquationSubmissionSource::ENTER_KEY
        || (submission->Source == EquationSubmissionSource::FOCUS_LOST && submission->HasTextChanged && eq->Expression != nullptr
            && eq->Expression->Length() > 0))
    {
        unsigned int index = 0;
        if (Equations->IndexOf(eq, &index))
        {
            if (index == Equations->Size - 1)
            {
                // If it's the last equation of the list
                AddNewEquation();
            }
            else
            {
                if (submission->Source == EquationSubmissionSource::ENTER_KEY)
                {
                    auto nextEquation = Equations->GetAt(index + 1);
                    FocusEquationTextBox(nextEquation);
                }
            }
        }
    }
}

void EquationInputArea::FocusEquationTextBox(EquationViewModel ^ equation)
{
    unsigned int index;
    if (!Equations->IndexOf(equation, &index) || index < 0)
    {
        return;
    }
    auto container = EquationInputList->TryGetElement(index);
    if (container == nullptr)
    {
        return;
    }
    auto equationTextBox = dynamic_cast<EquationTextBox ^>(container);
    if (equationTextBox != nullptr)
    {
        equationTextBox->FocusTextBox();
    }
    else
    {
        auto equationInput = VisualTree::FindDescendantByName(container, "EquationInputButton");
        if (equationInput == nullptr)
        {
            return;
        }
        equationTextBox = dynamic_cast<EquationTextBox ^>(equationInput);
        if (equationTextBox != nullptr)
        {
            equationTextBox->FocusTextBox();
        }
    }
}

void EquationInputArea::EquationTextBox_RemoveButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    auto tb = static_cast<EquationTextBox ^>(sender);
    auto eq = static_cast<EquationViewModel ^>(tb->DataContext);
    unsigned int index;

    if (Equations->IndexOf(eq, &index))
    {
        // Prevent removing the last equation
        if (index == Equations->Size - 1)
        {
            return;
        }

        Equations->RemoveAt(index);
        int lastIndex = Equations->Size - 1;

        if (Equations->Size <= 1)
        {
            m_lastFunctionLabelIndex = 1;
        }
        else
        {
            m_lastFunctionLabelIndex = Equations->GetAt(lastIndex - 1)->FunctionLabelIndex + 1;
        }

        Equations->GetAt(lastIndex)->FunctionLabelIndex = m_lastFunctionLabelIndex;
    }
}

void EquationInputArea::EquationTextBox_KeyGraphFeaturesButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    auto tb = static_cast<EquationTextBox ^>(sender);
    auto eq = static_cast<EquationViewModel ^>(tb->DataContext);
    KeyGraphFeaturesRequested(this, eq);
}

void EquationInputArea::EquationTextBox_EquationButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    auto tb = static_cast<EquationTextBox ^>(sender);
    auto eq = static_cast<EquationViewModel ^>(tb->DataContext);
    eq->IsLineEnabled = !eq->IsLineEnabled;
}

void EquationInputArea::EquationTextBox_Loaded(Object ^ sender, RoutedEventArgs ^ e)
{
    auto tb = static_cast<EquationTextBox ^>(sender);

    auto colorChooser = static_cast<EquationStylePanelControl ^>(tb->ColorChooserFlyout->Content);
    colorChooser->AvailableColors = AvailableColors;

    if (m_equationToFocus != nullptr && tb->DataContext == m_equationToFocus)
    {
        auto copyEquationToFocus = m_equationToFocus;
        m_equationToFocus = nullptr;
        tb->FocusTextBox();

        unsigned int index;
        if (Equations->IndexOf(copyEquationToFocus, &index))
        {
            auto container = EquationInputList->TryGetElement(index);
            if (container != nullptr)
            {
                container->StartBringIntoView();
            }
        }
    }
}

void EquationInputArea::EquationTextBox_DataContextChanged(Windows::UI::Xaml::FrameworkElement ^ sender, Windows::UI::Xaml::DataContextChangedEventArgs ^ args)
{
    auto tb = static_cast<EquationTextBox ^>(sender);
    if (!tb->IsLoaded)
    {
        return;
    }

    FocusEquationIfNecessary(tb);
}

void EquationInputArea::FocusEquationIfNecessary(CalculatorApp::Controls::EquationTextBox ^ textBox)
{
    if (m_equationToFocus != nullptr && textBox->DataContext == m_equationToFocus)
    {
        m_equationToFocus = nullptr;
        textBox->FocusTextBox();

        unsigned int index;
        if (Equations->IndexOf(m_equationToFocus, &index))
        {
            auto container = EquationInputList->TryGetElement(index);
            if (container != nullptr)
            {
                container->StartBringIntoView();
            }
        }
    }
}

void EquationInputArea::OnHighContrastChanged(AccessibilitySettings ^ sender, Object ^ args)
{
    ReloadAvailableColors(sender->HighContrast);
}

void EquationInputArea::ReloadAvailableColors(bool isHighContrast)
{
    m_AvailableColors->Clear();

    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush1")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush2")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush3")));
    m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush4")));

    // If this is not high contrast, we have all 16 colors, otherwise we will restrict this to a subset of high contrast colors
    if (!isHighContrast)
    {
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
    }

    // If there are no equations to reload, quit early
    if (Equations == nullptr || Equations->Size == 0)
    {
        return;
    }

    // Reassign colors for each equation
    m_lastLineColorIndex = -1;
    for (auto equationViewModel : Equations)
    {
        m_lastLineColorIndex = (m_lastLineColorIndex + 1) % AvailableColors->Size;
        equationViewModel->LineColor = AvailableColors->GetAt(m_lastLineColorIndex)->Color;
    }
}

void EquationInputArea::TextBoxGotFocus(TextBox ^ sender, RoutedEventArgs ^ e)
{
    sender->SelectAll();
}

void EquationInputArea::SubmitTextbox(TextBox ^ sender)
{
    auto variableViewModel = static_cast<VariableViewModel ^>(sender->DataContext);
    double val;
    if (sender->Name == "ValueTextBox")
    {
        val = validateDouble(sender->Text, variableViewModel->Value);
        variableViewModel->Value = val;
    }
    else if (sender->Name == "MinTextBox")
    {
        val = validateDouble(sender->Text, variableViewModel->Min);
        variableViewModel->Min = val;
    }
    else if (sender->Name == "MaxTextBox")
    {
        val = validateDouble(sender->Text, variableViewModel->Max);
        variableViewModel->Max = val;
    }
    else if (sender->Name == "StepTextBox")
    {
        val = validateDouble(sender->Text, variableViewModel->Step);
        variableViewModel->Step = val;
    }
    else
    {
        return;
    }

    wostringstream oss;
    oss << std::noshowpoint << val;
    sender->Text = ref new String(oss.str().c_str());
}

void EquationInputArea::TextBoxLosingFocus(TextBox ^ sender, LosingFocusEventArgs ^)
{
    SubmitTextbox(sender);
}

void EquationInputArea::TextBoxKeyDown(TextBox ^ sender, KeyRoutedEventArgs ^ e)
{
    if (e->Key == ::VirtualKey::Enter)
    {
        SubmitTextbox(sender);
    }
}

double EquationInputArea::validateDouble(String ^ value, double defaultValue)
{
    try
    {
        return stod(value->Data());
    }
    catch (...)
    {
        return defaultValue;
    }
}

::Visibility EquationInputArea::ManageEditVariablesButtonVisibility(unsigned int numberOfVariables)
{
    return numberOfVariables == 0 ? ::Visibility::Collapsed : ::Visibility::Visible;
}

String ^ EquationInputArea::GetChevronIcon(bool isCollapsed)
{
    return isCollapsed ? L"\uE70E" : L"\uE70D";
}

void EquationInputArea::VariableAreaTapped(Object ^ sender, TappedRoutedEventArgs ^ e)
{
    auto selectedVariableViewModel = static_cast<VariableViewModel ^>(static_cast<Grid ^>(sender)->DataContext);
    selectedVariableViewModel->SliderSettingsVisible = !selectedVariableViewModel->SliderSettingsVisible;

    // Collapse all other slider settings that are open
    for (auto variableViewModel : Variables)
    {
        if (variableViewModel != selectedVariableViewModel)
        {
            variableViewModel->SliderSettingsVisible = false;
        }
    }
}
  
void EquationInputArea::EquationTextBox_EquationFormatRequested(Object ^ sender, MathRichEditBoxFormatRequest ^ e)
{
    EquationFormatRequested(sender, e);
}
