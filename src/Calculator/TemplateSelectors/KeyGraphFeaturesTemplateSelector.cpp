// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "KeyGraphFeaturesTemplateSelector.h"
#include "CalcViewModel/GraphingCalculator/EquationViewModel.h"

using namespace CalculatorApp::ViewModel;
using namespace CalculatorApp::TemplateSelectors;
using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

DataTemplate ^ KeyGraphFeaturesTemplateSelector::SelectTemplateCore(Object ^ item)
{
    auto kgfItem = static_cast<KeyGraphFeaturesItem ^>(item);

    if (!kgfItem->IsText)
    {
        if (kgfItem->DisplayItems->Size != 0)
        {
            return RichEditTemplate;
        }
        else if (kgfItem->GridItems->Size != 0)
        {
            return GridTemplate;
        }
    }

    return TextBlockTemplate;
}

DataTemplate ^ KeyGraphFeaturesTemplateSelector::SelectTemplateCore(Object ^ item, DependencyObject ^ container)
{
    return SelectTemplateCore(item);
}
