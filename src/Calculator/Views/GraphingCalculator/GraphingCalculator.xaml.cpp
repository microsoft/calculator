
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


void GraphingCalculator::BeforeTextChanging(TextBox^ sender, TextBoxBeforeTextChangingEventArgs^ args)
{
    if (args->NewText->IsEmpty() || args->NewText == "-")
    {
        return;
    }

    try
    {
        stod(args->NewText->Data());
    }
    catch (...)
    {
        args->Cancel = true;
    }
}


// Move this into view model, make it take into account min/max values also
void GraphingCalculator::ValidateTextBox(TextBox^ sender)
{
    if (sender->Text->IsEmpty())
    {
        sender->Text = "1";
    }

    try
    {
        stod(sender->Text->Data());
    }
    catch (...)
    {
        sender->Text = "1";
    }
}


void GraphingCalculator::TextBoxLosingFocus(TextBox^ sender, LosingFocusEventArgs^ args)
{
    ValidateTextBox(sender);
}


void GraphingCalculator::TextBoxKeyDown(TextBox^ sender, KeyRoutedEventArgs^ e)
{
    if (e->Key == ::VirtualKey::Enter)
    {
        ValidateTextBox(sender);
    }
}
