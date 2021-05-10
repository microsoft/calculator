// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DirectX/RenderMain.h"
#include "Models/Equation.h"
#include "Models/EquationCollection.h"
#include "Models/Variable.h"
#include "Utils.h"
#include "IGraphAnalyzer.h"
#include "IMathSolver.h"
#include "Common.h"
#include "Models/KeyGraphFeaturesInfo.h"
#include <ppltasks.h>
#include "Logger/TraceLogger.h"

namespace GraphControl
{
public
    delegate void TracingChangedEventHandler(bool newValue);

public
    delegate void TracingValueChangedEventHandler(double xPointValue, double yPointValue);
public
    delegate void PointerValueChangedEventHandler(Windows::Foundation::Point value);

public enum class GraphViewChangedReason
{
    Manipulation,
    Reset
};  

    [Windows::UI::Xaml::Markup::ContentPropertyAttribute(Name = L"Equations")] public ref class Grapher sealed
        : public Windows::UI::Xaml::Controls::Control,
          public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        event TracingValueChangedEventHandler ^ TracingValueChangedEvent;
        event PointerValueChangedEventHandler ^ PointerValueChangedEvent;
        event TracingChangedEventHandler ^ TracingChangedEvent;
        event Windows::Foundation::EventHandler<GraphViewChangedReason> ^ GraphViewChangedEvent;
        event Windows::UI::Xaml::RoutedEventHandler ^ GraphPlottedEvent;
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

    public:
        Grapher();

