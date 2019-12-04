// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GraphingCalculator.xaml.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "CalcViewModel/Common/LocalizationSettings.h"
#include "CalcViewModel/Common/LocalizationStringUtil.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "CalcViewModel/Common/Automation/NarratorAnnouncement.h"
#include "CalcViewModel/Common/Automation/NarratorNotifier.h"
#include "Controls/CalculationResult.h"
#include "CalcManager/NumberFormattingUtils.h"
#include "Calculator/Controls/EquationTextBox.h"
#include "Calculator/Views/GraphingCalculator/EquationInputArea.xaml.h"
#include "CalcViewModel/Common/Utils.h"
#include "GraphingSettings.xaml.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::Automation;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace CalcManager::NumberFormattingUtils;
using namespace concurrency;
using namespace GraphControl;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace std::chrono;
using namespace Utils;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Popups;

constexpr auto sc_ViewModelPropertyName = L"ViewModel";

DEPENDENCY_PROPERTY_INITIALIZATION(GraphingCalculator, IsSmallState);

GraphingCalculator::GraphingCalculator()    
{
    Equation::RegisterDependencyProperties();
    Grapher::RegisterDependencyProperties();
    InitializeComponent();

    DataTransferManager ^ dataTransferManager = DataTransferManager::GetForCurrentView();

    // Register the current control as a share source.
    m_dataRequestedToken = dataTransferManager->DataRequested +=
        ref new TypedEventHandler<DataTransferManager ^, DataRequestedEventArgs ^>(this, &GraphingCalculator::OnDataRequested);

    // Request notifications when we should be showing the trace values
    GraphingControl->TracingChangedEvent += ref new TracingChangedEventHandler(this, &GraphingCalculator::OnShowTracePopupChanged);

    // And when the actual trace value changes
    GraphingControl->TracingValueChangedEvent += ref new TracingValueChangedEventHandler(this, &GraphingCalculator::OnTracePointChanged);
}

void GraphingCalculator::OnShowTracePopupChanged(bool newValue)
{
    if ((TraceValuePopup->Visibility == ::Visibility::Visible) != newValue)
    {
        TraceValuePopup->Visibility = newValue ? ::Visibility::Visible : ::Visibility::Collapsed;
    }
}

void GraphingCalculator::GraphingCalculator_DataContextChanged(FrameworkElement ^ sender, DataContextChangedEventArgs ^ args)
{
    if (ViewModel != nullptr)
    {
        if (m_vectorChangedToken.Value != 0)
        {
            ViewModel->Equations->VectorChanged -= m_vectorChangedToken;
            m_vectorChangedToken.Value = 0;
        }

        if (m_variableUpdatedToken.Value != 0)
        {
            ViewModel->VariableUpdated -= m_variableUpdatedToken;
            m_variableUpdatedToken.Value = 0;
        }
    }

    ViewModel = dynamic_cast<GraphingCalculatorViewModel ^>(args->NewValue);

    m_vectorChangedToken = ViewModel->Equations->VectorChanged +=
        ref new VectorChangedEventHandler<EquationViewModel ^>(this, &GraphingCalculator::OnEquationsVectorChanged);

    m_variableUpdatedToken = ViewModel->VariableUpdated +=
        ref new EventHandler<VariableChangedEventArgs>(this, &CalculatorApp::GraphingCalculator::OnVariableChanged);

    // Let the graph settings know who it's parent is.
    GraphSettings->DataContext = GraphingControl;
}

void GraphingCalculator::OnEquationsVectorChanged(IObservableVector<EquationViewModel ^> ^ sender, IVectorChangedEventArgs ^ event)
{
    // If an item is already added to the graph, changing it should automatically trigger a graph update
    if (event->CollectionChange == ::CollectionChange::ItemChanged)
    {
        return;
    }

    // Do not plot the graph if we are removing an empty equation, just remove it
    if (event->CollectionChange == ::CollectionChange::ItemRemoved)
    {
        auto itemToRemove = GraphingControl->Equations->GetAt(event->Index);

        if (itemToRemove->Expression->IsEmpty())
        {
            GraphingControl->Equations->RemoveAt(event->Index);

            return;
        }
    }

    // Do not plot the graph if we are adding an empty equation, just add it
    if (event->CollectionChange == ::CollectionChange::ItemInserted)
    {
        auto itemToAdd = sender->GetAt(event->Index);

        if (itemToAdd->Expression->IsEmpty())
        {
            GraphingControl->Equations->Append(itemToAdd->GraphEquation);

            return;
        }
    }

    // We are either adding or removing a valid equation, or resetting the collection. We will need to plot the graph
    GraphingControl->Equations->Clear();

    for (auto equationViewModel : ViewModel->Equations)
    {
        GraphingControl->Equations->Append(equationViewModel->GraphEquation);
    }

    GraphingControl->PlotGraph();
}

