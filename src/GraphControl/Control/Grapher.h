#pragma once

#include "InspectingDataSource.h"
#include "DirectX/RenderMain.h"
#include "Equation.h"
#include "EquationCollection.h"
#include "IMathSolver.h"
#include "Common.h"

namespace GraphControl
{
public
    delegate void TracingChangedEventHandler(bool newValue);

public
    delegate void TracingValueChangedEventHandler(Windows::Foundation::Point value);

    [Windows::UI::Xaml::Markup::ContentPropertyAttribute(Name = L"Equations")] public ref class Grapher sealed : public Windows::UI::Xaml::Controls::Control
    {
    public:
        event TracingValueChangedEventHandler ^ TracingValueChangedEvent;
        event TracingChangedEventHandler ^ TracingChangedEvent;

    public:
        Grapher();

        static void RegisterDependencyProperties();

#pragma region Windows::UI::Xaml::DataTemplate ^ EquationTemplate DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ EquationTemplateProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_equationTemplateProperty;
            }
        }

        property Windows::UI::Xaml::DataTemplate^ EquationTemplate
        {
            Windows::UI::Xaml::DataTemplate^ get()
            {
                return static_cast<Windows::UI::Xaml::DataTemplate^>(GetValue(s_equationTemplateProperty));
            }
            void set(Windows::UI::Xaml::DataTemplate^ value)
            {
                SetValue(s_equationTemplateProperty, value);
            }
        }
#pragma endregion

#pragma region Platform::Object ^ EquationsSource DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ EquationsSourceProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_equationsSourceProperty;
            }
        }

        property Platform::Object^ EquationsSource
        {
            Platform::Object^ get()
            {
                return GetValue(s_equationsSourceProperty);
            }
            void set(Platform::Object^ value)
            {
                SetValue(s_equationsSourceProperty, value);
            }
        }
#pragma endregion

#pragma region GraphControl::EquationCollection ^ Equations DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ EquationsProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_equationsProperty;
            }
        }

        property GraphControl::EquationCollection^ Equations
        {
            GraphControl::EquationCollection^ get()
            {
                return static_cast< GraphControl::EquationCollection^ >(GetValue(s_equationsProperty));
            }
        }
#pragma endregion

#pragma region Windows::Foundation::Collections::IObservableMap < Platform::String ^, double> ^ Variables DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ VariablesProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_variablesProperty;
            }
        }

        property Windows::Foundation::Collections::IObservableMap<Platform::String^, double>^ Variables
        {
            Windows::Foundation::Collections::IObservableMap<Platform::String^, double>^ get()
            {
                return static_cast<Windows::Foundation::Collections::IObservableMap<Platform::String^, double>^>(GetValue(s_variablesProperty));
            }

            void set(Windows::Foundation::Collections::IObservableMap<Platform::String^, double>^ value)
            {
                SetValue(s_variablesProperty, value);
            }
        }
#pragma endregion

#pragma region Windows::UI::Xaml::DataTemplate ^ ForceProportionalAxes DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ ForceProportionalAxesTemplateProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_forceProportionalAxesTemplateProperty;
            }
        }

        property bool ForceProportionalAxes
        {
            bool get()
            {
                return static_cast<bool>(GetValue(s_forceProportionalAxesTemplateProperty));
            }
            void set(bool value)
            {
                SetValue(s_forceProportionalAxesTemplateProperty, value);
            }
        }
#pragma endregion

        // Pass active tracing turned on or off down to the renderer
        property bool ActiveTracing
        {
            bool get()
            {
                return m_renderMain->ActiveTracing;
            }

            void set(bool value)
            {
                m_renderMain->ActiveTracing = value;
            }
        }

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

        static void OnCustomDependencyPropertyChanged(Windows::UI::Xaml::DependencyObject ^ obj, Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args);
        void OnDependencyPropertyChanged(Windows::UI::Xaml::DependencyObject ^ obj, Windows::UI::Xaml::DependencyProperty ^ p);

        void OnEquationTemplateChanged(Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args);

        void OnEquationsSourceChanged(Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args);
        void OnDataSourceChanged(GraphControl::InspectingDataSource ^ sender, GraphControl::DataSourceChangedEventArgs args);

        void OnEquationsChanged(Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args);
        void OnEquationsVectorChanged(
            Windows::Foundation::Collections::IObservableVector<GraphControl::Equation ^> ^ sender,
            Windows::Foundation::Collections::IVectorChangedEventArgs ^ event);
        void OnEquationChanged();

        void UpdateGraph();
        void UpdateGraphOptions(Graphing::IGraphingOptions& options, const std::vector<Equation ^>& validEqs);
        std::vector<Equation ^> GetValidEquations();
        void SetGraphArgs();
        void UpdateVariables();

        void OnForceProportionalAxesChanged(Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args);

        void OnBackgroundColorChanged(const Windows::UI::Color& color);

        void SyncEquationsWithItemsSource();
        void OnItemsAdded(int index, int count);
        void OnItemsRemoved(int index, int count);

        void ScaleRange(double centerX, double centerY, double scale);

        void OnCoreKeyDown(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ e);
        void OnCoreKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ e);

        void UpdateTracingChanged();
        void HandleTracingMovementTick(Object ^ sender, Object ^ e);
        void HandleKey(bool keyDown, Windows::System::VirtualKey key);


    private:
        DX::RenderMain ^ m_renderMain = nullptr;

        static Windows::UI::Xaml::DependencyProperty ^ s_equationTemplateProperty;

        static Windows::UI::Xaml::DependencyProperty ^ s_equationsSourceProperty;
        InspectingDataSource ^ m_dataSource;
        Windows::Foundation::EventRegistrationToken m_tokenDataSourceChanged;

        static Windows::UI::Xaml::DependencyProperty ^ s_equationsProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_variablesProperty;
        Windows::Foundation::EventRegistrationToken m_tokenEquationsChanged;
        Windows::Foundation::EventRegistrationToken m_tokenEquationChanged;

        static Windows::UI::Xaml::DependencyProperty ^ s_forceProportionalAxesTemplateProperty;

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

       Windows::UI::Xaml::DispatcherTimer ^ m_TraceingTrackingTimer;

    public:
        Windows::Storage::Streams::RandomAccessStreamReference ^ GetGraphBitmapStream();
    };
}
