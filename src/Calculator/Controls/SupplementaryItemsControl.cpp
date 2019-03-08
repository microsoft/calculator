// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SupplementaryItemsControl.h"
#include "CalcViewModel/UnitConverterViewModel.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace std;

DependencyObject^ SupplementaryItemsControl::GetContainerForItemOverride()
{
    return ref new SupplementaryContentPresenter();
}

void SupplementaryItemsControl::PrepareContainerForItemOverride(DependencyObject^ element, Object^ item)
{
    ItemsControl::PrepareContainerForItemOverride(element, item);

    auto supplementaryResult = dynamic_cast<SupplementaryResult^>(item);
    if (supplementaryResult)
    {
        AutomationProperties::SetName(element, supplementaryResult->GetLocalizedAutomationName());
    }
}

 AutomationPeer^ SupplementaryContentPresenter::OnCreateAutomationPeer()
 {
     return ref new SupplementaryContentPresenterAP(this);
 }


