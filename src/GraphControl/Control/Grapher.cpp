#include "pch.h"
#include "Grapher.h"

using namespace Graphing;
using namespace GraphControl;
using namespace GraphControl::DX;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Devices::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::UI;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;

namespace GraphControl
{
    constexpr auto s_defaultStyleKey = L"GraphControl.Grapher";
    constexpr auto s_templateKey_SwapChainPanel = L"GraphSurface";

    DependencyProperty^ Grapher::s_equationTemplateProperty;
    constexpr auto s_propertyName_EquationTemplate = L"EquationTemplate";

    DependencyProperty^ Grapher::s_equationsProperty;
    constexpr auto s_propertyName_Equations = L"Equations";

    DependencyProperty^ Grapher::s_equationsSourceProperty;
    constexpr auto s_propertyName_EquationsSource = L"EquationsSource";

    DependencyProperty^ Grapher::s_forceProportionalAxesTemplateProperty;
    constexpr auto s_propertyName_ForceProportionalAxes = L"ForceProportionalAxes";

    Grapher::Grapher()
        : m_solver{ IMathSolver::CreateMathSolver() }
        , m_graph{ m_solver->CreateGrapher() }
    {
        m_solver->ParsingOptions().SetFormatType(FormatType::Linear);

        DefaultStyleKey = StringReference(s_defaultStyleKey);

        this->SetValue(EquationsProperty, ref new EquationCollection());

        this->Loaded += ref new RoutedEventHandler(this, &Grapher::OnLoaded);
        this->Unloaded += ref new RoutedEventHandler(this, &Grapher::OnUnloaded);

        m_gestureRecognizer->GestureSettings =
            GestureSettings::ManipulationScale;

        m_gestureRecognizer->ManipulationUpdated += ref new TypedEventHandler<GestureRecognizer^, ManipulationUpdatedEventArgs^>(this, &Grapher::OnGestureManipulationUpdated);
    }

    void Grapher::OnLoaded(Object^ sender, RoutedEventArgs^ args)
    {
        if (auto backgroundBrush = safe_cast<SolidColorBrush^>(this->Background))
        {
            m_tokenBackgroundColorChanged.Value =
                backgroundBrush->RegisterPropertyChangedCallback(SolidColorBrush::ColorProperty, ref new DependencyPropertyChangedCallback(this, &Grapher::OnDependencyPropertyChanged));

            OnBackgroundColorChanged(backgroundBrush->Color);
        }
    }

    void Grapher::OnUnloaded(Object^ sender, RoutedEventArgs^ args)
    {
        if (auto backgroundBrush = safe_cast<SolidColorBrush^>(this->Background))
        {
            this->UnregisterPropertyChangedCallback(BackgroundProperty, m_tokenBackgroundColorChanged.Value);
        }
    }

