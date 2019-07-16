#pragma once

#include "Views\GraphingCalculator\GraphingCalculator.g.h"
#include "CalcViewModel/GraphingCalculator/GraphingCalculatorViewModel.h"
#include "Views\NumberPad.xaml.h"

namespace CalculatorApp
{
    public ref class GraphingCalculator sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        GraphingCalculator();

        OBSERVABLE_OBJECT();

        property CalculatorApp::ViewModel::GraphingCalculatorViewModel^ ViewModel
        {
            CalculatorApp::ViewModel::GraphingCalculatorViewModel^ get();
            void set(CalculatorApp::ViewModel::GraphingCalculatorViewModel^ vm);
        }

    private:
        void GraphingCalculator_DataContextChanged(Windows::UI::Xaml::FrameworkElement^ sender, Windows::UI::Xaml::DataContextChangedEventArgs^ args);

    private:
        CalculatorApp::ViewModel::GraphingCalculatorViewModel^ m_viewModel;
		void OnShareClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

    private:
        Windows::Foundation::EventRegistrationToken dataRequestedToken{};
        Windows::Foundation::EventRegistrationToken targetApplicationChosenToken{};

        void OnDataRequested(Windows::ApplicationModel::DataTransfer::DataTransferManager^ sender, Windows::ApplicationModel::DataTransfer::DataRequestedEventArgs^ e);
        void OnTargetApplicationChosen(Windows::ApplicationModel::DataTransfer::DataTransferManager^ sender, Windows::ApplicationModel::DataTransfer::TargetApplicationChosenEventArgs^ e);
        void OnShareCompleted(Windows::ApplicationModel::DataTransfer::DataPackage^ sender, Windows::ApplicationModel::DataTransfer::ShareCompletedEventArgs^ e);
        // worker
        bool GetShareContent(Windows::ApplicationModel::DataTransfer::DataRequest^ request);

	};
}
