// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "SettingsPage.g.h"

namespace CalculatorApp
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SettingsPage sealed
	{
	public:
		SettingsPage();

    private:
        void ColorSettingsButtonClicked(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SettingsFeedbackButtonClick(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SetVersionString();
        void SetCopyrightString();
        void LightChecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void DarkChecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SystemChecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    };
}
