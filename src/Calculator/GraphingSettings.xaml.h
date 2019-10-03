//
// MyUserControl.xaml.h
// Declaration of the MyUserControl class
//

#pragma once

#include "..\..\src\GraphingInterfaces\IMathSolver.h"

#include "CalcViewModel/Common/Utils.h"

#include "GraphingSettings.g.h"
#include <CalcViewModel\GraphingCalculator\GraphingCalculatorViewModel.h>

namespace CalculatorApp
{
    namespace Controls
    {
        [Windows::Foundation::Metadata::WebHostHidden] public ref class GraphingSettings sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            GraphingSettings();

        public:
            property CalculatorApp::ViewModel::ViewStateViewModel^ ViewState;

            OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);

        private:
            void TrigUnitModeClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

            CalculatorApp::ViewModel::ViewStateViewModel ^ ViewModel;

            void GraphingCalculator_DataContextChanged(Windows::UI::Xaml::FrameworkElement ^ sender, Windows::UI::Xaml::DataContextChangedEventArgs ^ args);

            void OnPropertyChanged(Platform::String ^ propertyName)
            {
                OutputDebugString(L"OnPropertyChanged");
                // VariableUpdated(this, VariableChangedEventArgs{ Name, Value });
            }
            void OnTextChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs ^ e);


            GraphControl::Grapher^ m_ParentGrapher;

            std::mutex m_updating;
            bool m_loading;

            double
                m_currentAspectRatio,
                m_width,
                m_height,
                m_xMin,
                m_yMin,
                m_xMax,
                m_yMax;

            void OnLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        };
    }
}