    void Grapher::ScaleRange(double centerX, double centerY, double scale)
    {
        if (m_graph)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                if (SUCCEEDED(renderer->ScaleRange(centerX, centerY, scale)))
                {
                    m_renderMain->RunRenderPass();
                }
            }
        }
    }

    void Grapher::MoveRangeByRatio(double ratioX, double ratioY)
    {
        if (m_graph)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                if (SUCCEEDED(renderer->MoveRangeByRatio(ratioX, ratioY)))
                {
                    m_renderMain->RunRenderPass();
                }
            }
        }
    }

    void Grapher::OnApplyTemplate()
    {
        auto swapChainPanel = dynamic_cast<SwapChainPanel^>(GetTemplateChild(StringReference(s_templateKey_SwapChainPanel)));
        if (swapChainPanel)
        {
            m_renderMain = ref new RenderMain(swapChainPanel);
        }

        UpdateGraph();
    }

    void Grapher::RegisterDependencyProperties()
    {
        if (!s_equationsProperty)
        {
            s_equationsProperty = DependencyProperty::Register(
                StringReference(s_propertyName_Equations),
                EquationCollection::typeid ,
                Grapher::typeid,
                ref new PropertyMetadata(
                    nullptr,
                    ref new PropertyChangedCallback(&Grapher::OnCustomDependencyPropertyChanged)));
        }

        if (!s_equationsSourceProperty)
        {
            s_equationsSourceProperty = DependencyProperty::Register(
                StringReference(s_propertyName_EquationsSource),
                Object::typeid,
                Grapher::typeid,
                ref new PropertyMetadata(
                    nullptr,
                    ref new PropertyChangedCallback(&Grapher::OnCustomDependencyPropertyChanged)));
        }

        if (!s_equationTemplateProperty)
        {
            s_equationTemplateProperty = DependencyProperty::Register(
                StringReference(s_propertyName_EquationTemplate),
                DataTemplate::typeid,
                Grapher::typeid,
                ref new PropertyMetadata(
                    nullptr,
                    ref new PropertyChangedCallback(&Grapher::OnCustomDependencyPropertyChanged)));
        }

        if (!s_forceProportionalAxesTemplateProperty)
        {
            s_forceProportionalAxesTemplateProperty = DependencyProperty::Register(
                StringReference(s_propertyName_ForceProportionalAxes),
                bool::typeid,
                Grapher::typeid,
                ref new PropertyMetadata(
                    true,
                    ref new PropertyChangedCallback(&Grapher::OnCustomDependencyPropertyChanged)));
        }
    }

    void Grapher::OnCustomDependencyPropertyChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ args)
    {
        auto self = static_cast<Grapher^>(obj);
        if (self)
        {
            if (args->Property == EquationsProperty)
            {
                self->OnEquationsChanged(args);
            }
            else if (args->Property == EquationsSourceProperty)
            {
                self->OnEquationsSourceChanged(args);
            }
            else if (args->Property == EquationTemplateProperty)
            {
                self->OnEquationTemplateChanged(args);
            }
            else if (args->Property == ForceProportionalAxesTemplateProperty)
            {
                self->OnForceProportionalAxesChanged(args);
            }
        }
    }

    void Grapher::OnDependencyPropertyChanged(DependencyObject^ obj, DependencyProperty^ p)
    {
        if (p == SolidColorBrush::ColorProperty)
        {
            auto brush = static_cast<SolidColorBrush^>(obj);
            OnBackgroundColorChanged(brush->Color);
        }
    }

    void Grapher::OnEquationTemplateChanged(DependencyPropertyChangedEventArgs^ args)
    {
        SyncEquationsWithItemsSource();
    }

    void Grapher::OnEquationsSourceChanged(DependencyPropertyChangedEventArgs^ args)
    {
        if (m_dataSource && m_tokenDataSourceChanged.Value != 0)
        {
            m_dataSource->DataSourceChanged -= m_tokenDataSourceChanged;
        }

        m_dataSource = args->NewValue ? ref new InspectingDataSource(args->NewValue) : nullptr;
        if (m_dataSource)
        {
            m_tokenDataSourceChanged =
                m_dataSource->DataSourceChanged += ref new TypedEventHandler<InspectingDataSource^, DataSourceChangedEventArgs>(this, &Grapher::OnDataSourceChanged);
        }

        SyncEquationsWithItemsSource();
    }

    void Grapher::OnDataSourceChanged(InspectingDataSource^ sender, DataSourceChangedEventArgs args)
    {
        switch (args.Action)
        {
        case DataSourceChangedAction::Insert:
            OnItemsAdded(args.NewStartingIndex, args.NewItemsCount);
            break;

        case DataSourceChangedAction::Remove:
            OnItemsRemoved(args.OldStartingIndex, args.OldItemsCount);
            break;

        case DataSourceChangedAction::Reset:
            SyncEquationsWithItemsSource();
            break;

        case DataSourceChangedAction::Replace:
            OnItemsRemoved(args.OldStartingIndex, args.OldItemsCount);
            OnItemsAdded(args.NewStartingIndex, args.NewItemsCount);
            break;
        }
    }

    void Grapher::OnItemsAdded(int index, int count)
    {
        for (int i = index + count - 1; i >= index; i--)
        {
            auto eq = safe_cast<Equation^>(EquationTemplate->LoadContent());
            eq->DataContext = m_dataSource->GetAt(i);

            Equations->InsertAt(index, eq);
        }
    }

    void Grapher::OnItemsRemoved(int index, int count)
    {
        for (int i = 0; i < count; i++)
        {
            Equations->RemoveAt(index);
        }
    }

    void Grapher::SyncEquationsWithItemsSource()
    {
        Equations->Clear();
        if (m_dataSource)
        {
            auto size = m_dataSource->GetSize();
            for (auto i = 0u; i < size; i++)
            {
                auto eq = safe_cast<Equation^>(EquationTemplate->LoadContent());
                eq->DataContext = m_dataSource->GetAt(i);

                Equations->Append(eq);
            }
        }
    }

    void Grapher::OnEquationsChanged(DependencyPropertyChangedEventArgs^ args)
    {
        if (auto older = static_cast<EquationCollection^>(args->OldValue))
        {
            if (m_tokenEquationsChanged.Value != 0)
            {
                older->VectorChanged -= m_tokenEquationsChanged;
                m_tokenEquationsChanged.Value = 0;
            }
            if (m_tokenEquationChanged.Value != 0)
            {
                older->EquationChanged -= m_tokenEquationChanged;
                m_tokenEquationChanged.Value = 0;
            }
        }

        if (auto newer = static_cast<EquationCollection^>(args->NewValue))
        {
            m_tokenEquationsChanged =
                newer->VectorChanged += ref new VectorChangedEventHandler<Equation^>(this, &Grapher::OnEquationsVectorChanged);

            m_tokenEquationChanged =
                newer->EquationChanged += ref new EquationChangedEventHandler(this, &Grapher::OnEquationChanged);
        }

        UpdateGraph();
    }

    void Grapher::OnEquationsVectorChanged(IObservableVector<Equation^>^ sender, IVectorChangedEventArgs^ event)
    {
        UpdateGraph();
    }

    void Grapher::OnEquationChanged()
    {
        UpdateGraph();
    }

    void Grapher::UpdateGraph()
    {
        if (m_renderMain && m_graph != nullptr)
        {
            auto validEqs = GetValidEquations();
            if (!validEqs.empty())
            {

                wstringstream ss{};
                ss << L"show2d(";

                int numValidEquations = 0;
                for (Equation^ eq : validEqs)
                {
                    if (numValidEquations++ > 0)
                    {
                        ss << L",";
                    }

                    ss << eq->GetRequest();
                }

                ss << L")";

                wstring request = ss.str();
                if (auto graphExpression = m_solver->ParseInput(request))
                {
                    if (m_graph->TryInitialize(graphExpression.get()))
                    {
                        UpdateGraphOptions(m_graph->GetOptions(), validEqs);

                        m_renderMain->Graph = m_graph;
                    }
                }
            }
        }
    }

    void Grapher::UpdateGraphOptions(IGraphingOptions& options, const vector<Equation^>& validEqs)
    {
        options.SetForceProportional(ForceProportionalAxes);

        vector<Graphing::Color> graphColors;
        graphColors.reserve(validEqs.size());
        for (Equation^ eq : validEqs)
        {
            auto lineColor = eq->LineColor;
            graphColors.emplace_back(
                lineColor.R,
                lineColor.G,
                lineColor.B,
                lineColor.A);
        }
        options.SetGraphColors(graphColors);
    }

    vector<Equation^> Grapher::GetValidEquations()
    {
        vector<Equation^> validEqs;

        for (Equation^ eq : Equations)
        {
            if (!eq->Expression->IsEmpty())
            {
                validEqs.push_back(eq);
            }
        }

        return validEqs;
    }

    void Grapher::OnForceProportionalAxesChanged(DependencyPropertyChangedEventArgs^ args)
    {
        UpdateGraph();
    }

    void Grapher::OnBackgroundColorChanged(const Windows::UI::Color& color)
    {
        if (m_renderMain)
        {
            m_renderMain->BackgroundColor = color;
        }
    }

    void Grapher::OnPointerEntered(PointerRoutedEventArgs^ e)
    {
        if (m_renderMain)
        {
            OnPointerMoved(e);
            m_renderMain->DrawNearestPoint = true;

            e->Handled = true;
        }
    }

    void Grapher::OnPointerMoved(PointerRoutedEventArgs^ e)
    {
        if (m_renderMain)
        {
            PointerPoint^ currPoint = e->GetCurrentPoint(/* relativeTo */ this);
            m_renderMain->PointerLocation = currPoint->Position;

            m_gestureRecognizer->ProcessMoveEvents(e->GetIntermediatePoints(/*relativeTo*/ this));

            e->Handled = true;
        }
    }

    void Grapher::OnPointerExited(PointerRoutedEventArgs^ e)
    {
        if (m_renderMain)
        {
            m_renderMain->DrawNearestPoint = false;
            e->Handled = true;
        }
    }

    void Grapher::OnPointerWheelChanged(PointerRoutedEventArgs^ args)
    {
        PointerPoint^ currentPointer = args->GetCurrentPoint(/*relative to*/ this);

        VirtualKeyModifiers vkmod = args->KeyModifiers;
        m_gestureRecognizer->ProcessMouseWheelEvent(
            currentPointer,
            static_cast<bool>(vkmod & VirtualKeyModifiers::Shift),
            // Usually we would detect if Control key is pressed. Ctrl+MWHEELUP/DOWN is
            // considered a Scale manipulation. In this case, we want a basic MWHEELUP/DOWN
            // to trigger zoom, so we'll trick the gesture recognizer by always saying Ctrl
            // is pressed for the MouseWheelEvent.
            true);
    }

    void Grapher::OnPointerPressed(PointerRoutedEventArgs^ e)
	{
        // Set the pointer capture to the element being interacted with so that only it
        // will fire pointer-related events
        CapturePointer(e->Pointer);
        // Feed the current point into the gesture recognizer as a down event
        m_gestureRecognizer->ProcessDownEvent(e->GetCurrentPoint(/*relativeTo*/ this));
	}

    void Grapher::OnPointerReleased(PointerRoutedEventArgs^ e)
	{
        // Feed the current point into the gesture recognizer as an up event
        m_gestureRecognizer->ProcessUpEvent(e->GetCurrentPoint(/*relativeTo*/ this));
        // Release the pointer
        ReleasePointerCapture(e->Pointer);
	}

    void Grapher::OnPointerCanceled(PointerRoutedEventArgs^ e)
    {
        m_gestureRecognizer->CompleteGesture();
        ReleasePointerCapture(e->Pointer);
    }

    void Grapher::OnGestureManipulationUpdated(GestureRecognizer^ sender, ManipulationUpdatedEventArgs^ args)
    {
        const double width = ActualWidth;
        const double height = ActualHeight;

        if (double scale = args->Delta.Scale; scale != 1.0)
        {
            // The graphing engine interprets scale amounts as the inverse of the value retrieved
            // from the ManipulationUpdatedEventArgs. Invert the scale amount for the engine.
            scale = 1.0 / scale;

            // Add a minor amplification effect to the scale gesture.
            constexpr double amplification = 1.05;
            scale *= (scale > 1) ? amplification : 1.0 / amplification;

            // The graphing engine interprets x,y position between the range [-1, 1].
            // Translate the pointer position to the [-1, 1] bounds.
            const auto& pos = args->Position;
            double centerX = 2 * pos.X / width - 1;
            double centerY = 1 - 2 * pos.Y / height;

            ScaleRange(centerX, centerY, scale);
        }
    }
}
