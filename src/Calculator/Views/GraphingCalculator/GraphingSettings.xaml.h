//
// MyUserControl.xaml.h
// Declaration of the MyUserControl class
//

#pragma once

#include "CalcViewModel/Common/Utils.h"

#include "Views\GraphingCalculator\GraphingSettings.g.h"
#include <CalcViewModel\GraphingCalculator\GraphingCalculatorViewModel.h>

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden] public ref class GraphingSettings sealed
    {
    public:
        GraphingSettings();

    private:
        void TrigUnitModeClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        void GraphingCalculator_DataContextChanged(Windows::UI::Xaml::FrameworkElement ^ sender, Windows::UI::Xaml::DataContextChangedEventArgs ^ args);

        // Helper for updating the extents from the various ways we can trigger it.
        void UpdateExtents(Platform::Object ^ sender);

        GraphControl::Grapher ^ m_ParentGrapher;

        std::mutex m_updating;
        bool m_loading;

        double m_xMin, m_yMin, m_xMax, m_yMax;

        void OnLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnLosingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args);
        void OnKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);
        void OnPreserveAspectRatioClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    };
}