void GraphingCalculator::OnTracePointChanged(Windows::Foundation::Point newPoint)
{
    wstringstream traceValueString;

    // TODO: The below precision should ideally be dynamic based on the current scale of the graph.
    traceValueString << "x=" << fixed << setprecision(1) << newPoint.X << ", y=" << fixed << setprecision(1) << newPoint.Y;

    TraceValue->Text = ref new String(traceValueString.str().c_str());

    auto peer = FrameworkElementAutomationPeer::FromElement(TraceValue);

    if (peer != nullptr)
    {
        peer->RaiseAutomationEvent(AutomationEvents::LiveRegionChanged);
    }

    PositionGraphPopup();
}

GraphingCalculatorViewModel ^ GraphingCalculator::ViewModel::get()
{
    return m_viewModel;
}

void GraphingCalculator::ViewModel::set(GraphingCalculatorViewModel ^ vm)
{
    if (m_viewModel != vm)
    {
        m_viewModel = vm;
        RaisePropertyChanged(StringReference(sc_ViewModelPropertyName));
    }
}

void CalculatorApp::GraphingCalculator::OnShareClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    // Ask the OS to start a share action.
    DataTransferManager::ShowShareUI();
}

// When share is invoked (by the user or programmatically) the event handler we registered will be called to populate the data package with the
// data to be shared. We will request the current graph image from the grapher as a stream that will pass to the share request.
void GraphingCalculator::OnDataRequested(DataTransferManager ^ sender, DataRequestedEventArgs ^ args)
{
    auto resourceLoader = ResourceLoader::GetForCurrentView();

    try
    {
        std::wstringstream rawHtml;
        std::wstringstream equationHtml;

        rawHtml << L"<p><img src='graph.png' width='600' alt='" << resourceLoader->GetString(L"GraphImageAltText")->Data() << "'></p>";

        auto equations = ViewModel->Equations;
        bool hasEquations = false;

        if (equations->Size > 0)
        {
            equationHtml << L"<span style=\"color: rgb(68, 114, 196); font-style: bold; font-size : 13pt;\">";
            equationHtml << resourceLoader->GetString(L"EquationsShareHeader")->Data();
            equationHtml << L"</span>";
            equationHtml << L"<table cellpadding=\"0\">";

            for (auto equation : equations)
            {
                auto expression = equation->Expression;
                if (expression->IsEmpty())
                {
                    continue;
                }

                auto color = equation->LineColor->Color;
                hasEquations = true;

                expression = GraphingControl->ConvertToLinear(expression);

                std::wstringstream equationColorHtml;
                equationColorHtml << L"color:rgb(" << color.R.ToString()->Data() << L"," << color.G.ToString()->Data() << L"," << color.B.ToString()->Data()
                                  << L");";

                equationHtml << L"<tr><td><span style=\"line-height: 0; font-size: 24pt;" << equationColorHtml.str()
                             << L"\">&#x25A0;</span></td><td><div style=\"margin: 4pt 0pt 0pt 0pt; \">";
                equationHtml << EscapeHtmlSpecialCharacters(expression)->Data();
                equationHtml << L"</div></td>";
            }
            equationHtml << L"</table>";
        }

        if (hasEquations)
        {
            rawHtml << equationHtml.str();
        }

        auto variables = ViewModel->Variables;

        if (variables->Size > 0)
        {
            auto localizedSeperator = LocalizationSettings::GetInstance().GetListSeparator() + L" ";

            rawHtml << L"<span style=\"color: rgb(68, 114, 196); font-style: bold; font-size: 13pt;\">";
            rawHtml << resourceLoader->GetString(L"VariablesShareHeader")->Data();
            rawHtml << L"</span><br><span>";

            for (unsigned i = 0; i < variables->Size; i++)
            {
                auto name = variables->GetAt(i)->Name;
                auto value = variables->GetAt(i)->Value;

                rawHtml << name->Data();
                rawHtml << L"=";
                auto formattedValue = to_wstring(value);
                TrimTrailingZeros(formattedValue);
                rawHtml << formattedValue;

                if (variables->Size - 1 != i)
                {
                    rawHtml << localizedSeperator;
                }
            }

            rawHtml << L"</span>";
        }

        rawHtml << L"<br><br>";

        // Shortcut to the request data
        auto requestData = args->Request->Data;

        DataPackage ^ dataPackage = ref new DataPackage();
        auto html = HtmlFormatHelper::CreateHtmlFormat(ref new String(rawHtml.str().c_str()));

        requestData->Properties->Title = resourceLoader->GetString(L"ShareActionTitle");

        requestData->SetHtmlFormat(html);

        auto bitmapStream = GraphingControl->GetGraphBitmapStream();

        requestData->ResourceMap->Insert(ref new String(L"graph.png"), bitmapStream);

        // Set the thumbnail image (in case the share target can't handle HTML)
        requestData->Properties->Thumbnail = bitmapStream;
    }
    catch (Exception ^ ex)
    {
        TraceLogger::GetInstance()->LogPlatformException(ViewMode::Graphing, __FUNCTIONW__, ex);

        // Something went wrong, notify the user.

        auto errDialog = ref new ContentDialog();
        errDialog->Content = resourceLoader->GetString(L"ShareActionErrorMessage");
        errDialog->CloseButtonText = resourceLoader->GetString(L"ShareActionErrorOk");
        errDialog->ShowAsync();
    }
}

