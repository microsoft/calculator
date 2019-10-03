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

void CalculatorApp::Controls::GraphingSettings::OnTextChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs ^ e)
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

                    //m_ParentGrapher->UpdateGraph();
                    
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
    // Get our first value to see if they have been set or are using defaults (expressed as nan which we don't want to show)
    m_xMin = m_ParentGrapher->XAxisMin;
    if (isnan(m_xMin))
    {
        // No range has been determined yet so use the current extents.
        m_ParentGrapher->GetDisplayRanges(&m_xMin, &m_xMax, &m_yMin, &m_yMax);
    }
    else
    {
        // We have ranges set so use them
        m_xMax = m_ParentGrapher->XAxisMax;

        m_yMin = m_ParentGrapher->YAxisMin;
        m_yMax = m_ParentGrapher->YAxisMax;
    }

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
