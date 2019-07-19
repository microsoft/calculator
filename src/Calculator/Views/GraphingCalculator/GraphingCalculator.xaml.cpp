
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

    // Register the current control as a share source.
    dataRequestedToken = dataTransferManager->DataRequested += ref new TypedEventHandler<DataTransferManager^, DataRequestedEventArgs^>(this, &GraphingCalculator::OnDataRequested);
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
    // Ask the OS to start a share action.
    if (TheGrapher != nullptr)
    {
        DataTransferManager::ShowShareUI();
    }

}

// When share is invoked (by the user or programatically) the event handler we registered will be called to populate the datapackage with the
// data to be shared.
// TODO: Cleanup temp file
void GraphingCalculator::OnDataRequested(DataTransferManager^ sender, DataRequestedEventArgs^ args)
{
    if (TheGrapher != nullptr)
    {
        auto requestData = args->Request->Data;
        requestData->Properties->Title = L"Look what I graphed";

        WCHAR ShareFilePath[MAX_PATH] = L"";
        TheGrapher->GetShareFile(ShareFilePath, MAX_PATH);
        String^ s = TheGrapher->GetShareFile();

        // If we got a path back
        if (s->Length() > 0)
        {
            String^ path = ref new String(ShareFilePath);

            create_task(Windows::Storage::StorageFile::GetFileFromPathAsync(path)).then([requestData](Windows::Storage::StorageFile^ imageFile)
                {
                    auto imageStreamRef = RandomAccessStreamReference::CreateFromFile(imageFile);
                    requestData->Properties->Thumbnail = imageStreamRef;
                    requestData->SetBitmap(imageStreamRef);
                });
        }
    }
}

