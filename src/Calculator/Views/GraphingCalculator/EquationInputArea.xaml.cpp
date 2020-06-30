// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EquationInputArea.xaml.h"
#include "Utils/VisualTree.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "CalcViewModel/Common/Automation/NarratorAnnouncement.h"
#include "CalcViewModel/Common/Automation/NarratorNotifier.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::Automation;
using namespace GraphControl;
using namespace CalculatorApp::ViewModel;
using namespace CalculatorApp::Controls;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI;
using namespace Windows::UI::Core;
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
    inline constexpr auto colorCount = 14;
    inline constexpr std::array<int, 14> colorAssignmentMapping = { 0, 3, 7, 10, 1, 4, 8, 11, 2, 5, 9, 12, 6, 13 };

    StringReference EquationsPropertyName(L"Equations");
    StringReference IsMatchAppThemePropertyName(L"IsMatchAppTheme");
}

EquationInputArea::EquationInputArea()
    : m_lastLineColorIndex{ -1 }
    , m_AvailableColors{ ref new Vector<SolidColorBrush ^>() }
    , m_accessibilitySettings{ ref new AccessibilitySettings() }
    , m_equationToFocus{ nullptr }
{
    m_accessibilitySettings->HighContrastChanged +=
        ref new TypedEventHandler<AccessibilitySettings ^, Object ^>(this, &EquationInputArea::OnHighContrastChanged);
    m_isHighContrast = m_accessibilitySettings->HighContrast;

    m_uiSettings = ref new UISettings();
    m_uiSettings->ColorValuesChanged += ref new TypedEventHandler<UISettings ^, Object ^>(this, &EquationInputArea::OnColorValuesChanged);

    ReloadAvailableColors(m_accessibilitySettings->HighContrast, true);

    InitializeComponent();
}

void EquationInputArea::OnPropertyChanged(String ^ propertyName)
{
    if (propertyName == EquationsPropertyName)
    {
        OnEquationsPropertyChanged();
    }

    else if (propertyName == IsMatchAppThemePropertyName)
    {
        ReloadAvailableColors(m_accessibilitySettings->HighContrast, false);
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

    int colorIndex;

    if (m_accessibilitySettings->HighContrast)
    {
        m_lastLineColorIndex = (m_lastLineColorIndex + 1) % AvailableColors->Size;
        colorIndex = m_lastLineColorIndex;
    }
    else
    {
        std::array<bool, colorCount> colorAssignmentUsed{};
        for (auto equation : Equations)
        {
            colorAssignmentUsed[equation->LineColorIndex] = true;
        }

        colorIndex = 0;
        // If for some reason all of the values in colorAssignmentUsed are true, the check for colorIndex < colorCount - 1 will
        // set it to the last color in the list
        while (colorIndex < colorCount - 1 && colorAssignmentUsed[colorAssignmentMapping[colorIndex]])
        {
            colorIndex++;
        }

        colorIndex = colorAssignmentMapping[colorIndex];
    }

    auto eq = ref new EquationViewModel(ref new Equation(), ++m_lastFunctionLabelIndex, AvailableColors->GetAt(colorIndex)->Color, colorIndex);
    eq->IsLastItemInList = true;
    m_equationToFocus = eq;
    Equations->Append(eq);
}

void EquationInputArea::EquationTextBox_GotFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    auto eq = GetViewModelFromEquationTextBox(sender);
    if (eq != nullptr)
    {
        eq->GraphEquation->IsSelected = true;
    }
}

void EquationInputArea::EquationTextBox_LostFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    auto eq = GetViewModelFromEquationTextBox(sender);
    if (eq != nullptr)
    {
        eq->GraphEquation->IsSelected = false;
    }
}

