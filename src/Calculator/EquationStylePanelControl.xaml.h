// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "EquationStylePanelControl.g.h"
#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/Common/TraceLogger.h"

namespace CalculatorApp
{
	[Windows::Foundation::Metadata::WebHostHidden] public ref class EquationStylePanelControl sealed
	{
	public:
        EquationStylePanelControl();
        DEPENDENCY_PROPERTY_OWNER(EquationStylePanelControl);

        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(Windows::UI::Color, SelectedColor, Windows::UI::Colors::Black);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(GraphControl::EquationLineStyle, SelectedStyle, GraphControl::EquationLineStyle::Solid);
        DEPENDENCY_PROPERTY(int, SelectedColorIndex);
        DEPENDENCY_PROPERTY_WITH_DEFAULT(Windows::Foundation::Collections::IVector<Windows::UI::Xaml::Media::SolidColorBrush ^> ^, AvailableColors, nullptr);
        DEPENDENCY_PROPERTY(bool, EnableLineStylePicker);

        static Windows::UI::Xaml::Media::DoubleCollection ^ GetLinePattern(Platform::Object ^ line);
        static Platform::String ^ GetLineAutomationName(Platform::Object ^ line);
        static Platform::String ^ GetColorAutomationName(Windows::UI::Xaml::Media::Brush ^ color);

	private:
        void SelectionChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^ e);
        void OnSelectedColorPropertyChanged(Windows::UI::Color oldColor, Windows::UI::Color newColor);
        void ColorChooserLoaded(
            Platform::Object ^ sender,
            Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SelectColor(Windows::UI::Color selectedColor);
        void OnSelectedStylePropertyChanged(GraphControl::EquationLineStyle oldStyle, GraphControl::EquationLineStyle newStyle);
        void SelectStyle(GraphControl::EquationLineStyle selectedStyle);
        void StyleChooserBox_SelectionChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^ e);
        void StyleChooserBox_Loaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    };
}
