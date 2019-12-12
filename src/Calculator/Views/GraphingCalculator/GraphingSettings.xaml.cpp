#include "pch.h"

#include "GraphingSettings.xaml.h"
#include "CalcViewModel\Common\AppResourceProvider.cpp"

using namespace Graphing;

using namespace CalculatorApp;

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

GraphingSettings::GraphingSettings()
{
    InitializeComponent();
}

void GraphingSettings::GraphingCalculator_DataContextChanged(FrameworkElement ^ sender, DataContextChangedEventArgs ^ args)
{
    // The graph control we are working with
    m_ParentGrapher = dynamic_cast<GraphControl::Grapher ^>(args->NewValue);
}

void GraphingSettings::TrigUnitModeClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (Degrees->IsChecked->Value == true)
    {
        m_ParentGrapher->TrigUnitMode = (int)Graphing::EvalTrigUnitMode::Degrees;
    }

    if (Radians->IsChecked->Value == true)
    {
        m_ParentGrapher->TrigUnitMode = (int)Graphing::EvalTrigUnitMode::Radians;
    }

    if (Gradians->IsChecked->Value == true)
    {
        m_ParentGrapher->TrigUnitMode = (int)Graphing::EvalTrigUnitMode::Grads;
    }
}

void GraphingSettings::UpdateExtents(Platform::Object ^ sender)
{
    Control ^ senderControl = static_cast<Control ^>(sender);

    TextBox ^ tb = static_cast<TextBox ^>(sender);

    double newVal = std::stod(tb->Text->Begin());
    if (tb->Name == "x_Min" && newVal != m_xMin)
    {
        m_xMin = newVal;
        m_ParentGrapher->XAxisMin = m_xMin;
    }

    if (tb->Name == "x_Max" && newVal != m_xMax)
    {
        m_xMax = newVal;
        m_ParentGrapher->XAxisMax = m_xMax;
    }

    if (tb->Name == "y_Min" && newVal != m_yMin)
    {
        m_yMin = newVal;
        m_ParentGrapher->YAxisMin = m_yMin;
    }

    if (tb->Name == "y_Max" && newVal != m_yMax)
    {
        m_yMax = newVal;
        m_ParentGrapher->YAxisMax = m_yMax;
    }

    if (m_yMin < m_yMax && m_xMin < m_xMax)
    {
        m_ParentGrapher->SetDisplayRanges(m_xMin, m_xMax, m_yMin, m_yMax);
        ErrorIcon->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    else
    {
        ErrorIcon->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}

void GraphingSettings::OnLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    m_ParentGrapher->GetDisplayRanges(&m_xMin, &m_xMax, &m_yMin, &m_yMax);

    // Now we can load our UX
    x_Min->Text = m_xMin.ToString();
    x_Max->Text = m_xMax.ToString();

    y_Min->Text = m_yMin.ToString();
    y_Max->Text = m_yMax.ToString();

    switch ((Graphing::EvalTrigUnitMode)m_ParentGrapher->TrigUnitMode)
    {
    case Graphing::EvalTrigUnitMode::Degrees:
    {
        Degrees->IsChecked = true;
        break;
    }
    case Graphing::EvalTrigUnitMode::Radians:
    {
        Radians->IsChecked = true;
        break;
    }
    case Graphing::EvalTrigUnitMode::Grads:
    {
        Gradians->IsChecked = true;
        break;
    }
    case Graphing::EvalTrigUnitMode::Invalid:
    {
        m_ParentGrapher->TrigUnitMode = (int)Graphing::EvalTrigUnitMode::Radians;
        break;
    }
    }
}

void GraphingSettings::OnLosingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args)
{
    UpdateExtents(sender);
}

void GraphingSettings::OnKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e)
{
    if (e->Key == Windows::System::VirtualKey::Enter)
    {
        UpdateExtents(sender);
    }
}
