
#include "pch.h"
#include "GraphingCalculator.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "CalcViewModel/Common/LocalizationSettings.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "Controls/CalculationResult.h"
#include "CalcManager/NumberFormattingUtils.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
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
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Popups;

constexpr auto sc_ViewModelPropertyName = L"ViewModel";

GraphingCalculator::GraphingCalculator()
    : ActiveTracingOn(false)
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
    if (TraceValuePopup->IsOpen != newValue)
    {
        TraceValuePopup->IsOpen = newValue;
        if (TraceValuePopup->IsOpen)
        {
            // Set the keyboard focus to the graph control so we can use the arrow keys safely.
            GraphingControl->Focus(::FocusState::Programmatic);
        }
    }
}

void GraphingCalculator::GraphingCalculator_DataContextChanged(FrameworkElement ^ sender, DataContextChangedEventArgs ^ args)
{
    ViewModel = dynamic_cast<GraphingCalculatorViewModel ^>(args->NewValue);

    ViewModel->VariableUpdated += ref new EventHandler<VariableChangedEventArgs>(this, &CalculatorApp::GraphingCalculator::OnVariableChanged);
}

void GraphingCalculator::OnTracePointChanged(Windows::Foundation::Point newPoint)
{
    TraceValuePopupTransform->X = (int)GraphingControl->TraceLocation.X + 15;
    TraceValuePopupTransform->Y = (int)GraphingControl->TraceLocation.Y - 30;

    TraceValue->Text = "(" + newPoint.X.ToString() + ", " + newPoint.Y.ToString() + ")";
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
        std::wstring rawHtml;
        std::wstring graphImageHtml = L"<p><img src='graph.png' width='600'></p>";

        auto equations = ViewModel->Equations;
        bool hasEquations = false;

        if (equations->Size > 0)
        {
            rawHtml += L"<span style=\"color: rgb(68, 114, 196); font-family: calibri, sans-serif; font-style: bold; font-size : 13pt;\">";
            rawHtml += resourceLoader->GetString(L"EquationsShareHeader")->Data();
            rawHtml += L"</span>";
            rawHtml += L"<table cellpadding=\"0\">";
            for (unsigned i = 0; i < equations->Size; i++)
            {
                auto expression = equations->GetAt(i)->Expression;
                auto color = equations->GetAt(i)->LineColor->Color;

                if (expression->IsEmpty())
                {
                    continue;
                }

                hasEquations = true;

                expression = GraphingControl->ConvertToLinear(expression);

                std::wstring equatinColorHtml = L"color:rgb(";
                equatinColorHtml += color.R.ToString()->Data();
                equatinColorHtml += L",";
                equatinColorHtml += color.G.ToString()->Data();
                equatinColorHtml += L",";
                equatinColorHtml += color.B.ToString()->Data();
                equatinColorHtml += L");";

                rawHtml +=
                    L"<tr><td><span style=\"line-height: 0; font-size: 24pt;" + equatinColorHtml + L"\">&#x25A0;</span></td><td><div style=\"margin: 4px 0px 0px 0px; \">";
                rawHtml += expression->Data();
                rawHtml += L"</div></td>";
            }
            rawHtml += L"</table>";
        }

        if (hasEquations)
        {
            rawHtml = graphImageHtml + rawHtml;
        }
        else
        {
            rawHtml = graphImageHtml;
        }

        auto variables = ViewModel->Variables;

        if (variables->Size > 0)
        {
            auto localizedSeperator = LocalizationSettings::GetInstance().GetListSeparator() + L" ";

            rawHtml += L"<span style=\"color: rgb(68, 114, 196); font-family: calibri, sans-serif; font-style: bold; font-size: 13pt;\">";
            rawHtml += resourceLoader->GetString(L"VaiablesShareHeader")->Data();
            rawHtml += L"</span><br><span>";

            for (unsigned i = 0; i < variables->Size; i++)
            {
                auto name = variables->GetAt(i)->Name;
                auto value = variables->GetAt(i)->Value;

                rawHtml += name->Data();
                rawHtml += L"=";
                auto formattedValue = to_wstring(value);
                TrimTrailingZeros(formattedValue);
                rawHtml += formattedValue;

                if (variables->Size - 1 != i)
                {
                    rawHtml += localizedSeperator;
                }
            }

            rawHtml += L"</span>";
        }

        rawHtml += L"<br><br>";

        // Shortcut to the request data
        auto requestData = args->Request->Data;

        DataPackage ^ dataPackage = ref new DataPackage();
        auto html = HtmlFormatHelper::CreateHtmlFormat(ref new String(rawHtml.c_str()));

        requestData->Properties->Title = resourceLoader->GetString(L"ShareActionTitle");

        requestData->SetHtmlFormat(html);

        auto bitmapStream = GraphingControl->GetGraphBitmapStream();

        requestData->ResourceMap->Insert(ref new String(L"graph.png"), bitmapStream);

        // Set the thumbnail image (in case the share target can't handle HTML)
        requestData->Properties->Thumbnail = bitmapStream;
    }
    catch (Exception ^ ex)
    {
        TraceLogger::GetInstance().LogPlatformException(ViewMode::Graphing, __FUNCTIONW__, ex);

        // Something went wrong, notify the user.

        auto errDialog = ref new ContentDialog();
        errDialog->Content = resourceLoader->GetString(L"ShareActionErrorMessage");
        errDialog->CloseButtonText = resourceLoader->GetString(L"ShareActionErrorOk");
        errDialog->ShowAsync();
    }
}

