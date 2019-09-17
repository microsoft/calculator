#include "pch.h"

#include "GraphingSettings.xaml.h"

using namespace Graphing;

using namespace CalculatorApp;
using namespace CalculatorApp::Controls;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

GraphingSettings::GraphingSettings()
{
    InitializeComponent();
}

void GraphingSettings::GraphingCalculator_DataContextChanged(FrameworkElement ^ sender, DataContextChangedEventArgs ^ args)
{
    auto t = args->NewValue;
    auto x = (CalculatorApp::ViewModel::ViewStateViewModel ^) t;
    ViewModel = dynamic_cast<CalculatorApp::ViewModel::ViewStateViewModel ^>(t);

    // ViewModel->VariableUpdated += ref new EventHandler<PropertyChangedEventArgs>(this, &GraphingSettings::OnVariableChanged);
}

void GraphingSettings::TrigUnitModeClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (Degrees->IsChecked->Value == true)
    {
        //        GraphingControl->SetTrigUnitMode((int)Graphing::EvalTrigUnitMode::Degrees);
    }

    if (Radians->IsChecked->Value == true)
    {
        //      GraphingControl->SetTrigUnitMode((int)Graphing::EvalTrigUnitMode::Radians);
    }
    // m_solver->EvalOptions().SetTrigUnitMode(z);
}

void CalculatorApp::Controls::GraphingSettings::OnTextChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs ^ e)
{
    TextBox ^ tb = reinterpret_cast<TextBox ^>(sender);

    if (tb->Name == "x_min")
    {
        auto val = tb->Text;

        int x = std::stoi(val->Begin());

        ViewModel->XMin = x;
    }
    if (tb->Name == "x_max")
    {
        ViewModel->XMax = std::stoi(tb->Text->Begin());
    }
    if (tb->Name == "y_min")
    {
        ViewModel->YMin = std::stoi(tb->Text->Begin());
    }
    if (tb->Name == "y_max")
    {
        ViewModel->YMax = std::stoi(tb->Text->Begin());
    }
}
