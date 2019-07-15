// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// SupplementaryResults.xaml.cpp
// Implementation of the SupplementaryResults class
//

#include "pch.h"
#include "SupplementaryResults.xaml.h"

using namespace CalculatorApp;

using namespace Platform;
using namespace Platform::Collections;
using namespace ViewModel;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

DEPENDENCY_PROPERTY_INITIALIZATION(SupplementaryResults, Results);

Object ^ DelighterUnitToStyleConverter::Convert(Object ^ value, TypeName /*targetType*/, Object ^ /*parameter*/, String ^ /*language*/)
{
    Unit ^ unit = safe_cast<Unit ^>(value);

    assert(unit->GetModelUnit().isWhimsical);
    if (!unit->GetModelUnit().isWhimsical)
    {
        return nullptr;
    }

    std::wstring key = L"Unit_";
    key.append(std::to_wstring(unit->GetModelUnit().id));
    return safe_cast<IStyle ^>(m_delighters->Lookup(ref new String(key.c_str())));
}

Object ^ DelighterUnitToStyleConverter::ConvertBack(Object ^ /*value*/, TypeName /*targetType*/, Object ^ /*parameter*/, String ^ /*language*/)
{
    // We never use convert back, only one way binding supported
    assert(false);
    return nullptr;
}

Windows::UI::Xaml::DataTemplate ^ SupplementaryResultDataTemplateSelector::SelectTemplateCore(Object ^ item, DependencyObject ^ /*container*/)
{
    SupplementaryResult ^ result = safe_cast<SupplementaryResult ^>(item);
    if (result->IsWhimsical())
    {
        return DelighterTemplate;
    }
    else
    {
        return RegularTemplate;
    }
}

SupplementaryResults::SupplementaryResults()
{
    InitializeComponent();
}

bool SupplementaryResultNoOverflowStackPanel::ShouldPrioritizeLastItem()
{
    if (Children->Size == 0)
    {
        return false;
    }
    auto lastChild = dynamic_cast<FrameworkElement ^>(Children->GetAt(Children->Size - 1));
    if (lastChild == nullptr)
    {
        return false;
    }
    auto suppResult = dynamic_cast<SupplementaryResult ^>(lastChild->DataContext);
    return suppResult == nullptr ? false : suppResult->IsWhimsical();
}
