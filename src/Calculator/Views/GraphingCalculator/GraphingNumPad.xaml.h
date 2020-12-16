// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views\GraphingCalculator\GraphingNumPad.g.h"
#include "CalcViewModel/GraphingCalculator/GraphingCalculatorViewModel.h"
#include "Views/GraphingCalculator/EquationInputArea.xaml.h"
#include "CalcViewModel/Common/CalculatorButtonUser.h"
#include "CalcViewModel/Common/TraceLogger.h"

namespace CalculatorApp
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class GraphingNumPad sealed 
	{
	public:
		GraphingNumPad();

    private:
        void ShiftButton_Check(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ShiftButton_Uncheck(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void TrigFlyoutShift_Toggle(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void TrigFlyoutHyp_Toggle(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void FlyoutButton_Clicked(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ShiftButton_IsEnabledChanged(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ e);
        void SetOperatorRowVisibility();
        void SetTrigRowVisibility();
        void Button_Clicked(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ e);
        void SubmitButton_Clicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ClearButton_Clicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void BackSpaceButton_Clicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void GraphingNumPad_PointerPressed(Platform::Object ^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e);
        Controls::MathRichEditBox^ GetActiveRichEdit();
        void Flyout_Opening(Platform::Object ^ sender, Platform::Object ^ e);

    private:
        static const std::tuple<Platform::String ^, int, int> GetButtonOutput(NumbersAndOperatorsEnum id);
    };
}
