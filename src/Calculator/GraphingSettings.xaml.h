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
        [Windows::Foundation::Metadata::WebHostHidden] public ref class GraphingSettings sealed
        {
        public:
            GraphingSettings();

        public:
            property CalculatorApp::ViewModel::ViewStateViewModel^ ViewState;

        private:
            void TrigUnitModeClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

            void GraphingCalculator_DataContextChanged(Windows::UI::Xaml::FrameworkElement ^ sender, Windows::UI::Xaml::DataContextChangedEventArgs ^ args);

            // Helper for updating the extents from the various ways we can trigger it.
            void UpdateExtents(Platform::Object ^ sender);


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

            void OnTextChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs ^ e);
            void OnLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnLosingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args);
            void OnKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);
        };
    }
}
