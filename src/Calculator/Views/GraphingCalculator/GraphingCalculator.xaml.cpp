// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GraphingCalculator.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "CalcViewModel/Common/LocalizationSettings.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "Controls/CalculationResult.h"
#include "CalcManager/NumberFormattingUtils.h"
#include "Calculator/Controls/EquationTextBox.h"
#include "Calculator/Views/GraphingCalculator/EquationInputArea.xaml.h"
#include "CalcViewModel/Common/Utils.h"

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

using namespace Windows::UI::Popups;
constexpr auto sc_ViewModelPropertyName = L"ViewModel";

GraphingCalculator::GraphingCalculator()
    : ActiveTracingOn(false)
{
    Equation::RegisterDependencyProperties();
    Grapher::RegisterDependencyProperties();
    InitializeComponent();

    DataTransferManager ^ dataTransferManager = DataTransferManager::GetForCurrentView();

    m_dataRequestedToken = dataTransferManager->DataRequested +=

    m_drawChangedToken = GraphingControl->TracingChangedEvent += ref new TracingChangedEventHandler(this, &GraphingCalculator::OnDrawChanged);

    // And when the actual trace value changes
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
}

void GraphingCalculator::OnEquationsVectorChanged(IObservableVector<EquationViewModel ^> ^ sender, IVectorChangedEventArgs ^ event)
{
    if (event->CollectionChange != ::CollectionChange::ItemChanged)
    {
        GraphingControl->Equations->Clear();

        for (auto equationViewModel : ViewModel->Equations)
        {
            GraphingControl->Equations->Append(equationViewModel->GraphEquation);
        }

        GraphingControl->PlotGraph();
    }
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
        std::wstringstream rawHtml;
        std::wstringstream equationHtml;
        DataTransferManager::ShowShareUI();
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
//void GraphingCalculator::OnNavigatedFrom(NavigationEventArgs^ e)
        }
//    DataTransferManager^ dataTransferManager = DataTransferManager::GetForCurrentView();
//    dataTransferManager->DataRequested -= dataRequestedToken;
//    dataTransferManager->TargetApplicationChosen -= targetApplicationChosenToken;
//}
        if (hasEquations)
// When share is invoked (by the user or programatically) the event handler we registered will be called to populate the datapackage with the
// data to be shared.
void GraphingCalculator::OnDataRequested(DataTransferManager^ sender, DataRequestedEventArgs^ e)
        {
    // Register to be notified if the share operation completes.
    e->Request->Data->ShareCompleted += ref new TypedEventHandler<DataPackage^, ShareCompletedEventArgs^>(this, &GraphingCalculator::OnShareCompleted);
            rawHtml << equationHtml.str();
    // Call the share worker
    GetShareContent(e->Request);
        }

        auto variables = ViewModel->Variables;
void GraphingCalculator::OnTargetApplicationChosen(DataTransferManager^ sender, TargetApplicationChosenEventArgs^ e)
        if (variables->Size > 0)
        {
            auto localizedSeperator = LocalizationSettings::GetInstance().GetListSeparator() + L" ";
    String^ shareCompletedStatus = "target chosen";
            rawHtml << L"<span style=\"color: rgb(68, 114, 196); font-style: bold; font-size: 13pt;\">";
            rawHtml << resourceLoader->GetString(L"VariablesShareHeader")->Data();
            rawHtml << L"</span><br><span>";
            for (unsigned i = 0; i < variables->Size; i++)
                auto name = variables->GetAt(i)->Name;
                auto value = variables->GetAt(i)->Value;

                rawHtml << name->Data();
                rawHtml << L"=";
                auto formattedValue = to_wstring(value);
                TrimTrailingZeros(formattedValue);
                rawHtml << formattedValue;
void GraphingCalculator::OnShareCompleted(DataPackage^ sender, ShareCompletedEventArgs^ e)
                if (variables->Size - 1 != i)
                {
                }
            }

            rawHtml << L"</span>";
bool GraphingCalculator::GetShareContent(DataRequest^ request)

    WeakReference weakThis(this);
    bool succeeded = false;

        auto refThis = weakThis.Resolve<GraphingCalculator>();
        rawHtml << L"<br><br>";
        String^ TextToShare = L"Here is some text";
        requestData->Properties->Title = L"share text title";
        DataPackage ^ dataPackage = ref new DataPackage();
        auto html = HtmlFormatHelper::CreateHtmlFormat(ref new String(rawHtml.str().c_str()));
//    requestData->Properties->ContentSourceApplicationLink = ApplicationLink;

        auto bitmapStream = GraphingControl->GetGraphBitmapStream();
        requestData->ResourceMap->Insert(ref new String(L"graph.png"), bitmapStream);
                if (refThis->TheGrapher != nullptr)
        // Set the thumbnail image (in case the share target can't handle HTML)
        requestData->Properties->Thumbnail = bitmapStream;
    }
    catch (Exception ^ ex)
                {
        TraceLogger::GetInstance().LogPlatformException(ViewMode::Graphing, __FUNCTIONW__, ex);

        BitmapImage^ bitmapImage = ref new BitmapImage();

        errDialog->Content = resourceLoader->GetString(L"ShareActionErrorMessage");
        errDialog->CloseButtonText = resourceLoader->GetString(L"ShareActionErrorOk");
        errDialog->ShowAsync();
}
        TheGrapher->Share(bitmapImage);
void GraphingCalculator::GraphingControl_VariablesUpdated(Object ^, Object ^)
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
            TheGrapher->GetShareFile(ShareFilePath, MAX_PATH);

            //    requestData->SetBitmap(stream);
    if (sender->Name == "ValueTextBox")
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
}
            //auto imageFile = Windows::Storage::StorageFile::GetFileFromPathAsync(path)->GetResults();
void GraphingCalculator::TextBoxKeyDown(TextBox ^ sender, KeyRoutedEventArgs ^ e)
{
    if (e->Key == ::VirtualKey::Enter)
    {
        SubmitTextbox(sender);
    }
}
            //Windows::Storage::StorageFile^ imageFile = TheGrapher->GetShareFile();
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
        //
{
    // The focus change to this button will have turned off the tracing if it was on
    ActiveTracingOn = !ActiveTracingOn;
        }
        
void GraphingCalculator::GraphingControl_LostFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    // If the graph is losing focus while we are in active tracing we need to turn it off so we don't try to eat keys in other controls.
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
            //}//));
        
        
        return succeeded;
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

