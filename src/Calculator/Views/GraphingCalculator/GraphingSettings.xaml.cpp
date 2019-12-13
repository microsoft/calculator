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

    if (m_yMin < m_yMax)
    {
        m_yHasError = false;
        VisualStateManager::GoToState(y_Min, "Normal", false);
        VisualStateManager::GoToState(y_Max, "Normal", false);
    }
    if (m_xMin < m_xMax)
    {
        m_xHasError = false;
        VisualStateManager::GoToState(x_Min, "Normal", false);
        VisualStateManager::GoToState(x_Max, "Normal", false);
    }

    if (m_yMin >= m_yMax)
    {
        m_yHasError = true;
        VisualStateManager::GoToState(y_Min, "Error", false);
        VisualStateManager::GoToState(y_Max, "Error", false);
    }
    if (m_xMin >= m_xMax)
    {
        m_xHasError = true;
        VisualStateManager::GoToState(x_Min, "Error", false);
        VisualStateManager::GoToState(x_Max, "Error", false);
    }

    if (!m_yHasError && !m_xHasError)
    {
        m_ParentGrapher->SetDisplayRanges(m_xMin, m_xMax, m_yMin, m_yMax);
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
    String ^ state = "Normal";
    TextBox ^ tb = static_cast<TextBox ^>(sender);
    String ^ textBoxName = tb->Name;

    if ((textBoxName == "x_Min" || textBoxName == "x_Max") && m_xHasError)
    {
        state = "Error";
    }

    if ((textBoxName == "y_Min" || textBoxName == "y_Max") && m_yHasError)
    {
        state = "Error";
    }
    VisualStateManager::GoToState(this, state, false);

    UpdateExtents(sender);
}

void GraphingSettings::OnKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e)
{
    if (e->Key == Windows::System::VirtualKey::Enter)
    {
        UpdateExtents(sender);
    }
}

void CalculatorApp::GraphingSettings::OnGettingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::GettingFocusEventArgs ^ args)
{
    String ^ state = "Focused";
    TextBox ^ tb = static_cast<TextBox ^>(sender);
    String ^ textBoxName = tb->Name;

    if ((textBoxName == "x_Min" || textBoxName == "x_Max") && m_xHasError)
    {
        state = "FocusedError";
    }

    if ((textBoxName == "y_Min" || textBoxName == "y_Max") && m_yHasError)
    {
        state = "FocusedError";
    }

    VisualStateManager::GoToState(this, state, false);
}

void CalculatorApp::GraphingSettings::OnPointerEntered(Platform::Object ^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e)
{
    String ^ state = "PointerOver";
    TextBox ^ tb = static_cast<TextBox ^>(sender);
    String ^ textBoxName = tb->Name;

    if ((textBoxName == "x_Min" || textBoxName == "x_Max") && m_xHasError)
    {
        state = "PointerOverError";
    }

    if ((textBoxName == "y_Min" || textBoxName == "y_Max") && m_yHasError)
    {
        state = "PointerOverError";
    }

    VisualStateManager::GoToState(this, state, false);
}

void CalculatorApp::GraphingSettings::OnPointerExited(Platform::Object ^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e)
{
    String ^ state = "Normal";
    TextBox ^ tb = static_cast<TextBox ^>(sender);
    String ^ textBoxName = tb->Name;

    if ((textBoxName == "x_Min" || textBoxName == "x_Max") && m_xHasError)
    {
        state = "Error";
    }

    if ((textBoxName == "y_Min" || textBoxName == "y_Max") && m_yHasError)
    {
        state = "Error";
    }
    VisualStateManager::GoToState(this, state, false);
}
