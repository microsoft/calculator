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

namespace
{
    constexpr auto s_defaultStyleKey = L"GraphControl.Grapher";
    constexpr auto s_templateKey_SwapChainPanel = L"GraphSurface";

    constexpr auto s_propertyName_EquationTemplate = L"EquationTemplate";
    constexpr auto s_propertyName_Equations = L"Equations";
    constexpr auto s_propertyName_EquationsSource = L"EquationsSource";
    constexpr auto s_propertyName_Variables = L"Variables";
    constexpr auto s_propertyName_ForceProportionalAxes = L"ForceProportionalAxes";

    constexpr auto s_X = L"x";
    constexpr auto s_Y = L"y";

    // Helper function for converting a pointer position to a position that the graphing engine will understand.
    // posX/posY are the pointer position elements and width,height are the dimensions of the graph surface.
    // The graphing engine interprets x,y position between the range [-1, 1].
    // Translate the pointer position to the [-1, 1] bounds.
    __inline pair<double, double> PointerPositionToGraphPosition(double posX, double posY, double width, double height)
    {
        return make_pair(( 2 * posX / width - 1 ), ( 1 - 2 * posY / height ));
    }
}

namespace GraphControl
{
    DependencyProperty^ Grapher::s_equationTemplateProperty;
    DependencyProperty^ Grapher::s_equationsProperty;
    DependencyProperty^ Grapher::s_equationsSourceProperty;
    DependencyProperty^ Grapher::s_variablesProperty;
    DependencyProperty^ Grapher::s_forceProportionalAxesTemplateProperty;
    
    Grapher::Grapher()
        : m_solver{ IMathSolver::CreateMathSolver() }
        , m_graph{ m_solver->CreateGrapher() }
    {
        m_solver->ParsingOptions().SetFormatType(FormatType::Linear);

        DefaultStyleKey = StringReference(s_defaultStyleKey);

        this->SetValue(EquationsProperty, ref new EquationCollection());
        this->SetValue(VariablesProperty, ref new Map<String^, double>());

        this->Loaded += ref new RoutedEventHandler(this, &Grapher::OnLoaded);
        this->Unloaded += ref new RoutedEventHandler(this, &Grapher::OnUnloaded);

        this->ManipulationMode =
            ManipulationModes::TranslateX |
            ManipulationModes::TranslateY |
            ManipulationModes::TranslateInertia |
            ManipulationModes::Scale |
            ManipulationModes::ScaleInertia;
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
        if (m_graph != nullptr && m_renderMain != nullptr)
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

        if (!s_variablesProperty)
        {
            s_variablesProperty = DependencyProperty::Register(
                StringReference(s_propertyName_Variables),
                IObservableMap<String^, double>::typeid,
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
        if (event->CollectionChange == ::CollectionChange::ItemInserted || event->CollectionChange == ::CollectionChange::ItemChanged)
        {
            auto eq = sender->GetAt(event->Index);

            // Don't update the graph unless the equations being added/modified is valid.
            if (eq->Expression->IsEmpty())
            {
                return;
            }
        }

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
                unique_ptr<IExpression> graphExpression;
                if (graphExpression = m_solver->ParseInput(request))
                {
                    if (m_graph->TryInitialize(graphExpression.get()))
                    {
                        UpdateGraphOptions(m_graph->GetOptions(), validEqs);
                        SetGraphArgs();

                        m_renderMain->Graph = m_graph;

                        UpdateVariables();
                    }
                }
            }
            else
            {
                if (m_graph->TryInitialize())
                {
                    UpdateGraphOptions(m_graph->GetOptions(), validEqs);
                    SetGraphArgs();

                    m_renderMain->Graph = m_graph;

                    UpdateVariables();
                }
            }
        }
    }

    void Grapher::SetGraphArgs()
    {
        if (m_graph)
        {
            for (auto variable : Variables)
            {
                m_graph->SetArgValue(variable->Key->Data(), variable->Value);
            }
        }
    }

    void Grapher::UpdateVariables()
    {
        auto updatedVariables = ref new Map<String^, double>();
        if (m_graph)
        {
            auto graphVariables = m_graph->GetVariables();

            for (auto graphVar : graphVariables)
            {
                if (graphVar->GetVariableName() != s_X && graphVar->GetVariableName() != s_Y)
                {
                    auto key = ref new String(graphVar->GetVariableName().data());
                    double value = 1.0;

                    if (Variables->HasKey(key))
                    {
                        value = Variables->Lookup(key);
                    }

                    updatedVariables->Insert(key, value);
                }
            }
        }

        Variables = updatedVariables;
        VariablesUpdated(this, Variables);
    }

