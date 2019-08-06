
#include "pch.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "GraphingCalculator.xaml.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "Controls/CalculationResult.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace concurrency;
using namespace GraphControl;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace std::chrono;
using namespace Utils;
using namespace Windows::ApplicationModel::DataTransfer;
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
{
    Equation::RegisterDependencyProperties();
    Grapher::RegisterDependencyProperties();
    InitializeComponent();

    DataTransferManager ^ dataTransferManager = DataTransferManager::GetForCurrentView();

    // Register the current control as a share source.
    m_dataRequestedToken = dataTransferManager->DataRequested += ref new TypedEventHandler<DataTransferManager ^, DataRequestedEventArgs ^>(this, &GraphingCalculator::OnDataRequested);

    // Request notifications when we should be showing the trace values
    m_showTracePopupChangedToken = GraphingControl->TracingChangedEvent += ref new TracingChangedEventHandler(this, &GraphingCalculator::OnShowTracePopupChanged);

    // And when the actual trace value changes
    m_tracePointChangedToken = GraphingControl->TracingValueChangedEvent += ref new TracingValueChangedEventHandler(this, &GraphingCalculator::OnTracePointChanged);
}

void GraphingCalculator::OnShowTracePopupChanged(bool newValue)
{
    TraceValuePopup->IsOpen = newValue;
}

void GraphingCalculator::GraphingCalculator_DataContextChanged(FrameworkElement ^ sender, DataContextChangedEventArgs ^ args)
{
    ViewModel = dynamic_cast<GraphingCalculatorViewModel ^>(args->NewValue);

    ViewModel->VariableUpdated += ref new EventHandler<VariableChangedEventArgs>(this, &CalculatorApp::GraphingCalculator::OnVariableChanged);
}

void GraphingCalculator::OnTracePointChanged(Windows::Foundation::Point newPoint)
{
    auto p = GraphingControl->TraceValue;
    auto l = GraphingControl->TraceLocation;
    TraceValuePopupTransform->X = (int)l.X + 15;
    TraceValuePopupTransform->Y = (int)l.Y - 30;

    TraceValue->Text = "x=" + newPoint.X.ToString() + ", y=" + newPoint.Y.ToString();
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
    try
    {
        // Get our title from the localized resources
        auto resourceLoader = Windows::ApplicationModel::Resources::ResourceLoader::GetForCurrentView();
        auto EmptyEquationString = resourceLoader->GetString(L"EmptyEquationString");

        std::wstring rawHtml = L"<p><img src='graph.png'></p>";

        auto equations = ViewModel->Equations;
        rawHtml += L"<p><table cellpadding=\"10\">";
        rawHtml += L"<col width=\"20\">";
        rawHtml += L"<row height=\"20\">";
        for (unsigned i = 0; i < equations->Size; i++)
        {
            auto expression = equations->GetAt(i)->Expression->Data();
            auto color = equations->GetAt(i)->LineColor;

            if (equations->GetAt(i)->Expression->Length() == 0)
            {
                expression = EmptyEquationString->Data();
            }

            rawHtml += L"<tr>";

            rawHtml += L"<td style=\"background-color:rgb(";
            rawHtml += color.R.ToString()->Data();
            rawHtml += L",";
            rawHtml += color.G.ToString()->Data();
            rawHtml += L",";
            rawHtml += color.B.ToString()->Data();
            rawHtml += L"); \">";
            rawHtml += L"</td>";
            rawHtml += L"<td>";
            rawHtml += expression;
            rawHtml += L"</td>";

            rawHtml += L"</tr>";
        }
        rawHtml += L"</table></p>";

        auto variables = ViewModel->Variables;
        rawHtml += L"<p><table cellpadding=\"10\">";
        rawHtml += L"<col width=\"20\">";
        rawHtml += L"<row height=\"20\">";
        for (unsigned i = 0; i < variables->Size; i++)
        {
            auto name = variables->GetAt(i)->Name;
            auto value = variables->GetAt(i)->Value;

            if (name->Length() >= 0)
            {
                rawHtml += L"<tr>";

                rawHtml += L"<td>";
                rawHtml += name->Data();
                rawHtml += L"</td>";
                rawHtml += L"<td>";
                rawHtml += std::to_wstring(value);
                rawHtml += L"</td>";

                rawHtml += L"</tr>";
            }
        }
        rawHtml += L"</table></p>";

        // Shortcut to the request data
        auto requestData = args->Request->Data;

        DataPackage ^ dataPackage = ref new DataPackage();
        auto html = HtmlFormatHelper::CreateHtmlFormat(ref new String(rawHtml.c_str()));

        auto titleString = resourceLoader->GetString(L"ShareActionTitle");
        requestData->Properties->Title = titleString;

        requestData->SetHtmlFormat(html);

        auto bitmapStream = GraphingControl->GetGraphBitmapStream();

        requestData->ResourceMap->Insert(ref new String(L"graph.png"), bitmapStream);

        // Set the thumbnail image (in case the share target can't handle HTML)
        requestData->Properties->Thumbnail = bitmapStream;

        // And the bitmap (in case the share target can't handle HTML)
        requestData->SetBitmap(bitmapStream);
    }
    catch (Exception ^ ex)
    {
        TraceLogger::GetInstance().LogPlatformException(__FUNCTIONW__, ex);

        // Something went wrong, notify the user.
        auto resourceLoader = Windows::ApplicationModel::Resources::ResourceLoader::GetForCurrentView();

        auto errorTitleString = resourceLoader->GetString(L"ShareActionErrorMessage");
        auto errorOkString = resourceLoader->GetString(L"ShareActionErrorOk");
        auto errDialog = ref new Windows::UI::Xaml::Controls::ContentDialog();

        errDialog->Content = errorTitleString;
        errDialog->CloseButtonText = errorOkString;
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
        variableViewModel->Step = validateDouble(sender->Text, variableViewModel->Step);
    }
    else if (sender->Name == "StepTextBox")
    {
        variableViewModel->Max = validateDouble(sender->Text, variableViewModel->Max);
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

void GraphingCalculator::OnActiveTracingClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    GraphingControl->ActiveTracing = !GraphingControl->ActiveTracing;
}