void GraphingCalculator::GraphVariablesUpdated(Object ^, Object ^)
{
    m_viewModel->UpdateVariables(GraphingControl->Variables);
}

void GraphingCalculator::OnVariableChanged(Platform::Object ^ sender, VariableChangedEventArgs args)
{
    GraphingControl->SetVariable(args.variableName, args.newValue);
}

void GraphingCalculator::SubmitTextbox(TextBox ^ sender)
{
    auto variableViewModel = static_cast<VariableViewModel ^>(sender->DataContext);

    if (sender->Name == "ValueTextBox")
    {
        variableViewModel->SetValue(validateDouble(sender->Text, variableViewModel->Value));
    }
    else if (sender->Name == "MinTextBox")
    {
        variableViewModel->Min = validateDouble(sender->Text, variableViewModel->Min);
    }
    else if (sender->Name == "MaxTextBox")
    {
        variableViewModel->Max = validateDouble(sender->Text, variableViewModel->Max);
    }
    else if (sender->Name == "StepTextBox")
    {
        variableViewModel->Step = validateDouble(sender->Text, variableViewModel->Step);
    }
}

void GraphingCalculator::TextBoxLosingFocus(TextBox ^ sender, LosingFocusEventArgs ^)
{
    SubmitTextbox(sender);
}

void GraphingCalculator::TextBoxKeyDown(TextBox ^ sender, KeyRoutedEventArgs ^ e)
{
    if (e->Key == ::VirtualKey::Enter)
    {
        SubmitTextbox(sender);
    }
}

double GraphingCalculator::validateDouble(String ^ value, double defaultValue)
{
    try
    {
        return stod(value->Data());
    }
    catch (...)
    {
        return defaultValue;
    }
}

void GraphingCalculator::TextBoxGotFocus(TextBox ^ sender, RoutedEventArgs ^ e)
{
    sender->SelectAll();
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

void GraphingCalculator::OnActiveTracingClick(Object ^ sender, RoutedEventArgs ^ e)
{
    // The focus change to this button will have turned off the tracing if it was on
    ActiveTracingOn = !ActiveTracingOn;
    GraphingControl->ActiveTracing = ActiveTracingOn;
}

void GraphingCalculator::OnGraphLostFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    // If the graph is losing focus while we are in active tracing we need to turn it off so we don't try to eat keys in other controls.
    if (GraphingControl->ActiveTracing)
    {
        GraphingControl->ActiveTracing = false;
        OnShowTracePopupChanged(false);
    }
}

void GraphingCalculator::OnLosingFocus(UIElement ^ sender, LosingFocusEventArgs ^ args)
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
