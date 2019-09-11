// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "StylePanel.g.h"
#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class StylePanel sealed
	{
	public:
		StylePanel();
        DEPENDENCY_PROPERTY_OWNER(StylePanel);

        DEPENDENCY_PROPERTY_WITH_CALLBACK(Windows::UI::Xaml::Media::SolidColorBrush ^, SelectedColor);

	private:
        void SelectionChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^ e);
        void OnSelectedColorPropertyChanged(Windows::UI::Xaml::Media::SolidColorBrush ^ oldValue, Windows::UI::Xaml::Media::SolidColorBrush ^ newValue);
        Windows::UI::Xaml::Shapes::Ellipse ^ GetEllipse(Windows::UI::Xaml::Controls::GridViewItem ^ gridViewItem);
        void ColorChooserLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SelectColor(Windows::UI::Xaml::Media::SolidColorBrush ^ selectedColor);
    };
}