    void Grapher::SetVariable(Platform::String^ variableName, double newValue)
    {
        if (Variables->HasKey(variableName))
        {
            if (Variables->Lookup(variableName) == newValue)
            {
                return;
            }

            Variables->Remove(variableName);
        }

        Variables->Insert(variableName, newValue);

        if (m_graph)
        {
            m_graph->SetArgValue(variableName->Data(), newValue);

            if (m_renderMain)
            {
                m_renderMain->RunRenderPass();
            }
        }
    }

    void Grapher::UpdateGraphOptions(IGraphingOptions& options, const vector<Equation^>& validEqs)
    {
        options.SetForceProportional(ForceProportionalAxes);

        if (!validEqs.empty())
        {
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

    void Grapher::OnPointerWheelChanged(PointerRoutedEventArgs^ e)
    {
        PointerPoint^ currentPointer = e->GetCurrentPoint(/*relative to*/ this);

        double delta = currentPointer->Properties->MouseWheelDelta;

        // The maximum delta is 120 according to:
        // https://docs.microsoft.com/en-us/uwp/api/windows.ui.input.pointerpointproperties.mousewheeldelta#Windows_UI_Input_PointerPointProperties_MouseWheelDelta
        // Apply a dampening effect so that small mouse movements have a smoother zoom.
        constexpr double scrollDamper = 0.15;
        double scale = 1.0 + (abs(delta) / WHEEL_DELTA) * scrollDamper;

        // positive delta if wheel scrolled away from the user
        if (delta >= 0)
        {
            scale = 1.0 / scale;
        }

        // For scaling, the graphing engine interprets x,y position between the range [-1, 1].
        // Translate the pointer position to the [-1, 1] bounds.
        const auto& pos = currentPointer->Position;
        const auto[centerX, centerY] = PointerPositionToGraphPosition(pos.X, pos.Y, ActualWidth, ActualHeight);

        ScaleRange(centerX, centerY, scale);

        e->Handled = true;
    }

    void Grapher::OnPointerPressed(PointerRoutedEventArgs^ e)
	{
        // Set the pointer capture to the element being interacted with so that only it
        // will fire pointer-related events
        CapturePointer(e->Pointer);
	}

    void Grapher::OnPointerReleased(PointerRoutedEventArgs^ e)
	{
        ReleasePointerCapture(e->Pointer);
	}

    void Grapher::OnPointerCanceled(PointerRoutedEventArgs^ e)
    {
        ReleasePointerCapture(e->Pointer);
    }

    void Grapher::OnManipulationDelta(ManipulationDeltaRoutedEventArgs^ e)
    {
        if (m_renderMain != nullptr && m_graph != nullptr)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                // Only call for a render pass if we actually scaled or translated.
                bool needsRenderPass = false;

                const double width = ActualWidth;
                const double height = ActualHeight;

                const auto& translation = e->Delta.Translation;
                double translationX = translation.X;
                double translationY = translation.Y;
                if (translationX != 0 || translationY != 0)
                {
                    // The graphing engine pans the graph according to a ratio for x and y.
                    // A value of +1 means move a half screen in the positive direction for the given axis.
                    // Convert the manipulation's translation values to ratios for the engine.
                    translationX /= -width;
                    translationY /= height;

                    if (FAILED(renderer->MoveRangeByRatio(translationX, translationY)))
                    {
                        return;
                    }

                    needsRenderPass = true;
                }

                if (double scale = e->Delta.Scale; scale != 1.0)
                {
                    // The graphing engine interprets scale amounts as the inverse of the value retrieved
                    // from the ManipulationUpdatedEventArgs. Invert the scale amount for the engine.
                    scale = 1.0 / scale;

                    // Convert from PointerPosition to graph position.
                    const auto& pos = e->Position;
                    const auto[centerX, centerY] = PointerPositionToGraphPosition(pos.X, pos.Y, width, height);

                    if (FAILED(renderer->ScaleRange(centerX, centerY, scale)))
                    {
                        return;
                    }

                    needsRenderPass = true;
                }

                if (needsRenderPass)
                {
                    m_renderMain->RunRenderPass();
                }
            }
        }
    }
}
