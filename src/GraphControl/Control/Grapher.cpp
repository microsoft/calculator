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
using namespace Concurrency;
using namespace Windows::Devices::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::System;
using namespace Windows::System::Threading;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace GraphControl;

DEPENDENCY_PROPERTY_INITIALIZATION(Grapher, ForceProportionalAxes);
DEPENDENCY_PROPERTY_INITIALIZATION(Grapher, UseCommaDecimalSeperator);
DEPENDENCY_PROPERTY_INITIALIZATION(Grapher, Variables);
DEPENDENCY_PROPERTY_INITIALIZATION(Grapher, Equations);
DEPENDENCY_PROPERTY_INITIALIZATION(Grapher, AxesColor);
DEPENDENCY_PROPERTY_INITIALIZATION(Grapher, GraphBackground);
DEPENDENCY_PROPERTY_INITIALIZATION(Grapher, GridLinesColor);
DEPENDENCY_PROPERTY_INITIALIZATION(Grapher, LineWidth);
DEPENDENCY_PROPERTY_INITIALIZATION(Grapher, IsKeepCurrentView);

namespace
{
    constexpr auto s_defaultStyleKey = L"GraphControl.Grapher";
    constexpr auto s_templateKey_SwapChainPanel = L"GraphSurface";

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
    Grapher::Grapher()
        : m_solver{ IMathSolver::CreateMathSolver() }
        , m_graph{ m_solver->CreateGrapher() }
        , m_Moving{ false }
    {
        Equations = ref new EquationCollection();

        m_solver->ParsingOptions().SetFormatType(s_defaultFormatType);
        m_solver->FormatOptions().SetFormatType(s_defaultFormatType);
        m_solver->FormatOptions().SetMathMLPrefix(wstring(L"mml"));

        DefaultStyleKey = StringReference(s_defaultStyleKey);

        this->ManipulationMode = ManipulationModes::TranslateX | ManipulationModes::TranslateY | ManipulationModes::TranslateInertia | ManipulationModes::Scale
                                 | ManipulationModes::ScaleInertia;

        auto cw = CoreWindow::GetForCurrentThread();
        cw->KeyDown += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &Grapher::OnCoreKeyDown);
        cw->KeyUp += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &Grapher::OnCoreKeyUp);

        auto& formatOptions = m_solver->FormatOptions();
    }

    void Grapher::ZoomFromCenter(double scale)
    {
        ScaleRange(0, 0, scale);
        GraphViewChangedEvent(this, GraphViewChangedReason::Manipulation);
    }

    void Grapher::ScaleRange(double centerX, double centerY, double scale)
    {
        if (m_graph != nullptr && m_renderMain != nullptr)
        {
            if (auto renderer = m_graph->GetRenderer())
            {
                m_renderMain->GetCriticalSection().lock();

                if (SUCCEEDED(renderer->ScaleRange(centerX, centerY, scale)))
                {
                    m_renderMain->GetCriticalSection().unlock();

                    m_renderMain->RunRenderPass();
                    GraphViewChangedEvent(this, GraphViewChangedReason::Manipulation);
                }
                else
                {
                    m_renderMain->GetCriticalSection().unlock();
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
                HRESULT hr;

                // Reset the Grid using the m_initialDisplayRange properties when the user was last in Manual Adjustment mode and an equation was added.
                // Reset the Grid using the TryPlotGraph method when the range is updated via Graph Settings. Return out of this block so we don't render 2 times.
                // Reset the Grid using the ResetRange() in all other cases.
                if (m_resetUsingInitialDisplayRange)
                {
                    hr = renderer->SetDisplayRanges(m_initialDisplayRangeXMin, m_initialDisplayRangeXMax, m_initialDisplayRangeYMin, m_initialDisplayRangeYMax);
                    m_resetUsingInitialDisplayRange = false;
                }
                else if (m_rangeUpdatedBySettings)
                {
                    IsKeepCurrentView = false;
                    TryPlotGraph(false, false);
                    m_rangeUpdatedBySettings = false;
                    GraphViewChangedEvent(this, GraphViewChangedReason::Reset);
                    return;
                }
                else
                {
                    hr = renderer->ResetRange();
                }

                if (SUCCEEDED(hr))
                {
                    m_renderMain->RunRenderPass();
                    GraphViewChangedEvent(this, GraphViewChangedReason::Reset);
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
            m_renderMain->BackgroundColor = GraphBackground;
        }

        TryUpdateGraph(false);
    }

    void Grapher::OnEquationsPropertyChanged(EquationCollection ^ oldValue, EquationCollection ^ newValue)
    {
        if (oldValue != nullptr)
        {
            if (m_tokenEquationChanged.Value != 0)
            {
                oldValue->EquationChanged -= m_tokenEquationChanged;
                m_tokenEquationChanged.Value = 0;
            }

            if (m_tokenEquationStyleChanged.Value != 0)
            {
                oldValue->EquationStyleChanged -= m_tokenEquationStyleChanged;
                m_tokenEquationStyleChanged.Value = 0;
            }

            if (m_tokenEquationLineEnabledChanged.Value != 0)
            {
                oldValue->EquationLineEnabledChanged -= m_tokenEquationLineEnabledChanged;
                m_tokenEquationLineEnabledChanged.Value = 0;
            }
        }

        if (newValue != nullptr)
        {
            m_tokenEquationChanged = newValue->EquationChanged += ref new EquationChangedEventHandler(this, &Grapher::OnEquationChanged);

            m_tokenEquationStyleChanged = newValue->EquationStyleChanged += ref new EquationChangedEventHandler(this, &Grapher::OnEquationStyleChanged);

            m_tokenEquationLineEnabledChanged = newValue->EquationLineEnabledChanged +=
                ref new EquationChangedEventHandler(this, &Grapher::OnEquationLineEnabledChanged);
        }

        PlotGraph(false);
    }

    void Grapher::OnEquationChanged(Equation ^ equation)
    {
        // Reset these properties if the equation is requesting to be graphed again
        equation->HasGraphError = false;
        equation->IsValidated = false;

        TryPlotGraph(false, true);
    }

    void Grapher::OnEquationStyleChanged(Equation ^)
    {
        if (m_graph)
        {
            m_graph->TryResetSelection();
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
        bool keepCurrentView = true;

        // If the equation has changed, the IsLineEnabled state is reset.
        // This checks if the equation has been reset and sets keepCurrentView to false in this case.
        if (!equation->HasGraphError && !equation->IsValidated && equation->IsLineEnabled)
        {
            keepCurrentView = false;
        }

        PlotGraph(keepCurrentView);
    }

    KeyGraphFeaturesInfo ^ Grapher::AnalyzeEquation(Equation ^ equation)
    {
        if (auto graph = GetGraph(equation))
        {
            SetGraphArgs(graph);

            if (auto analyzer = graph->GetAnalyzer())
            {
                vector<Equation ^> equationVector;
                equationVector.push_back(equation);
                UpdateGraphOptions(graph->GetOptions(), equationVector);
                bool variableIsNotX;
                if (analyzer->CanFunctionAnalysisBePerformed(variableIsNotX) && !variableIsNotX)
                {
                    if (S_OK
                        == analyzer->PerformFunctionAnalysis(
                            (Graphing::Analyzer::NativeAnalysisType)Graphing::Analyzer::PerformAnalysisType::PerformAnalysisType_All))
                    {
                        Graphing::IGraphFunctionAnalysisData functionAnalysisData = m_solver->Analyze(analyzer.get());
                        return KeyGraphFeaturesInfo::Create(functionAnalysisData);
                    }
                }
                else if (variableIsNotX)
                {
                    return KeyGraphFeaturesInfo::Create(CalculatorApp::AnalysisErrorType::VariableIsNotX);
                }
                else
                {
                    return KeyGraphFeaturesInfo::Create(CalculatorApp::AnalysisErrorType::AnalysisNotSupported);
                }
            }
        }

        return KeyGraphFeaturesInfo::Create(CalculatorApp::AnalysisErrorType::AnalysisCouldNotBePerformed);
    }

    void Grapher::PlotGraph(bool keepCurrentView)
    {
        TryPlotGraph(keepCurrentView, false);
    }

    task<void> Grapher::TryPlotGraph(bool keepCurrentView, bool shouldRetry)
    {
        if (co_await TryUpdateGraph(keepCurrentView))
        {
            SetEquationsAsValid();
        }
        else
        {
            SetEquationErrors();

            // If we failed to plot the graph, try again after the bad equations are flagged.
            if (shouldRetry)
            {
                co_await TryUpdateGraph(keepCurrentView);
            }
        }

        int valid = 0;
        int invalid = 0;
        for (Equation ^ eq : Equations)
        {
            if (eq->HasGraphError)
            {
                invalid++;
            }
            if (eq->IsValidated)
            {
                valid++;
            }
        }
        if (!m_trigUnitsChanged)
        {
            TraceLogger::GetInstance()->LogEquationCountChanged(valid, invalid);
        }

        m_trigUnitsChanged = false;
        GraphPlottedEvent(this, ref new RoutedEventArgs());
    }

    task<bool> Grapher::TryUpdateGraph(bool keepCurrentView)
    {
        optional<vector<shared_ptr<IEquation>>> initResult = nullopt;
        bool successful = false;
        m_errorCode = 0;
        m_errorType = 0;

        if (m_renderMain && m_graph != nullptr)
        {
            unique_ptr<IExpression> graphExpression;
            wstring request;

            auto validEqs = GetGraphableEquations();

            // Will be set to true if the previous graph should be kept in the event of an error
            bool shouldKeepPreviousGraph = false;

            if (!validEqs.empty())
            {
                request = s_getGraphOpeningTags;

                int numValidEquations = 0;
                for (Equation ^ eq : validEqs)
                {
                    if (eq->IsValidated)
                    {
                        shouldKeepPreviousGraph = true;
                    }

                    if (numValidEquations++ > 0)
                    {
                        if (!UseCommaDecimalSeperator)
                        {
                            request += L"<mo>,</mo>";
                        }
                        else
                        {
                            request += L"<mo>;</mo>";
                        }
                    }
                    auto equationRequest = eq->GetRequest()->Data();

                    // If the equation request failed, then fail graphing.
                    if (equationRequest == nullptr)
                    {
                        co_return false;
                    }

                    unique_ptr<IExpression> expr;
                    wstring parsableEquation = s_getGraphOpeningTags;
                    parsableEquation += equationRequest;
                    parsableEquation += s_getGraphClosingTags;

                    // Wire up the corresponding error to an error message in the UI at some point
                    if (!(expr = m_solver->ParseInput(parsableEquation, m_errorCode, m_errorType)))
                    {
                        co_return false;
                    }

                    request += equationRequest;
                }

                request += s_getGraphClosingTags;
            }

            if (graphExpression = m_solver->ParseInput(request, m_errorCode, m_errorType))
            {
                initResult = TryInitializeGraph(keepCurrentView, graphExpression.get());

                if (initResult != nullopt)
                {
                    auto graphedEquations = initResult.value();

                    for (int i = 0; i < validEqs.size(); i++)
                    {
                        validEqs[i]->GraphedEquation = graphedEquations[i];
                    }

                    UpdateGraphOptions(m_graph->GetOptions(), validEqs);
                    SetGraphArgs(m_graph);

                    m_renderMain->Graph = m_graph;

                    // It is possible that the render fails, in that case fall through to explicit empty initialization
                    co_await m_renderMain->RunRenderPassAsync(false);
                    if (m_renderMain->IsRenderPassSuccesful())
                    {
                        UpdateVariables();
                        successful = true;
                    }
                    else
                    {
                        // If we failed to render then we have already lost the previous graph
                        shouldKeepPreviousGraph = false;
                        initResult = nullopt;
                        m_solver->HRErrorToErrorInfo(m_renderMain->GetRenderError(), m_errorCode, m_errorType);
                    }
                }
                else
                {
                    m_solver->HRErrorToErrorInfo(m_graph->GetInitializationError(), m_errorCode, m_errorType);
                }
            }

            if (initResult == nullopt)
            {
                // Do not re-initialize the graph to empty if there are still valid equations graphed
                if (!shouldKeepPreviousGraph)
                {
                    initResult = TryInitializeGraph(false, nullptr);
                    if (initResult != nullopt)
                    {
                        UpdateGraphOptions(m_graph->GetOptions(), vector<Equation ^>());
                        SetGraphArgs(m_graph);

                        m_renderMain->Graph = m_graph;
                        co_await m_renderMain->RunRenderPassAsync();

                        UpdateVariables();

                        // Initializing an empty graph is only a success if there were no equations to graph.
                        successful = (validEqs.size() == 0);
                    }
                }
            }
        }

        // Return true if we were able to graph and render all graphable equations
        co_return successful;
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
                eq->GraphErrorType = static_cast<ErrorType>(m_errorType);
                eq->GraphErrorCode = m_errorCode;
                eq->HasGraphError = true;
            }
        }
    }

    void Grapher::SetGraphArgs(shared_ptr<IGraph> graph)
    {
        if (graph != nullptr && m_renderMain != nullptr)
        {
            critical_section::scoped_lock lock(m_renderMain->GetCriticalSection());

            for (auto variablePair : Variables)
            {
                graph->SetArgValue(variablePair->Key->Data(), variablePair->Value->Value);
            }
        }
    }

    shared_ptr<IGraph> Grapher::GetGraph(Equation ^ equation)
    {
        shared_ptr<Graphing::IGraph> graph = m_solver->CreateGrapher();

        wstring request = s_getGraphOpeningTags;
        request += equation->GetRequest()->Data();
        request += s_getGraphClosingTags;

        if (unique_ptr<IExpression> graphExpression = m_solver->ParseInput(request, m_errorCode, m_errorType))
        {
            if (graph->TryInitialize(graphExpression.get()))
            {
                return graph;
            }
        }

        return nullptr;
    }

    void Grapher::UpdateVariables()
    {
        auto updatedVariables = ref new Map<String ^, Variable ^>();

        if (m_graph)
        {
            auto graphVariables = m_graph->GetVariables();

            for (auto graphVar : graphVariables)
            {
                if (graphVar->GetVariableName() != s_X && graphVar->GetVariableName() != s_Y)
                {
                    auto key = ref new String(graphVar->GetVariableName().data());
                    double value = 1.0;

                    Variable ^ variable;

                    if (Variables->HasKey(key))
                    {
                        variable = Variables->Lookup(key);
                    }

                    if (variable == nullptr)
                    {
                        variable = ref new Variable(1.0);
                    }

                    updatedVariables->Insert(key, variable);
                }
            }
        }

        if (Variables->Size != updatedVariables->Size)
        {
            TraceLogger::GetInstance()->LogVariableCountChanged(updatedVariables->Size);
        }

        Variables = updatedVariables;
        VariablesUpdated(this, Variables);
    }

    void Grapher::SetVariable(Platform::String ^ variableName, double newValue)
    {
        if (!Variables->HasKey(variableName))
        {
            Variables->Insert(variableName, ref new Variable(newValue));
        }

        if (m_graph != nullptr && m_renderMain != nullptr)
        {
            auto workItemHandler = ref new WorkItemHandler([this, variableName, newValue](IAsyncAction ^ action) {
                m_renderMain->GetCriticalSection().lock();
                m_graph->SetArgValue(variableName->Data(), newValue);
                m_renderMain->GetCriticalSection().unlock();

                m_renderMain->RunRenderPass();
            });

            ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::None);
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
                auto lineColor = eq->LineColor;
                graphColors.emplace_back(lineColor.R, lineColor.G, lineColor.B, lineColor.A);

                if (eq->GraphedEquation)
                {
                    if (!eq->HasGraphError && eq->IsSelected)
                    {
                        eq->GraphedEquation->TrySelectEquation();
                    }

                    eq->GraphedEquation->GetGraphEquationOptions()->SetLineStyle(static_cast<::Graphing::Renderer::LineStyle>(eq->EquationStyle));
                    eq->GraphedEquation->GetGraphEquationOptions()->SetLineWidth(LineWidth);
                    eq->GraphedEquation->GetGraphEquationOptions()->SetSelectedEquationLineWidth(LineWidth + ((LineWidth <= 2) ? 1 : 2));
                }
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

    void Grapher::OnForceProportionalAxesPropertyChanged(bool /*oldValue*/, bool newValue)
    {
        m_calculatedForceProportional = newValue;
        TryUpdateGraph(false);
    }

    void Grapher::OnUseCommaDecimalSeperatorPropertyChanged(bool oldValue, bool newValue)
    {
        if (newValue)
        {
            m_solver->ParsingOptions().SetLocalizationType(::LocalizationType::DecimalCommaAndListSemicolon);
            m_solver->FormatOptions().SetLocalizationType(::LocalizationType::DecimalCommaAndListSemicolon);
        }
        else
        {
            m_solver->ParsingOptions().SetLocalizationType(::LocalizationType::DecimalPointAndListComma);
            m_solver->FormatOptions().SetLocalizationType(::LocalizationType::DecimalPointAndListComma);
        }
    }

    void Grapher::OnPointerEntered(PointerRoutedEventArgs ^ e)
    {
        if (m_renderMain)
        {
            OnPointerMoved(e);

            e->Handled = true;
        }
    }

    void Grapher::UpdateTracingChanged()
    {
        if (m_renderMain->Tracing)
        {
            TracingChangedEvent(true);
            TracingValueChangedEvent(m_renderMain->XTraceValue, m_renderMain->YTraceValue);
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
            m_renderMain->DrawNearestPoint = true;
            Point currPosition = e->GetCurrentPoint(/* relativeTo */ this)->Position;

            if (m_renderMain->ActiveTracing)
            {
                PointerValueChangedEvent(currPosition);
                ActiveTraceCursorPosition = currPosition;

                if (m_cachedCursor == nullptr)
                {
                    m_cachedCursor = ::CoreWindow::GetForCurrentThread()->PointerCursor;
                    ::CoreWindow::GetForCurrentThread()->PointerCursor = nullptr;
                }
            }
            else if (m_cachedCursor != nullptr)
            {
                m_renderMain->PointerLocation = currPosition;

                ::CoreWindow::GetForCurrentThread()->PointerCursor = m_cachedCursor;
                m_cachedCursor = nullptr;

                UpdateTracingChanged();
            }
            else
            {
                m_renderMain->PointerLocation = currPosition;
                UpdateTracingChanged();
            }

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

        if (m_cachedCursor != nullptr)
        {
            ::CoreWindow::GetForCurrentThread()->PointerCursor = m_cachedCursor;
            m_cachedCursor = nullptr;
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
        GraphViewChangedEvent(this, GraphViewChangedReason::Manipulation);

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

                    m_renderMain->GetCriticalSection().lock();

                    if (FAILED(renderer->MoveRangeByRatio(translationX, translationY)))
                    {
                        m_renderMain->GetCriticalSection().unlock();
                        return;
                    }

                    m_renderMain->GetCriticalSection().unlock();
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

                    m_renderMain->GetCriticalSection().lock();

                    if (FAILED(renderer->ScaleRange(centerX, centerY, scale)))
                    {
                        m_renderMain->GetCriticalSection().unlock();
                        return;
                    }

                    m_renderMain->GetCriticalSection().unlock();
                    needsRenderPass = true;
                }

                if (needsRenderPass)
                {
                    m_renderMain->RunRenderPass();
                    GraphViewChangedEvent(this, GraphViewChangedReason::Manipulation);
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
                shared_ptr<Graphing::IBitmap> BitmapOut;
                bool hasSomeMissingDataOut = false;
                HRESULT hr = E_FAIL;
                hr = renderer->GetBitmap(BitmapOut, hasSomeMissingDataOut);
                if (SUCCEEDED(hr))
                {
                    // Get the raw data
                    vector<BYTE> byteVector = BitmapOut->GetData();
                    auto arr = ArrayReference<BYTE>(byteVector.data(), (unsigned int)byteVector.size());

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
        PointerValueChangedEvent(curPos);
    }
}

String ^ Grapher::ConvertToLinear(String ^ mmlString)
{
    m_solver->FormatOptions().SetFormatType(FormatType::LinearInput);

    auto expression = m_solver->ParseInput(mmlString->Data(), m_errorCode, m_errorType);
    auto linearExpression = m_solver->Serialize(expression.get());

    m_solver->FormatOptions().SetFormatType(s_defaultFormatType);

    return ref new String(linearExpression.c_str());
}

String ^ Grapher::FormatMathML(String ^ mmlString)
{
    auto expression = m_solver->ParseInput(mmlString->Data(), m_errorCode, m_errorType);
    auto formattedExpression = m_solver->Serialize(expression.get());
    return ref new String(formattedExpression.c_str());
}

void Grapher::OnAxesColorPropertyChanged(Windows::UI::Color /*oldValue*/, Windows::UI::Color newValue)
{
    if (m_graph)
    {
        auto axesColor = Graphing::Color(newValue.R, newValue.G, newValue.B, newValue.A);
        m_graph->GetOptions().SetAxisColor(axesColor);
        m_graph->GetOptions().SetFontColor(axesColor);
    }
}

void Grapher::OnGraphBackgroundPropertyChanged(Windows::UI::Color /*oldValue*/, Windows::UI::Color newValue)
{
    if (m_renderMain)
    {
        m_renderMain->BackgroundColor = newValue;
    }
    if (m_graph)
    {
        auto color = Graphing::Color(newValue.R, newValue.G, newValue.B, newValue.A);
        m_graph->GetOptions().SetBackColor(color);
        m_graph->GetOptions().SetBoxColor(color);
    }
}

void Grapher::OnGridLinesColorPropertyChanged(Windows::UI::Color /*oldValue*/, Windows::UI::Color newValue)
{
    if (m_renderMain != nullptr && m_graph != nullptr)
    {
        auto gridLinesColor = Graphing::Color(newValue.R, newValue.G, newValue.B, newValue.A);
        m_graph->GetOptions().SetGridColor(gridLinesColor);
        m_renderMain->RunRenderPassAsync();
    }
}

void Grapher::OnLineWidthPropertyChanged(double oldValue, double newValue)
{
    if (m_graph)
    {
        UpdateGraphOptions(m_graph->GetOptions(), GetGraphableEquations());
        if (m_renderMain)
        {
            m_renderMain->SetPointRadius(LineWidth + 1);
            m_renderMain->RunRenderPass();

            TraceLogger::GetInstance()->LogLineWidthChanged();
        }
    }
}

optional<vector<shared_ptr<Graphing::IEquation>>> Grapher::TryInitializeGraph(bool keepCurrentView, const IExpression* graphingExp)
{
    critical_section::scoped_lock lock(m_renderMain->GetCriticalSection());
    if (keepCurrentView || IsKeepCurrentView)
    {
        auto renderer = m_graph->GetRenderer();
        double xMin, xMax, yMin, yMax;
        renderer->GetDisplayRanges(xMin, xMax, yMin, yMax);
        auto initResult = m_graph->TryInitialize(graphingExp);
        if (initResult != nullopt)
        {
            if (IsKeepCurrentView)
            {
                // PrepareGraph() populates the values of the graph after TryInitialize but before rendering. This allows us to get the range of the graph to be rendered.
                if (SUCCEEDED(renderer->PrepareGraph()))
                {
                    // Get the initial display ranges from the graph that was just initialized to be used in ResetGrid if they user clicks the GraphView button.
                    renderer->GetDisplayRanges(m_initialDisplayRangeXMin, m_initialDisplayRangeXMax, m_initialDisplayRangeYMin, m_initialDisplayRangeYMax);
                    m_resetUsingInitialDisplayRange = true;
                }
            }

            renderer->SetDisplayRanges(xMin, xMax, yMin, yMax);
        }

        return initResult;
    }
    else
    {
        m_resetUsingInitialDisplayRange = false;
        return m_graph->TryInitialize(graphingExp);
    }
}