        DEPENDENCY_PROPERTY_OWNER(Grapher);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(bool, ForceProportionalAxes, true);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(bool, UseCommaDecimalSeperator, false);
        DEPENDENCY_PROPERTY_WITH_DEFAULT(
            SINGLE_ARG(Windows::Foundation::Collections::IObservableMap<Platform::String ^, GraphControl::Variable ^> ^),
            Variables,
            SINGLE_ARG(ref new Platform::Collections::Map<Platform::String ^, GraphControl::Variable ^>()));
        DEPENDENCY_PROPERTY_R_WITH_DEFAULT_AND_CALLBACK(GraphControl::EquationCollection ^, Equations, nullptr);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(Windows::UI::Color, AxesColor, Windows::UI::Colors::Transparent);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(Windows::UI::Color, GraphBackground, Windows::UI::Colors::Transparent);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(Windows::UI::Color, GridLinesColor, Windows::UI::Colors::Transparent);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, LineWidth, 2.0);
        DEPENDENCY_PROPERTY_WITH_DEFAULT(bool, IsKeepCurrentView, false);

        // Pass active tracing turned on or off down to the renderer
        property bool ActiveTracing
        {
            bool get()
            {
                return m_renderMain != nullptr && m_renderMain->ActiveTracing;
            }

            void set(bool value)
            {
                if (m_renderMain != nullptr && m_renderMain->ActiveTracing != value)
                {
                    m_renderMain->ActiveTracing = value;
                    UpdateTracingChanged();
                    PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"ActiveTracing"));
                }
            }
        }

        void ZoomFromCenter(double scale);
        void ResetGrid();

        property Windows::Foundation::Point TraceLocation
        {
            Windows::Foundation::Point get()
            {
                return m_renderMain->TraceLocation;
            }
        }

        property Windows::Foundation::Point ActiveTraceCursorPosition
        {
            Windows::Foundation::Point get()
            {
                return m_renderMain->ActiveTraceCursorPosition;
            }

            void set(Windows::Foundation::Point newValue)
            {
                if (m_renderMain->ActiveTraceCursorPosition != newValue)
                {
                    m_renderMain->ActiveTraceCursorPosition = newValue;
                    UpdateTracingChanged();
                }
            }
        }

        event Windows::Foundation::EventHandler<Windows::Foundation::Collections::IMap<Platform::String ^, Variable ^> ^> ^ VariablesUpdated;
        void SetVariable(Platform::String ^ variableName, double newValue);
        Platform::String ^ ConvertToLinear(Platform::String ^ mmlString);
        Platform::String ^ FormatMathML(Platform::String ^ mmlString);

        /// <summary>
        /// Draw the graph. Call this method if you add or modify an equation.
        /// </summary>
        /// <param name="keepCurrentView">Force the graph control to not pan or zoom to adapt the view.</param>
        void PlotGraph(bool keepCurrentView);

        GraphControl::KeyGraphFeaturesInfo ^ AnalyzeEquation(GraphControl::Equation ^ equation);

        // We can't use the EvalTrigUnitMode enum directly in as the property type because it comes from another module which doesn't expose
        // it as a public enum class.  So the compiler doesn't recognize it as a valid type for the ABI boundary.
        property int TrigUnitMode
        {
            void set(int value)
            {
                if (value != (int)m_solver->EvalOptions().GetTrigUnitMode())
                {
                    m_solver->EvalOptions().SetTrigUnitMode((Graphing::EvalTrigUnitMode)value);
                    m_trigUnitsChanged = true;
                    PlotGraph(true);
                }
            }

            int get()
            {
                return (int)m_solver->EvalOptions().GetTrigUnitMode();
            }
        }

        property double XAxisMin
        {
            double get()
            {
                return m_graph->GetOptions().GetDefaultXRange().first;
            }
            void set(double value)
            {
                std::pair<double, double> newValue(value, XAxisMax);
                if (m_graph != nullptr)
                {
                    m_graph->GetOptions().SetDefaultXRange(newValue);
                    if (m_renderMain != nullptr)
                    {
                        m_renderMain->RunRenderPass();
                    }
                }
            }
        }

        property double XAxisMax
        {
            double get()
            {
                return m_graph->GetOptions().GetDefaultXRange().second;
            }
            void set(double value)
            {
                std::pair<double, double> newValue(XAxisMin, value);
                if (m_graph != nullptr)
                {
                    m_graph->GetOptions().SetDefaultXRange(newValue);
                    if (m_renderMain != nullptr)
                    {
                        m_renderMain->RunRenderPass();
                    }
                }
            }
        }

        property double YAxisMin
        {
            double get()
            {
                return m_graph->GetOptions().GetDefaultXRange().first;
            }
            void set(double value)
            {
                std::pair<double, double> newValue(value, YAxisMax);
                if (m_graph != nullptr)
                {
                    m_graph->GetOptions().SetDefaultYRange(newValue);
                    if (m_renderMain != nullptr)
                    {
                        m_renderMain->RunRenderPass();
                    }
                }
            }
        }

        property double YAxisMax
        {
            double get()
            {
                return m_graph->GetOptions().GetDefaultXRange().second;
            }
            void set(double value)
            {
                std::pair<double, double> newValue(YAxisMin, value);
                if (m_graph != nullptr)
                {
                    m_graph->GetOptions().SetDefaultYRange(newValue);
                    if (m_renderMain != nullptr)
                    {
                        m_renderMain->RunRenderPass();
                    }
                }
            }
        }

        void GetDisplayRanges(double* xMin, double* xMax, double* yMin, double* yMax)
        {
            try
            {
                if (m_graph != nullptr && m_renderMain != nullptr)
                {
                    if (auto render = m_graph->GetRenderer())
                    {
                        Concurrency::critical_section::scoped_lock lock(m_renderMain->GetCriticalSection());
                        render->GetDisplayRanges(*xMin, *xMax, *yMin, *yMax);
                    }
                }
            }
            catch (const std::exception&)
            {
                OutputDebugString(L"GetDisplayRanges failed\r\n");
            }
        }

        void SetDisplayRanges(double xMin, double xMax, double yMin, double yMax)
        {
            try
            {
                if (auto render = m_graph->GetRenderer())
                {
                    render->SetDisplayRanges(xMin, xMax, yMin, yMax);
                    m_rangeUpdatedBySettings = true;
                    if (m_renderMain)
                    {
                        m_renderMain->RunRenderPass();
                        GraphViewChangedEvent(this, GraphViewChangedReason::Manipulation);
                    }
                }
            }
            catch (const std::exception&)
            {
                OutputDebugString(L"SetDisplayRanges failed\r\n");
            }
        }

    protected:
#pragma region Control Overrides
        void OnApplyTemplate() override;

        void OnPointerEntered(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
        void OnPointerMoved(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
        void OnPointerExited(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
        void OnPointerWheelChanged(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
        void OnPointerPressed(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
        void OnPointerReleased(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
        void OnPointerCanceled(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
        void OnManipulationDelta(Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs ^ e) override;
#pragma endregion

    private:
        void OnForceProportionalAxesPropertyChanged(bool oldValue, bool newValue);
        void OnUseCommaDecimalSeperatorPropertyChanged(bool oldValue, bool newValue);
        void OnEquationsPropertyChanged(EquationCollection ^ oldValue, EquationCollection ^ newValue);
        void OnAxesColorPropertyChanged(Windows::UI::Color oldValue, Windows::UI::Color newValue);
        void OnGraphBackgroundPropertyChanged(Windows::UI::Color oldValue, Windows::UI::Color newValue);
        void OnGridLinesColorPropertyChanged(Windows::UI::Color /*oldValue*/, Windows::UI::Color newValue);
        void OnLineWidthPropertyChanged(double oldValue, double newValue);
        void OnEquationChanged(Equation ^ equation);
        void OnEquationStyleChanged(Equation ^ equation);
        void OnEquationLineEnabledChanged(Equation ^ equation);
        concurrency::task<bool> TryUpdateGraph(bool keepCurrentView);
        concurrency::task<void> TryPlotGraph(bool keepCurrentView, bool shouldRetry);
        void UpdateGraphOptions(Graphing::IGraphingOptions& options, const std::vector<Equation ^>& validEqs);
        std::vector<Equation ^> GetGraphableEquations();
        void SetGraphArgs(std::shared_ptr<Graphing::IGraph> graph);
        std::shared_ptr<Graphing::IGraph> GetGraph(GraphControl::Equation ^ equation);
        void UpdateVariables();

        void ScaleRange(double centerX, double centerY, double scale);

        void OnCoreKeyDown(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ e);
        void OnCoreKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ e);

        void UpdateTracingChanged();
        void HandleTracingMovementTick(Object ^ sender, Object ^ e);
        void HandleKey(bool keyDown, Windows::System::VirtualKey key);

        void SetEquationsAsValid();
        void SetEquationErrors();
        std::optional<std::vector<std::shared_ptr<Graphing::IEquation>>> TryInitializeGraph(bool keepCurrentView, _In_ const Graphing::IExpression* graphingExp = nullptr);


    private:
        DX::RenderMain ^ m_renderMain = nullptr;

        static Windows::UI::Xaml::DependencyProperty ^ s_equationTemplateProperty;

        static Windows::UI::Xaml::DependencyProperty ^ s_equationsSourceProperty;
        Windows::Foundation::EventRegistrationToken m_tokenDataSourceChanged;

        static Windows::UI::Xaml::DependencyProperty ^ s_equationsProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_variablesProperty;
        Windows::Foundation::EventRegistrationToken m_tokenEquationsChanged;
        Windows::Foundation::EventRegistrationToken m_tokenEquationStyleChanged;
        Windows::Foundation::EventRegistrationToken m_tokenEquationChanged;
        Windows::Foundation::EventRegistrationToken m_tokenEquationLineEnabledChanged;

        Windows::Foundation::EventRegistrationToken m_tokenBackgroundColorChanged;

        const std::unique_ptr<Graphing::IMathSolver> m_solver;
        const std::shared_ptr<Graphing::IGraph> m_graph;
        bool m_calculatedForceProportional = false;
        bool m_tracingTracking;
        bool m_trigUnitsChanged;
        enum KeysPressedSlots
        {
            Left,
            Right,
            Down,
            Up,
            Accelerator
        };

        bool m_KeysPressed[5];
        bool m_Moving;
        Windows::UI::Xaml::DispatcherTimer ^ m_TracingTrackingTimer;
        Windows::UI::Core::CoreCursor ^ m_cachedCursor;
        int m_errorType;
        int m_errorCode;
        bool m_resetUsingInitialDisplayRange;
        bool m_rangeUpdatedBySettings;
        double m_initialDisplayRangeXMin;
        double m_initialDisplayRangeXMax;
        double m_initialDisplayRangeYMin;
        double m_initialDisplayRangeYMax;

    public:
        Windows::Storage::Streams::RandomAccessStreamReference ^ GetGraphBitmapStream();
    };
}
