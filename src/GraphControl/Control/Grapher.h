// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DirectX/RenderMain.h"
#include "../Models/Equation.h"
#include "../Models/EquationCollection.h"
#include "../Utils.h"
#include "IGraphAnalyzer.h"
#include "IMathSolver.h"
#include "Common.h"
#include "Models/KeyGraphFeaturesInfo.h"
#include <ppltasks.h>

namespace GraphControl
{
public
    delegate void TracingChangedEventHandler(bool newValue);

public
    delegate void TracingValueChangedEventHandler(Windows::Foundation::Point value);

    [Windows::UI::Xaml::Markup::ContentPropertyAttribute(Name = L"Equations")] public ref class Grapher sealed : public Windows::UI::Xaml::Controls::Control, public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        event TracingValueChangedEventHandler ^ TracingValueChangedEvent;
        event TracingChangedEventHandler ^ TracingChangedEvent;
        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

    public:
        Grapher();

        DEPENDENCY_PROPERTY_OWNER(Grapher);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(bool, ForceProportionalAxes, true);
        DEPENDENCY_PROPERTY_WITH_DEFAULT(
            SINGLE_ARG(Windows::Foundation::Collections::IObservableMap<Platform::String ^, double> ^),
            Variables,
            SINGLE_ARG(ref new Platform::Collections::Map<Platform::String ^, double>()));
        DEPENDENCY_PROPERTY_R_WITH_DEFAULT_AND_CALLBACK(GraphControl::EquationCollection ^, Equations, nullptr);
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

        property Windows::Foundation::Point TraceValue
        {
            Windows::Foundation::Point get()
            {
                return m_renderMain->TraceValue;
            }
        }

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

        event Windows::Foundation::EventHandler<Windows::Foundation::Collections::IMap<Platform::String ^, double> ^> ^ VariablesUpdated;
        void SetVariable(Platform::String ^ variableName, double newValue);
        Platform::String ^ ConvertToLinear(Platform::String ^ mmlString);
        void PlotGraph();
        void AnalyzeEquation(GraphControl::Equation ^ equation);
       // We can't use the EValTrigUnitMode enum directly in as the property type because it comes from another module which doesn't expose
        // it as a public enum class.  So the compiler doesn't recognize it as a valid type for the ABI boundary.
        property int TrigUnitMode
        {
            void set(int value)
            {
                if (value != (int)m_solver->EvalOptions().GetTrigUnitMode())
                {
                    m_solver->EvalOptions().SetTrigUnitMode((Graphing::EvalTrigUnitMode)value);
                    UpdateGraph();
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
                m_graph->GetOptions().SetDefaultXRange(newValue);
                m_renderMain->RunRenderPass();
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
                std::pair<double, double> newValue(XAxisMax, value);
                m_graph->GetOptions().SetDefaultXRange(newValue);
                m_renderMain->RunRenderPass();
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
                m_graph->GetOptions().SetDefaultYRange(newValue);
                m_renderMain->RunRenderPass();
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
                std::pair<double, double> newValue(YAxisMax, value);
                m_graph->GetOptions().SetDefaultYRange(newValue);
                m_renderMain->RunRenderPass();
            }
        }

        void GetDisplayRanges(double* xMin, double* xMax, double* yMin, double* yMax)
        {
            try
            {
                m_graph->GetRenderer()->GetDisplayRanges(*xMin, *xMax, *yMin, *yMax);
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
                m_graph->GetRenderer()->SetDisplayRanges(xMin, xMax, yMin, yMax);
            }
            catch (const std::exception&)
            {
                OutputDebugString(L"SetDisplayRanges failed\r\n");
            }
        }
        GraphControl::KeyGraphFeaturesInfo ^ AnalyzeEquation(GraphControl::Equation ^ equation);

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
        void OnLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ args);
        void OnUnloaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ args);

        void OnForceProportionalAxesPropertyChanged(bool oldValue, bool newValue);
        void OnEquationsPropertyChanged(EquationCollection ^ oldValue, EquationCollection ^ newValue);
        void OnDependencyPropertyChanged(Windows::UI::Xaml::DependencyObject ^ obj, Windows::UI::Xaml::DependencyProperty ^ p);

        void OnEquationChanged(Equation ^ equation);
        void OnEquationStyleChanged(Equation ^ equation);
        void OnEquationLineEnabledChanged(Equation ^ equation);
        bool TryUpdateGraph();
        void TryPlotGraph(bool shouldRetry);
        void UpdateGraphOptions(Graphing::IGraphingOptions& options, const std::vector<Equation ^>& validEqs);
        std::vector<Equation ^> GetGraphableEquations();
        void SetGraphArgs();
        std::shared_ptr<Graphing::IGraph> GetGraph(GraphControl::Equation ^ equation);
        void UpdateVariables();

        void OnBackgroundColorChanged(const Windows::UI::Color& color);

        void ScaleRange(double centerX, double centerY, double scale);

        void OnCoreKeyDown(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ e);
        void OnCoreKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ e);

        void UpdateTracingChanged();
        void HandleTracingMovementTick(Object ^ sender, Object ^ e);
        void HandleKey(bool keyDown, Windows::System::VirtualKey key);

        void SetEquationsAsValid();
        void SetEquationErrors();

        Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^ ConvertWStringVector(std::vector<std::wstring> inVector);
        Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::String ^> ^ ConvertWStringIntMap(std::map<std::wstring, int> inMap);

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

        bool m_tracingTracking;
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

    public:
        Windows::Storage::Streams::RandomAccessStreamReference ^ GetGraphBitmapStream();
    };
}