void GraphingCalculator::GraphingControl_VariablesUpdated(Object ^, Object ^)
{
    m_viewModel->UpdateVariables(GraphingControl->Variables);
}

void GraphingCalculator::OnVariableChanged(Platform::Object ^ sender, VariableChangedEventArgs args)
{
    GraphingControl->SetVariable(args.variableName, args.newValue);
}

void GraphingCalculator::OnZoomInCommand(Object ^ /* parameter */)
{
    GraphingControl->ZoomFromCenter(zoomInScale);
}

void GraphingCalculator::OnZoomOutCommand(Object ^ /* parameter */)
{
    GraphingControl->ZoomFromCenter(zoomOutScale);
}

void GraphingCalculator::OnZoomResetCommand(Object ^ /* parameter */)
{
    GraphingControl->ResetGrid();
}

String ^ GraphingCalculator::GetTracingLegend(Platform::IBox<bool> ^ isTracing)
{
    auto resProvider = AppResourceProvider::GetInstance();
    return isTracing != nullptr && isTracing->Value ? resProvider->GetResourceString(L"disableTracingButtonToolTip")
                                                    : resProvider->GetResourceString(L"enableTracingButtonToolTip");
}

void GraphingCalculator::GraphingControl_LostFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    // If the graph is losing focus while we are in active tracing we need to turn it off so we don't try to eat keys in other controls.
    if (GraphingControl->ActiveTracing)
    {
        if (ActiveTracing->Equals(FocusManager::GetFocusedElement()) && ActiveTracing->IsPressed)
        {
            m_ActiveTracingPointerCaptureLost = ActiveTracing->PointerCaptureLost +=
                ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &CalculatorApp::GraphingCalculator::ActiveTracing_PointerCaptureLost);
        }
        else
        {
            GraphingControl->ActiveTracing = false;
            OnShowTracePopupChanged(false);
        }
    }
}

void CalculatorApp::GraphingCalculator::ActiveTracing_PointerCaptureLost(Platform::Object ^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e)
{
    if (m_ActiveTracingPointerCaptureLost.Value != 0)
    {
        ActiveTracing->PointerCaptureLost -= m_ActiveTracingPointerCaptureLost;
        m_ActiveTracingPointerCaptureLost.Value = 0;
    }

    if (GraphingControl->ActiveTracing)
    {
        GraphingControl->ActiveTracing = false;
        OnShowTracePopupChanged(false);
    }
}

void GraphingCalculator::GraphingControl_LosingFocus(UIElement ^ sender, LosingFocusEventArgs ^ args)
{
    auto newFocusElement = dynamic_cast<FrameworkElement ^>(args->NewFocusedElement);
    if (newFocusElement == nullptr || newFocusElement->Name == nullptr)
    {
        // Because clicking on the swap chain panel will try to move focus to a control that can't actually take focus
        // we will get a null destination.  So we are going to try and cancel that request.
        // If the destination is not in our application we will also get a null destination but the cancel will fail so it doesn't hurt to try.
        args->TryCancel();
    }
}

