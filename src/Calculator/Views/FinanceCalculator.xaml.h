// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// FinanceCalculator.xaml.h
// Declaration of the FinanceCalculator class
//

#pragma once

#include "Converters/BooleanNegationConverter.h"
#include "Converters/VisibilityNegationConverter.h"
#include "CalcViewModel\Common\AppResourceProvider.h"
#include "Views\FinanceCalculator.g.h"

namespace CalculatorApp
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class FinanceCalculator sealed
	{
	public:
		FinanceCalculator();
        void SetDefaultFocus();

    private:
        void CalculateInterestButton_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnCopyMenuItemClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        double FutureValue();
        double InterestEarned();
    };
}