void EquationInputArea::EquationTextBox_Submitted(Object ^ sender, MathRichEditBoxSubmission ^ submission)
{
    auto eq = GetViewModelFromEquationTextBox(sender);
    if (eq == nullptr)
    {
        return;
    }

    if (submission->Source == EquationSubmissionSource::ENTER_KEY
        || (submission->Source == EquationSubmissionSource::FOCUS_LOST && submission->HasTextChanged && eq->Expression != nullptr
            && eq->Expression->Length() > 0))
    {
        if (submission->Source == EquationSubmissionSource::ENTER_KEY)
        {
            eq->IsLineEnabled = true;
        }

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
    auto container = static_cast<UIElement ^>(EquationInputList->ContainerFromIndex(index));
    if (container != nullptr)
    {
        container->StartBringIntoView();

        auto equationInput = VisualTree::FindDescendantByName(container, "EquationInputButton");
        if (equationInput == nullptr)
        {
            return;
        }
        auto equationTextBox = dynamic_cast<EquationTextBox ^>(equationInput);
        if (equationTextBox != nullptr)
        {
            equationTextBox->FocusTextBox();
        }
    }
}

void EquationInputArea::EquationTextBox_RemoveButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    auto eq = GetViewModelFromEquationTextBox(sender);
    unsigned int index;

    if (Equations->IndexOf(eq, &index))
    {
        // Prevent removing the last equation
        if (index == Equations->Size - 1)
        {
            return;
        }

        Equations->RemoveAt(index);

        auto narratorNotifier = ref new NarratorNotifier();
        auto announcement =
            CalculatorAnnouncement::GetFunctionRemovedAnnouncement(AppResourceProvider::GetInstance()->GetResourceString(L"FunctionRemovedAnnouncement"));
        narratorNotifier->Announce(announcement);

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

        // Focus the next equation after the one we just removed. There should always be at least one ghost equation,
        // but check to make sure that there is an equation we can focus in the index where we just removed an equation.
        if (index < Equations->Size)
        {
            FocusEquationTextBox(Equations->GetAt(index));
        }
    }
}

void EquationInputArea::EquationTextBox_KeyGraphFeaturesButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    KeyGraphFeaturesRequested(this, GetViewModelFromEquationTextBox(sender));
}

void EquationInputArea::EquationTextBox_EquationButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    auto eq = GetViewModelFromEquationTextBox(sender);
    eq->IsLineEnabled = !eq->IsLineEnabled;

    TraceLogger::GetInstance()->LogShowHideButtonClicked(eq->IsLineEnabled ? false : true);
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
            auto container = static_cast<UIElement ^>(EquationInputList->ContainerFromIndex(index));
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
            auto container = static_cast<UIElement ^>(EquationInputList->ContainerFromIndex(index));
            if (container != nullptr)
            {
                container->StartBringIntoView();
            }
        }
    }
}

void EquationInputArea::OnHighContrastChanged(AccessibilitySettings ^ sender, Object ^ args)
{
    ReloadAvailableColors(sender->HighContrast, true);
    m_isHighContrast = sender->HighContrast;
}

void EquationInputArea::OnColorValuesChanged(Windows::UI::ViewManagement::UISettings ^ sender, Platform::Object ^ args)
{
    WeakReference weakThis(this);
    this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([weakThis]() {
                                   auto refThis = weakThis.Resolve<EquationInputArea>();
                                   if (refThis != nullptr && refThis->m_isHighContrast == refThis->m_accessibilitySettings->HighContrast)
                                   {
                                       refThis->ReloadAvailableColors(false, false);
                                   }
                               }));
}

