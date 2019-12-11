// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Grapher.h"
#include "IBitmap.h"
#include "../../CalcViewModel/GraphingCalculatorEnums.h"

using namespace Graphing;
using namespace GraphControl;
using namespace GraphControl::DX;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Devices::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::System;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;

namespace
{
    constexpr auto s_defaultStyleKey = L"GraphControl.Grapher";
    constexpr auto s_templateKey_SwapChainPanel = L"GraphSurface";

    constexpr auto s_propertyName_Equations = L"Equations";
    constexpr auto s_propertyName_Variables = L"Variables";
    constexpr auto s_propertyName_ForceProportionalAxes = L"ForceProportionalAxes";

    constexpr auto s_X = L"x";
    constexpr auto s_Y = L"y";
    constexpr auto s_defaultFormatType = FormatType::MathML;
    constexpr auto s_getGraphOpeningTags = L"<math xmlns=\"http://www.w3.org/1998/Math/MathML\"><mrow><mi>show2d</mi><mfenced separators=\"\">";
    constexpr auto s_getGraphClosingTags = L"</mfenced></mrow></math>";

    // Helper function for converting a pointer position to a position that the graphing engine will understand.
    // posX/posY are the pointer position elements and width,height are the dimensions of the graph surface.
    // The graphing engine interprets x,y position between the range [-1, 1].
    // Translate the pointer position to the [-1, 1] bounds.
    __inline pair<double, double> PointerPositionToGraphPosition(double posX, double posY, double width, double height)
    {
        return make_pair((2 * posX / width - 1), (1 - 2 * posY / height));
    }
}

namespace GraphControl
{
    DependencyProperty ^ Grapher::s_equationsProperty;
    DependencyProperty ^ Grapher::s_variablesProperty;
    DependencyProperty ^ Grapher::s_forceProportionalAxesTemplateProperty;

