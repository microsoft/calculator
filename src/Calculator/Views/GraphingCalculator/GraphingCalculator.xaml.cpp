
#include "pch.h"
#include "GraphingCalculator.xaml.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "Controls/CalculationResult.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace concurrency;
using namespace GraphControl;
using namespace Platform;
using namespace std;
using namespace std::chrono;
using namespace Utils;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;

constexpr auto sc_ViewModelPropertyName = L"ViewModel";

GraphingCalculator::GraphingCalculator()
{
    Equation::RegisterDependencyProperties();
    Grapher::RegisterDependencyProperties();
    InitializeComponent();
}

void GraphingCalculator::GraphingCalculator_DataContextChanged(FrameworkElement^ sender, DataContextChangedEventArgs^ args)
{
    ViewModel = dynamic_cast<GraphingCalculatorViewModel^>(args->NewValue);

    ViewModel->VariableUpdated += ref new EventHandler<VariableChangedEventArgs>(this, &CalculatorApp::GraphingCalculator::OnVariableChanged);
}

GraphingCalculatorViewModel^ GraphingCalculator::ViewModel::get()
{
    return m_viewModel;
}

void GraphingCalculator::ViewModel::set(GraphingCalculatorViewModel^ vm)
{
    if (m_viewModel != vm)
    {
        m_viewModel = vm;
        RaisePropertyChanged(StringReference(sc_ViewModelPropertyName));
    }
}

void GraphingCalculator::GraphVariablesUpdated(Object^, Object^)
{
    m_viewModel->UpdateVariables(GraphingControl->Variables);
}

void GraphingCalculator::OnVariableChanged(Platform::Object^ sender, VariableChangedEventArgs args)
{
    GraphingControl->SetVariable(args.variableName, args.newValue);
}


void GraphingCalculator::SubmitTextbox(TextBox^ sender)
{
    auto variableViewModel = static_cast<VariableViewModel^>(sender->DataContext);

    if (sender->Name == "ValueTextBox")
    {
        variableViewModel->SetValue(validateDouble(sender->Text, variableViewModel->Value));
    }
    else if (sender->Name == "MinTextBox")
    {
        variableViewModel->Min = validateDouble(sender->Text, variableViewModel->Min);
    }
    else if (sender->Name == "MaxTextBox")
    {
        variableViewModel->Step = validateDouble(sender->Text, variableViewModel->Step);
    }
    else if (sender->Name == "StepTextBox")
    {
        variableViewModel->Max = validateDouble(sender->Text, variableViewModel->Max);
    }
}

void GraphingCalculator::TextBoxLosingFocus(TextBox^ sender, LosingFocusEventArgs^)
{
    SubmitTextbox(sender);
}


void GraphingCalculator::TextBoxKeyDown(TextBox^ sender, KeyRoutedEventArgs^ e)
{
    if (e->Key == ::VirtualKey::Enter)
    {
        SubmitTextbox(sender);
    }
}

double GraphingCalculator::validateDouble(String^ value, double defaultValue)
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

void GraphingCalculator::TextBoxGotFocus(TextBox^ sender, RoutedEventArgs^ e)
{
    sender->SelectAll();
}

void GraphingCalculator::ZoomInButtonClick(Object ^ /* parameter */)
{
    constexpr double scrollDamper = 0.15;
    double scale = 1.0 + (abs(zoomButtonDelta) / WHEEL_DELTA) * scrollDamper;
    scale = 1.0 / scale;

    GraphingControl->ScaleRange(0, 0, scale);
}

void GraphingCalculator::ZoomOutButtonClick(Object ^ /* parameter */)
{
    constexpr double scrollDamper = 0.15;
    double scale = 1.0 + (abs(zoomButtonDelta) / WHEEL_DELTA) * scrollDamper;

    GraphingControl->ScaleRange(0, 0, scale);
}

void GraphingCalculator::ZoomResetButtonClick(Object ^ /* parameter */)
{
    GraphingControl->ResetGrid();
}