void EquationInputArea::ReloadAvailableColors(bool isHighContrast, bool reassignColors)
{
    m_AvailableColors->Clear();
    if (isHighContrast)
    {
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush1")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush2")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush3")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup(L"EquationBrush4")));
    }

    // If this is not high contrast, we have all 16 colors, otherwise we will restrict this to a subset of high contrast colors
    else
    {
        Object ^ themeDictionaryName = L"Light";
        if (IsMatchAppTheme && Application::Current->RequestedTheme == ApplicationTheme::Dark)
        {
            themeDictionaryName = L"Default";
        }
        auto themeDictionary = static_cast<ResourceDictionary ^>(Application::Current->Resources->ThemeDictionaries->Lookup(themeDictionaryName));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush1")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush2")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush3")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush4")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush5")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush6")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush7")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush8")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush9")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush10")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush11")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush12")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush13")));
        m_AvailableColors->Append(safe_cast<SolidColorBrush ^>(themeDictionary->Lookup(L"EquationBrush14")));
    }

    // If there are no equations to reload, quit early
    if (Equations == nullptr || Equations->Size == 0)
    {
        return;
    }

    // Reassign colors for each equation
    if (reassignColors)
    {
        m_lastLineColorIndex = -1;
    }

    for (auto equationViewModel : Equations)
    {
        if (reassignColors)
        {
            m_lastLineColorIndex = (m_lastLineColorIndex + 1) % AvailableColors->Size;
            equationViewModel->LineColorIndex = m_lastLineColorIndex;
        }
        equationViewModel->LineColor = AvailableColors->GetAt(equationViewModel->LineColorIndex)->Color;
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
        TraceLogger::GetInstance()->LogVariableChanged(L"ValueTextBox", variableViewModel->Name);
    }
    else if (sender->Name == "MinTextBox")
    {
        val = validateDouble(sender->Text, variableViewModel->Min);

        variableViewModel->Min = val;
        TraceLogger::GetInstance()->LogVariableSettingsChanged(L"MinTextBox");
    }
    else if (sender->Name == "MaxTextBox")
    {
        val = validateDouble(sender->Text, variableViewModel->Max);
        variableViewModel->Max = val;
        TraceLogger::GetInstance()->LogVariableSettingsChanged(L"MaxTextBox");
    }
    else if (sender->Name == "StepTextBox")
    {
        val = validateDouble(sender->Text, variableViewModel->Step);

        // Don't allow a value less than or equal to 0 as the step
        if (val <= 0)
        {
            val = variableViewModel->Step;
        }

        variableViewModel->Step = val;
        TraceLogger::GetInstance()->LogVariableSettingsChanged(L"StepTextBox");
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

bool EquationInputArea::ManageEditVariablesButtonLoaded(unsigned int numberOfVariables)
{
    return numberOfVariables != 0;
}

String ^ EquationInputArea::GetChevronIcon(bool isCollapsed)
{
    return isCollapsed ? L"\uE70E" : L"\uE70D";
}

void EquationInputArea::VariableAreaTapped(Object ^ sender, TappedRoutedEventArgs ^ e)
{
    ToggleVariableArea(static_cast<VariableViewModel ^>(static_cast<FrameworkElement ^>(sender)->DataContext));
}

void EquationInputArea::VariableAreaButtonTapped(Object ^ sender, TappedRoutedEventArgs ^ e)
{
    e->Handled = true;
}

void EquationInputArea::EquationTextBox_EquationFormatRequested(Object ^ sender, MathRichEditBoxFormatRequest ^ e)
{
    EquationFormatRequested(sender, e);
}

void EquationInputArea::VariableAreaClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    ToggleVariableArea(static_cast<VariableViewModel ^>(static_cast<ToggleButton ^>(sender)->DataContext));
}

void EquationInputArea::ToggleVariableArea(VariableViewModel ^ selectedVariableViewModel)
{
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

void EquationInputArea::Slider_ValueChanged(Object ^ sender, RangeBaseValueChangedEventArgs ^ e)
{
    if (variableSliders == nullptr)
    {
        variableSliders = ref new Map<String ^, DispatcherTimerDelayer ^>();
    }

    auto slider = static_cast<Slider ^>(sender);

    // The slider value updates when the user uses the TextBox to change the variable value.
    // Check the focus state so that we don't trigger the event when the user used the textbox to change the variable value.
    if (slider->FocusState == Windows::UI::Xaml::FocusState::Unfocused)
    {
        return;
    }

    auto variableVM = static_cast<VariableViewModel ^>(slider->DataContext);
    if (variableVM == nullptr)
    {
        return;
    }

    auto name = variableVM->Name;

    if (!variableSliders->HasKey(name))
    {
        TimeSpan timeSpan;
        timeSpan.Duration = 10000000; // The duration is 1 second. TimeSpan durations are expressed in 100 nanosecond units.
        DispatcherTimerDelayer ^ delayer = ref new DispatcherTimerDelayer(timeSpan);
        delayer->Action += ref new EventHandler<Platform::Object ^>([this, name](Platform::Object ^ sender, Platform::Object ^ e) {
            TraceLogger::GetInstance()->LogVariableChanged("Slider", name);
            variableSliders->Remove(name);
        });
        delayer->Start();
        variableSliders->Insert(name, delayer);
    }

    else
    {
        auto delayer = variableSliders->Lookup(name);
        delayer->ResetAndStart();
    }
}

EquationViewModel ^ EquationInputArea::GetViewModelFromEquationTextBox(Object ^ sender)
{
    auto tb = static_cast<EquationTextBox ^>(sender);
    if (tb == nullptr)
    {
        return nullptr;
    }

    auto eq = static_cast<EquationViewModel ^>(tb->DataContext);

    return eq;
}

 SolidColorBrush ^ EquationInputArea::GetForegroundColor(Color lineColor)
 {
     return Utils::GetContrastColor(lineColor);
 }
