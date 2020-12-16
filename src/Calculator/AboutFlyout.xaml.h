// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AboutFlyout.g.h"

namespace CalculatorApp
{
public
    ref class AboutFlyout sealed
    {
    public:
        AboutFlyout();

        void SetDefaultFocus();

    private:
        void FeedbackButton_Click(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SetVersionString();
        void InitializeContributeTextBlock();
    };
} /* namespace CalculatorApp */
