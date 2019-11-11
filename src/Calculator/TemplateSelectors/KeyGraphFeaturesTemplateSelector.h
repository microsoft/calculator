// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
namespace CalculatorApp
{
    namespace TemplateSelectors
    {
    public
        ref class KeyGraphFeaturesTemplateSelector  sealed : Windows::UI::Xaml::Controls::DataTemplateSelector
        {
        public:
            KeyGraphFeaturesTemplateSelector()
            {
            }

            property Windows::UI::Xaml::DataTemplate ^ RichEditTemplate;
            property Windows::UI::Xaml::DataTemplate ^ GridTemplate;
            property Windows::UI::Xaml::DataTemplate ^ TextBlockTemplate;

            Windows::UI::Xaml::DataTemplate ^ SelectTemplateCore(Platform::Object ^ item) override;
            Windows::UI::Xaml::DataTemplate ^ SelectTemplateCore(Platform::Object ^ item, Windows::UI::Xaml::DependencyObject ^ container) override;
        };
    }
}
