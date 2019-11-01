#include "pch.h"

#include "GraphingSettings.xaml.h"
#include <CalcViewModel\Common\AppResourceProvider.cpp>

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

    auto resLoader = AppResourceProvider::GetInstance();

    x_MinHeading->Text = resLoader.GetResourceString(L"x-min");
    x_MaxHeading->Text = resLoader.GetResourceString(L"x-max");
    y_MinHeading->Text = resLoader.GetResourceString(L"y-min");
    y_MaxHeading->Text = resLoader.GetResourceString(L"y-max");

    GridHeading->Text = resLoader.GetResourceString(L"GridHeading");
    UnitsHeading->Text = resLoader.GetResourceString(L"UnitsHeading");

    Radians->Content = resLoader.GetResourceString(L"TrigModeRadians");
    Degrees->Content = resLoader.GetResourceString(L"TrigModeDegrees");
    Gradians->Content = resLoader.GetResourceString(L"TrigModeGradians");
}

void GraphingSettings::GraphingCalculator_DataContextChanged(FrameworkElement ^ sender, DataContextChangedEventArgs ^ args)
{
    // The graph control we are working with
    m_ParentGrapher = (GraphControl::Grapher ^) args->NewValue;
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
}

void GraphingSettings::OnTextChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs ^ e)
{
    //UpdateExtents(sender);
}

void GraphingSettings::UpdateExtents(Platform::Object ^ sender)
{
    FrameworkElement ^ fe = (FrameworkElement ^) sender;
    FrameworkElement ^ currentFocus = (FrameworkElement ^) FocusManager::GetFocusedElement();

    bool thisControlHasFocus = (fe == currentFocus);
    TextBox ^ tb = reinterpret_cast<TextBox ^>(sender);
    bool widthChanged = false;
    bool heightChanged = false;

    try
    {
        double newVal = std::stod(tb->Text->Begin());
        if (tb->Name == "x_min" && newVal != m_xMin)
        {
            widthChanged = true;
            m_xMin = newVal;
            m_ParentGrapher->XAxisMin = m_xMin;
        }

        if (tb->Name == "x_max" && newVal != m_xMax)
        {
            widthChanged = true;
            m_xMax = newVal;
            m_ParentGrapher->XAxisMax = m_xMax;
        }

        if (tb->Name == "y_min" && newVal != m_yMin)
        {
            heightChanged = true;
            m_yMin = newVal;
            m_ParentGrapher->YAxisMin = m_yMin;
        }

        if (tb->Name == "y_max" && newVal != m_yMax)
        {
            heightChanged = true;
            m_yMax = newVal;
            m_ParentGrapher->YAxisMax = m_yMax;
        }

        // Only the focused element get's to trigger the recompute

        if (thisControlHasFocus)
        {
            if (PreserveAspectRatio->IsChecked)
            {
                if (widthChanged)
                {
                    m_width = m_xMax - m_xMin;
                    m_height = m_width / m_currentAspectRatio;
                    double bounds = m_height / 2;
                    m_yMin = -bounds;
                    m_yMax = bounds;
                    m_ParentGrapher->YAxisMin = m_yMin;
                    m_ParentGrapher->YAxisMax = m_yMax;
                }

                if (heightChanged)
                {
                    m_height = m_yMax - m_yMin;
                    m_width = m_height * m_currentAspectRatio;
                    double bounds = m_width / 2;
                    m_xMin = -bounds;
                    m_xMax = bounds;
                    m_ParentGrapher->XAxisMin = m_xMin;
                    m_ParentGrapher->XAxisMax = m_xMax;
                }

                if (widthChanged || heightChanged)
                {
                    x_min->Text = m_xMin.ToString();
                    x_max->Text = m_xMax.ToString();

                    y_min->Text = m_yMin.ToString();
                    y_max->Text = m_yMax.ToString();

                    m_ParentGrapher->SetDisplayRanges(m_xMin, m_xMax, m_yMin, m_yMax);
                }
            }
        }
    }
    catch (const std::exception&)
    {
    }
}

void CalculatorApp::Controls::GraphingSettings::OnLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    m_ParentGrapher->GetDisplayRanges(&m_xMin, &m_xMax, &m_yMin, &m_yMax);

    // Now we can load our UX
    x_min->Text = m_xMin.ToString();
    x_max->Text = m_xMax.ToString();

    y_min->Text = m_yMin.ToString();
    y_max->Text = m_yMax.ToString();

    switch ((Graphing::EvalTrigUnitMode)m_ParentGrapher->TrigUnitMode)
    {
    case Graphing::EvalTrigUnitMode::Degrees:
    {
        Degrees->IsChecked = true;
    }
    break;
    case Graphing::EvalTrigUnitMode::Radians:
    {
        Radians->IsChecked = true;
    }
    break;
    case Graphing::EvalTrigUnitMode::Grads:
    {
        Gradians->IsChecked = true;
    }
    break;

    case Graphing::EvalTrigUnitMode::Invalid:
    {
        OutputDebugString(L"GraphingSettings::GraphingCalculator_DataContextChanged Unsupported TrigUnitMode\r\n");
        m_ParentGrapher->TrigUnitMode = (int)Graphing::EvalTrigUnitMode::Radians;
    }
    }

    m_width = m_xMax - m_xMin;
    m_height = m_yMax - m_yMin;
    m_currentAspectRatio = m_height / m_width;
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
