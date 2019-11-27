// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "EquationStylePanelControl.g.h"
#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
	[Windows::Foundation::Metadata::WebHostHidden] public ref class EquationStylePanelControl sealed
	{
	public:
        EquationStylePanelControl();
        DEPENDENCY_PROPERTY_OWNER(EquationStylePanelControl);

        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(Windows::UI::Color, SelectedColor, Windows::UI::Colors::Black);
        DEPENDENCY_PROPERTY_WITH_DEFAULT(Windows::Foundation::Collections::IVector<Windows::UI::Xaml::Media::SolidColorBrush ^> ^, AvailableColors, nullptr);

	private:
        void SelectionChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^ e);
        void OnSelectedColorPropertyChanged(Windows::UI::Color oldColor, Windows::UI::Color newColor);
        void ColorChooserLoaded(
            Platform::Object ^ sender,
            Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SelectColor(Windows::UI::Color selectedColor);
    };
}