void GraphingCalculator::OnEquationKeyGraphFeaturesRequested(Object ^ sender, RoutedEventArgs ^ e)
{
    auto equationViewModel = static_cast<EquationViewModel ^>(sender);
    GraphingControl->AnalyzeEquation(equationViewModel->GraphEquation);
    equationViewModel->PopulateKeyGraphFeatures();
    IsKeyGraphFeaturesVisible = true;
}

void GraphingCalculator::OnKeyGraphFeaturesClosed(Object ^ sender, RoutedEventArgs ^ e)
{
    IsKeyGraphFeaturesVisible = false;
}

Visibility GraphingCalculator::ShouldDisplayPanel(bool isSmallState, bool isEquationModeActivated, bool isGraphPanel)
{
    return (!isSmallState || isEquationModeActivated ^ isGraphPanel) ? ::Visibility::Visible : ::Visibility::Collapsed;
}

Platform::String ^ GraphingCalculator::GetInfoForSwitchModeToggleButton(bool isChecked)
{
    if (isChecked)
    {
        return AppResourceProvider::GetInstance()->GetResourceString(L"GraphSwitchToGraphMode");
    }
    else
    {
        return AppResourceProvider::GetInstance()->GetResourceString(L"GraphSwitchToEquationMode");
    }
}

void GraphingCalculator::SwitchModeToggleButton_Checked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    auto narratorNotifier = ref new NarratorNotifier();
    String ^ announcementText;
    if (SwitchModeToggleButton->IsChecked->Value)
    {
        announcementText = AppResourceProvider::GetInstance()->GetResourceString(L"GraphSwitchedToEquationModeAnnouncement");
    }
    else
    {
        announcementText = AppResourceProvider::GetInstance()->GetResourceString(L"GraphSwitchedToGraphModeAnnouncement");
    }

    auto announcement = CalculatorAnnouncement::GetGraphModeChangedAnnouncement(announcementText);
    narratorNotifier->Announce(announcement);
}

void GraphingCalculator::PositionGraphPopup()
{
    if (GraphingControl->TraceLocation.X + 15 + TraceValuePopup->ActualWidth >= GraphingControl->ActualWidth)
    {
        TraceValuePopupTransform->X = (int)GraphingControl->TraceLocation.X - 15 - TraceValuePopup->ActualWidth;
    }
    else
    {
        TraceValuePopupTransform->X = (int)GraphingControl->TraceLocation.X + 15;
    }

    if (GraphingControl->TraceLocation.Y >= 30)
    {
        TraceValuePopupTransform->Y = (int)GraphingControl->TraceLocation.Y - 30;
    }
    else
    {
        TraceValuePopupTransform->Y = (int)GraphingControl->TraceLocation.Y;
    }
}

void GraphingCalculator::TraceValuePopup_SizeChanged(Object ^ sender, SizeChangedEventArgs ^ e)
{
    PositionGraphPopup();
}

::Visibility GraphingCalculator::ManageEditVariablesButtonVisibility(unsigned int numberOfVariables)
{
    return numberOfVariables == 0 ? ::Visibility::Collapsed : ::Visibility::Visible;
}

void CalculatorApp::GraphingCalculator::ActiveTracing_Checked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    GraphingControl->Focus(::FocusState::Programmatic);

    m_activeTracingKeyUpToken = Window::Current->CoreWindow->KeyUp +=
        ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^>(
            this, &CalculatorApp::GraphingCalculator::ActiveTracing_KeyUp);

    KeyboardShortcutManager::IgnoreEscape(false);
}

void CalculatorApp::GraphingCalculator::ActiveTracing_Unchecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (m_ActiveTracingPointerCaptureLost.Value != 0)
    {
        ActiveTracing->PointerCaptureLost -= m_ActiveTracingPointerCaptureLost;
        m_ActiveTracingPointerCaptureLost.Value = 0;
    }

    if (m_activeTracingKeyUpToken.Value != 0)
    {
        Window::Current->CoreWindow->KeyUp -= m_activeTracingKeyUpToken;
        m_activeTracingKeyUpToken.Value = 0;
    }
    KeyboardShortcutManager::HonorEscape();
}

void CalculatorApp::GraphingCalculator::ActiveTracing_KeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args)
{
    if (args->VirtualKey == VirtualKey::Escape)
    {
        GraphingControl->ActiveTracing = false;
        args->Handled = true;
    }
}