    Grapher::Grapher()
        : m_solver{ IMathSolver::CreateMathSolver() }
        , m_graph{ m_solver->CreateGrapher() }
        , m_Moving{ false }
    {
        m_solver->ParsingOptions().SetFormatType(s_defaultFormatType);
        m_solver->FormatOptions().SetFormatType(s_defaultFormatType);
        m_solver->FormatOptions().SetMathMLPrefix(wstring(L"mml"));

        DefaultStyleKey = StringReference(s_defaultStyleKey);

        this->SetValue(EquationsProperty, ref new EquationCollection());
        this->SetValue(VariablesProperty, ref new Map<String ^, double>());

        this->Loaded += ref new RoutedEventHandler(this, &Grapher::OnLoaded);
        this->Unloaded += ref new RoutedEventHandler(this, &Grapher::OnUnloaded);

        this->ManipulationMode = ManipulationModes::TranslateX | ManipulationModes::TranslateY | ManipulationModes::TranslateInertia | ManipulationModes::Scale
                                 | ManipulationModes::ScaleInertia;

        auto cw = CoreWindow::GetForCurrentThread();
        cw->KeyDown += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &Grapher::OnCoreKeyDown);
        cw->KeyUp += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &Grapher::OnCoreKeyUp);

        auto& formatOptions = m_solver->FormatOptions();
    }

    void Grapher::OnLoaded(Object ^ sender, RoutedEventArgs ^ args)
    {
        if (auto backgroundBrush = safe_cast<SolidColorBrush ^>(this->Background))
        {
            m_tokenBackgroundColorChanged.Value = backgroundBrush->RegisterPropertyChangedCallback(
                SolidColorBrush::ColorProperty, ref new DependencyPropertyChangedCallback(this, &Grapher::OnDependencyPropertyChanged));

            OnBackgroundColorChanged(backgroundBrush->Color);
        }
    }

    void Grapher::OnUnloaded(Object ^ sender, RoutedEventArgs ^ args)
    {
        if (auto backgroundBrush = safe_cast<SolidColorBrush ^>(this->Background))
        {
            this->UnregisterPropertyChangedCallback(BackgroundProperty, m_tokenBackgroundColorChanged.Value);
        }
    }

    void Grapher::ZoomFromCenter(double scale)
    {
        ScaleRange(0, 0, scale);
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

    void Grapher::ResetGrid()
    {
        if (m_graph != nullptr && m_renderMain != nullptr)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                if (SUCCEEDED(renderer->ResetRange()))
                {
                    m_renderMain->RunRenderPass();
                }
            }
        }
    }

    void Grapher::OnApplyTemplate()
    {
        auto swapChainPanel = dynamic_cast<SwapChainPanel ^>(GetTemplateChild(StringReference(s_templateKey_SwapChainPanel)));
        if (swapChainPanel)
        {
            swapChainPanel->AllowFocusOnInteraction = true;
            m_renderMain = ref new RenderMain(swapChainPanel);
        }

        TryUpdateGraph();
    }

    void Grapher::RegisterDependencyProperties()
    {
        if (!s_equationsProperty)
        {
            s_equationsProperty = DependencyProperty::Register(
                StringReference(s_propertyName_Equations),
                EquationCollection::typeid,
                Grapher::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Grapher::OnCustomDependencyPropertyChanged)));
        }

        if (!s_variablesProperty)
        {
            s_variablesProperty = DependencyProperty::Register(
                StringReference(s_propertyName_Variables),
                IObservableMap<String ^, double>::typeid,
                Grapher::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Grapher::OnCustomDependencyPropertyChanged)));
        }

        if (!s_forceProportionalAxesTemplateProperty)
        {
            s_forceProportionalAxesTemplateProperty = DependencyProperty::Register(
                StringReference(s_propertyName_ForceProportionalAxes),
                bool ::typeid,
                Grapher::typeid,
                ref new PropertyMetadata(true, ref new PropertyChangedCallback(&Grapher::OnCustomDependencyPropertyChanged)));
        }
    }

    void Grapher::OnCustomDependencyPropertyChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ args)
    {
        auto self = static_cast<Grapher ^>(obj);
        if (self)
        {
            if (args->Property == EquationsProperty)
            {
                self->OnEquationsChanged(args);
            }
            else if (args->Property == ForceProportionalAxesTemplateProperty)
            {
                self->OnForceProportionalAxesChanged(args);
            }
        }
    }

    void Grapher::OnDependencyPropertyChanged(DependencyObject ^ obj, DependencyProperty ^ p)
    {
        if (p == SolidColorBrush::ColorProperty)
        {
            auto brush = static_cast<SolidColorBrush ^>(obj);
            OnBackgroundColorChanged(brush->Color);
        }
    }

    void Grapher::OnEquationsChanged(DependencyPropertyChangedEventArgs ^ args)
    {
        if (auto older = static_cast<EquationCollection ^>(args->OldValue))
        {
            if (m_tokenEquationChanged.Value != 0)
            {
                older->EquationChanged -= m_tokenEquationChanged;
                m_tokenEquationChanged.Value = 0;
            }

            if (m_tokenEquationStyleChanged.Value != 0)
            {
                older->EquationStyleChanged -= m_tokenEquationStyleChanged;
                m_tokenEquationStyleChanged.Value = 0;
            }
        }

        if (auto newer = static_cast<EquationCollection ^>(args->NewValue))
        {
            m_tokenEquationChanged = newer->EquationChanged += ref new EquationChangedEventHandler(this, &Grapher::OnEquationChanged);

            m_tokenEquationStyleChanged = newer->EquationStyleChanged += ref new EquationChangedEventHandler(this, &Grapher::OnEquationStyleChanged);

            m_tokenEquationLineEnabledChanged = newer->EquationLineEnabledChanged +=
                ref new EquationChangedEventHandler(this, &Grapher::OnEquationLineEnabledChanged);
        }

        PlotGraph();
    }

    void Grapher::OnEquationChanged(Equation ^ equation)
    {
        // If the equation was previously valid, we should try to graph again in the event of the failure
        bool shouldRetry = equation->IsValidated;

        // Reset these properties if the equation is requesting to be graphed again
        equation->HasGraphError = false;
        equation->IsValidated = false;

        TryPlotGraph(shouldRetry);
    }

    void Grapher::OnEquationStyleChanged(Equation ^)
    {
        if (m_graph)
        {
            UpdateGraphOptions(m_graph->GetOptions(), GetGraphableEquations());
        }

        if (m_renderMain)
        {
            m_renderMain->RunRenderPass();
        }
    }

    void Grapher::OnEquationLineEnabledChanged(Equation ^ equation)
    {
        // If the equation is in an error state or is empty, it should not be graphed anyway.
        if (equation->HasGraphError || equation->Expression->IsEmpty())
        {
            return;
        }

        PlotGraph();
    }

    void Grapher::AnalyzeEquation(Equation ^ equation)
    {
        if (auto graph = GetGraph(equation))
        {
            if (auto analyzer = graph->GetAnalyzer())
            {
                if (analyzer->CanFunctionAnalysisBePerformed())
                {
                    if (S_OK
                        == analyzer->PerformFunctionAnalysis(
                            (Graphing::Analyzer::NativeAnalysisType)Graphing::Analyzer::PerformAnalysisType::PerformAnalysisType_All))
                    {
                        Graphing::IGraphFunctionAnalysisData functionAnalysisData = m_solver->Analyze(analyzer.get());
                        {
                            equation->XIntercept = ref new String(functionAnalysisData.Zeros.c_str());
                            equation->YIntercept = ref new String(functionAnalysisData.YIntercept.c_str());
                            equation->Domain = ref new String(functionAnalysisData.Domain.c_str());
                            equation->Range = ref new String(functionAnalysisData.Range.c_str());
                            equation->Parity = functionAnalysisData.Parity;
                            equation->PeriodicityDirection = functionAnalysisData.PeriodicityDirection;
                            equation->PeriodicityExpression = ref new String(functionAnalysisData.PeriodicityExpression.c_str());
                            equation->Minima = ConvertWStringVector(functionAnalysisData.Minima);
                            equation->Maxima = ConvertWStringVector(functionAnalysisData.Maxima);
                            equation->InflectionPoints = ConvertWStringVector(functionAnalysisData.InflectionPoints);
                            equation->Monotonicity = ConvertWStringIntMap(functionAnalysisData.MonotoneIntervals);
                            equation->VerticalAsymptotes = ConvertWStringVector(functionAnalysisData.VerticalAsymptotes);
                            equation->HorizontalAsymptotes = ConvertWStringVector(functionAnalysisData.HorizontalAsymptotes);
                            equation->ObliqueAsymptotes = ConvertWStringVector(functionAnalysisData.ObliqueAsymptotes);
                            equation->TooComplexFeatures = functionAnalysisData.TooComplexFeatures;
                            equation->AnalysisError = CalculatorApp::AnalysisErrorType::NoError;

                            return;
                        }
                    }
                }
                else
                {
                    equation->AnalysisError = CalculatorApp::AnalysisErrorType::AnalysisNotSupported;

                    return;
                }
            }
        }

        equation->AnalysisError = CalculatorApp::AnalysisErrorType::AnalysisCouldNotBePerformed;
    }

    void Grapher::PlotGraph()
    {
        TryPlotGraph(false);
    }

    void Grapher::TryPlotGraph(bool shouldRetry)
    {
        if (TryUpdateGraph())
        {
            SetEquationsAsValid();
        }
        else
        {
            SetEquationErrors();

            // If we failed to plot the graph, try again after the bad equations are flagged.
            if (shouldRetry)
            {
                TryUpdateGraph();
            }
        }
    }

    bool Grapher::TryUpdateGraph()
    {
        optional<vector<shared_ptr<IEquation>>> initResult = nullopt;
        bool successful = false;

        if (m_renderMain && m_graph != nullptr)
        {
            unique_ptr<IExpression> graphExpression;
            wstring request;

            auto validEqs = GetGraphableEquations();

            // Will be set to true if the previous graph should be kept in the event of an error
            bool shouldKeepPreviousGraph = false;

            if (!validEqs.empty())
            {
                wstringstream ss{};
                ss << s_getGraphOpeningTags;

                int numValidEquations = 0;
                for (Equation ^ eq : validEqs)
                {
                    if (eq->IsValidated)
                    {
                        shouldKeepPreviousGraph = true;
                    }

                    if (numValidEquations++ > 0)
                    {
                        ss << L"<mo>,</mo>";
                    }

                    ss << eq->GetRequest();
                }

                ss << s_getGraphClosingTags;

                request = ss.str();
            }

            if (graphExpression = m_solver->ParseInput(request))
            {
                initResult = m_graph->TryInitialize(graphExpression.get());

                if (initResult != nullopt)
                {
                    UpdateGraphOptions(m_graph->GetOptions(), validEqs);
                    SetGraphArgs();

                    m_renderMain->Graph = m_graph;

                    // It is possible that the render fails, in that case fall through to explicit empty initialization
                    if (m_renderMain->RunRenderPass())
                    {
                        UpdateVariables();
                        successful = true;
                    }
                    else
                    {
                        // If we failed to render then we have already lost the previous graph
                        shouldKeepPreviousGraph = false;
                        initResult = nullopt;
                    }
                }
            }

            if (initResult == nullopt)
            {
                // Do not re-initialize the graph to empty if there are still valid equations graphed
                if (!shouldKeepPreviousGraph)
                {
                    initResult = m_graph->TryInitialize();

                    if (initResult != nullopt)
                    {
                        UpdateGraphOptions(m_graph->GetOptions(), validEqs);
                        SetGraphArgs();

                        m_renderMain->Graph = m_graph;
                        m_renderMain->RunRenderPass();

                        UpdateVariables();

                        // Initializing an empty graph is only a success if there were no equations to graph.
                        successful = (validEqs.size() == 0);
                    }
                }
            }
        }

        // Return true if we were able to graph and render all graphable equations
        return successful;
    }

    void Grapher::SetEquationsAsValid()
    {
        for (Equation ^ eq : GetGraphableEquations())
        {
            eq->IsValidated = true;
        }
    }

    void Grapher::SetEquationErrors()
    {
        for (Equation ^ eq : GetGraphableEquations())
        {
            if (!eq->IsValidated)
            {
                eq->HasGraphError = true;
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

    shared_ptr<IGraph> Grapher::GetGraph(GraphControl::Equation ^ equation)
    {
        std::shared_ptr<Graphing::IGraph> graph = m_solver->CreateGrapher();

        wstringstream ss{};
        ss << s_getGraphOpeningTags;
        ss << equation->GetRequest();
        ss << s_getGraphClosingTags;

        wstring request = ss.str();
        unique_ptr<IExpression> graphExpression;
        if (graphExpression = m_solver->ParseInput(request))
        {
            if (graph->TryInitialize(graphExpression.get()))
            {
                return graph;
            }
        }

        return nullptr;
    }

    IObservableVector<String ^> ^ Grapher::ConvertWStringVector(vector<wstring> inVector)
    {
        Vector<String ^> ^ outVector = ref new Vector<String ^>();

        for (auto v : inVector)
        {
            outVector->Append(ref new String(v.c_str()));
        }

        return outVector;
    }

    IObservableMap<String ^, String ^> ^ Grapher::ConvertWStringIntMap(map<wstring, int> inMap)
    {
        Map<String ^, String ^> ^ outMap = ref new Map<String ^, String ^>();
        ;
        for (auto m : inMap)
        {
            outMap->Insert(ref new String(m.first.c_str()), m.second.ToString());
        }

        return outMap;
    }

    void Grapher::UpdateVariables()
    {
        auto updatedVariables = ref new Map<String ^, double>();

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

    void Grapher::SetVariable(Platform::String ^ variableName, double newValue)
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

    void Grapher::UpdateGraphOptions(IGraphingOptions& options, const vector<Equation ^>& validEqs)
    {
        options.SetForceProportional(ForceProportionalAxes);

        if (!options.GetAllowKeyGraphFeaturesForFunctionsWithParameters())
        {
            options.SetAllowKeyGraphFeaturesForFunctionsWithParameters(true);
        }

        if (!validEqs.empty())
        {
            vector<Graphing::Color> graphColors;
            graphColors.reserve(validEqs.size());
            for (Equation ^ eq : validEqs)
            {
                auto lineColor = eq->LineColor->Color;
                graphColors.emplace_back(lineColor.R, lineColor.G, lineColor.B, lineColor.A);
            }
            options.SetGraphColors(graphColors);
        }
    }

    vector<Equation ^> Grapher::GetGraphableEquations()
    {
        vector<Equation ^> validEqs;

        for (Equation ^ eq : Equations)
        {
            if (eq->IsGraphableEquation())
            {
                validEqs.push_back(eq);
            }
        }

        return validEqs;
    }

    void Grapher::OnForceProportionalAxesChanged(DependencyPropertyChangedEventArgs ^ args)
    {
        TryUpdateGraph();
    }

    void Grapher::OnBackgroundColorChanged(const Windows::UI::Color& color)
    {
        if (m_renderMain)
        {
            m_renderMain->BackgroundColor = color;
        }
    }

    void Grapher::OnPointerEntered(PointerRoutedEventArgs ^ e)
    {
        if (m_renderMain)
        {
            OnPointerMoved(e);
            m_renderMain->DrawNearestPoint = true;

            e->Handled = true;
        }
    }

    void Grapher::UpdateTracingChanged()
    {
        if (m_renderMain->Tracing)
        {
            TracingChangedEvent(true);
            TracingValueChangedEvent(m_renderMain->TraceValue);
        }
        else
        {
            TracingChangedEvent(false);
        }
    }

    void Grapher::OnPointerMoved(PointerRoutedEventArgs ^ e)
    {
        if (m_renderMain)
        {
            PointerPoint ^ currPoint = e->GetCurrentPoint(/* relativeTo */ this);
            m_renderMain->PointerLocation = currPoint->Position;
            UpdateTracingChanged();

            e->Handled = true;
        }
    }

    void Grapher::OnPointerExited(PointerRoutedEventArgs ^ e)
    {
        if (m_renderMain)
        {
            m_renderMain->DrawNearestPoint = false;
            TracingChangedEvent(false);
            e->Handled = true;
        }
    }

    void Grapher::OnPointerWheelChanged(PointerRoutedEventArgs ^ e)
    {
        PointerPoint ^ currentPointer = e->GetCurrentPoint(/*relative to*/ this);

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
        const auto [centerX, centerY] = PointerPositionToGraphPosition(pos.X, pos.Y, ActualWidth, ActualHeight);

        ScaleRange(centerX, centerY, scale);

        e->Handled = true;
    }

    void Grapher::OnPointerPressed(PointerRoutedEventArgs ^ e)
    {
        // Set the pointer capture to the element being interacted with so that only it
        // will fire pointer-related events
        CapturePointer(e->Pointer);
    }

    void Grapher::OnPointerReleased(PointerRoutedEventArgs ^ e)
    {
        ReleasePointerCapture(e->Pointer);
    }

    void Grapher::OnPointerCanceled(PointerRoutedEventArgs ^ e)
    {
        ReleasePointerCapture(e->Pointer);
    }

    void Grapher::OnManipulationDelta(ManipulationDeltaRoutedEventArgs ^ e)
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
                    const auto [centerX, centerY] = PointerPositionToGraphPosition(pos.X, pos.Y, width, height);

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

    RandomAccessStreamReference ^ Grapher::GetGraphBitmapStream()
    {
        RandomAccessStreamReference ^ outputStream;

        if (m_renderMain != nullptr && m_graph != nullptr)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                std::shared_ptr<Graphing::IBitmap> BitmapOut;
                bool hasSomeMissingDataOut = false;
                HRESULT hr = E_FAIL;
                hr = renderer->GetBitmap(BitmapOut, hasSomeMissingDataOut);
                if (SUCCEEDED(hr))
                {
                    // Get the raw date
                    std::vector<BYTE> byteVector = BitmapOut->GetData();
                    auto arr = ref new Array<BYTE>(&byteVector[0], (unsigned int)byteVector.size());

                    // create a memory stream wrapper
                    InMemoryRandomAccessStream ^ stream = ref new InMemoryRandomAccessStream();

                    // Get a writer to transfer the data
                    auto writer = ref new DataWriter(stream->GetOutputStreamAt(0));

                    // write the data
                    writer->WriteBytes(arr);
                    writer->StoreAsync()->GetResults();

                    // Get a reference stream to return;
                    outputStream = RandomAccessStreamReference::CreateFromStream(stream);
                }
                else
                {
                    OutputDebugString(L"Grapher::GetGraphBitmapStream() unable to get graph image from renderer\r\n");
                    winrt::throw_hresult(hr);
                }
            }
        }

        return outputStream;
    }
}

void Grapher::OnCoreKeyUp(CoreWindow ^ sender, KeyEventArgs ^ e)
{
    // We don't want to react to keyboard input unless the graph control has the focus.
    // NOTE: you can't select the graph control from the mouse for focus but you can tab to it.
    GraphControl::Grapher ^ gcHasFocus = dynamic_cast<GraphControl::Grapher ^>(FocusManager::GetFocusedElement());
    if (gcHasFocus == nullptr || gcHasFocus != this)
    {
        // Not a graphingCalculator control so we don't want the input.
        return;
    }

    switch (e->VirtualKey)
    {
    case VirtualKey::Left:
    case VirtualKey::Right:
    case VirtualKey::Down:
    case VirtualKey::Up:
    case VirtualKey::Shift:
    {
        HandleKey(false, e->VirtualKey);
    }
    break;
    }
}

void Grapher::OnCoreKeyDown(CoreWindow ^ sender, KeyEventArgs ^ e)
{
    // We don't want to react to any keys when we are not in the graph control
    GraphControl::Grapher ^ gcHasFocus = dynamic_cast<GraphControl::Grapher ^>(FocusManager::GetFocusedElement());
    if (gcHasFocus == nullptr || gcHasFocus != this)
    {
        // Not a graphingCalculator control so we don't want the input.
        return;
    }

    switch (e->VirtualKey)
    {
    case VirtualKey::Left:
    case VirtualKey::Right:
    case VirtualKey::Down:
    case VirtualKey::Up:
    case VirtualKey::Shift:
    {
        HandleKey(true, e->VirtualKey);
    }
    break;
    }
}

void Grapher::HandleKey(bool keyDown, VirtualKey key)
{
    int pressedKeys = 0;
    if (key == VirtualKey::Left)
    {
        m_KeysPressed[KeysPressedSlots::Left] = keyDown;
        if (keyDown)
        {
            pressedKeys++;
        }
    }
    if (key == VirtualKey::Right)
    {
        m_KeysPressed[KeysPressedSlots::Right] = keyDown;
        if (keyDown)
        {
            pressedKeys++;
        }
    }
    if (key == VirtualKey::Up)
    {
        m_KeysPressed[KeysPressedSlots::Up] = keyDown;
        if (keyDown)
        {
            pressedKeys++;
        }
    }
    if (key == VirtualKey::Down)
    {
        m_KeysPressed[KeysPressedSlots::Down] = keyDown;
        if (keyDown)
        {
            pressedKeys++;
        }
    }
    if (key == VirtualKey::Shift)
    {
        m_KeysPressed[KeysPressedSlots::Accelerator] = keyDown;
    }

    if (pressedKeys > 0 && !m_Moving)
    {
        m_Moving = true;
        // Key(s) we care about, so ensure we are ticking our timer (and that we have one to tick)
        if (m_TracingTrackingTimer == nullptr)
        {
            m_TracingTrackingTimer = ref new DispatcherTimer();

            m_TracingTrackingTimer->Tick += ref new EventHandler<Object ^>(this, &Grapher::HandleTracingMovementTick);
            TimeSpan ts;
            ts.Duration = 100000; // .1 second
            m_TracingTrackingTimer->Interval = ts;
            auto i = m_TracingTrackingTimer->Interval;
        }
        m_TracingTrackingTimer->Start();
    }
}

void Grapher::HandleTracingMovementTick(Object ^ sender, Object ^ e)
{
    int delta = 5;
    int liveKeys = 0;

    if (m_KeysPressed[KeysPressedSlots::Accelerator])
    {
        delta = 1;
    }

    auto curPos = ActiveTraceCursorPosition;

    if (m_KeysPressed[KeysPressedSlots::Left])
    {
        liveKeys++;
        curPos.X -= delta;
        if (curPos.X < 0)
        {
            curPos.X = 0;
        }
    }

    if (m_KeysPressed[KeysPressedSlots::Right])
    {
        liveKeys++;
        curPos.X += delta;
        if (curPos.X > ActualWidth - delta)
        {
            curPos.X = (float)ActualWidth - delta; // TODO change this to deal with size of cursor
        }
    }

    if (m_KeysPressed[KeysPressedSlots::Up])
    {
        liveKeys++;
        curPos.Y -= delta;
        if (curPos.Y < 0)
        {
            curPos.Y = 0;
        }
    }

    if (m_KeysPressed[KeysPressedSlots::Down])
    {
        liveKeys++;
        curPos.Y += delta;
        if (curPos.Y > ActualHeight - delta)
        {
            curPos.Y = (float)ActualHeight - delta; // TODO change this to deal with size of cursor
        }
    }

    if (liveKeys == 0)
    {
        m_Moving = false;

        // Non of the keys we care about are being hit any longer so shut down our timer
        m_TracingTrackingTimer->Stop();
    }
    else
    {
        ActiveTraceCursorPosition = curPos;
    }
}

String ^ Grapher::ConvertToLinear(String ^ mmlString)
{
    m_solver->FormatOptions().SetFormatType(FormatType::LinearInput);

    auto expression = m_solver->ParseInput(mmlString->Data());
    auto linearExpression = m_solver->Serialize(expression.get());

    m_solver->FormatOptions().SetFormatType(s_defaultFormatType);

    return ref new String(linearExpression.c_str());
}
