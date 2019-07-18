
#include "pch.h"
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
using namespace std::chrono;
using namespace Utils;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;

using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml::Media;

using namespace Windows::UI::Core;

namespace MathSolverEngine
{
    namespace Graph
    {
        namespace Renderer
        {
            struct IBitmap;

        }
    }
}

constexpr auto sc_ViewModelPropertyName = L"ViewModel";

GraphingCalculator::GraphingCalculator()
{
    Equation::RegisterDependencyProperties();
    Grapher::RegisterDependencyProperties();
    InitializeComponent();

    DataTransferManager^ dataTransferManager = DataTransferManager::GetForCurrentView();

    // Register the current page as a share source.
    dataRequestedToken = dataTransferManager->DataRequested += ref new TypedEventHandler<DataTransferManager^, DataRequestedEventArgs^>(this, &GraphingCalculator::OnDataRequested);

    // Request to be notified when the user chooses a share target app.
    targetApplicationChosenToken = dataTransferManager->TargetApplicationChosen += ref new TypedEventHandler<DataTransferManager^, TargetApplicationChosenEventArgs^>(this, &GraphingCalculator::OnTargetApplicationChosen);
}

void GraphingCalculator::GraphingCalculator_DataContextChanged(FrameworkElement^ sender, DataContextChangedEventArgs^ args)
{
    ViewModel = dynamic_cast<GraphingCalculatorViewModel^>(args->NewValue);
}

GraphingCalculatorViewModel^ GraphingCalculator::ViewModel::get()
{
    return m_viewModel;
}

void GraphingCalculator::ViewModel::set(GraphingCalculatorViewModel^ vm)
{
    if (m_viewModel != vm)
    {
        m_viewModel = vm;
        RaisePropertyChanged(StringReference(sc_ViewModelPropertyName));
    }
}


void CalculatorApp::GraphingCalculator::OnShareClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Notify the graphing component to start a share action.
    if (TheGrapher != nullptr)
    {
        DataTransferManager::ShowShareUI();
    }

}

//void GraphingCalculator::OnNavigatedFrom(NavigationEventArgs^ e)
//{
//    // Unregister our event handlers.
//    DataTransferManager^ dataTransferManager = DataTransferManager::GetForCurrentView();
//    dataTransferManager->DataRequested -= dataRequestedToken;
//    dataTransferManager->TargetApplicationChosen -= targetApplicationChosenToken;
//}

// When share is invoked (by the user or programatically) the event handler we registered will be called to populate the datapackage with the
// data to be shared.
void GraphingCalculator::OnDataRequested(DataTransferManager^ sender, DataRequestedEventArgs^ e)
{
    // Register to be notified if the share operation completes.
    e->Request->Data->ShareCompleted += ref new TypedEventHandler<DataPackage^, ShareCompletedEventArgs^>(this, &GraphingCalculator::OnShareCompleted);

    // Call the share worker
    GetShareContent(e->Request);
}

void GraphingCalculator::OnTargetApplicationChosen(DataTransferManager^ sender, TargetApplicationChosenEventArgs^ e)
{
    String^ shareCompletedStatus = "target chosen";

    //this->rootPage->NotifyUser("User chose " + e->ApplicationName, NotifyType::StatusMessage);
}

void GraphingCalculator::OnShareCompleted(DataPackage^ sender, ShareCompletedEventArgs^ e)
{
    String^ shareCompletedStatus = "Shared successfully. ";
}


// Used to get graph content to share by was of the OS share contract
// This is an OS callback
bool GraphingCalculator::GetShareContent(DataRequest^ request)
{
    bool succeeded = false;
    if (TheGrapher != nullptr)
    {
        String^ TextToShare = L"Here is some text";
        auto requestData = request->Data;
        requestData->Properties->Title = L"share text title";
        requestData->Properties->Description = L"Share text description"; // The description is optional.
        requestData->SetText(TextToShare);

        WCHAR ShareFilePath[MAX_PATH] = L"";
        succeeded = TheGrapher->GetShareFile(ShareFilePath, MAX_PATH);
        if (succeeded)
        {
            String^ path = ref new String(ShareFilePath);

            create_task(Windows::Storage::StorageFile::GetFileFromPathAsync(path)).then([requestData, &succeeded](Windows::Storage::StorageFile^ imageFile)
                {
                    auto imageStreamRef = RandomAccessStreamReference::CreateFromFile(imageFile);
                    requestData->Properties->Thumbnail = imageStreamRef;
                    requestData->SetBitmap(imageStreamRef);
                    succeeded = true;
                });
        }
    }

    return succeeded;
}

