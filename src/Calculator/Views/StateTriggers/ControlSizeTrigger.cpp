// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ControlSizeTrigger.h"

using namespace CalculatorApp::Views::StateTriggers;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

DEPENDENCY_PROPERTY_INITIALIZATION(ControlSizeTrigger, Source);
DEPENDENCY_PROPERTY_INITIALIZATION(ControlSizeTrigger, MinHeight);
DEPENDENCY_PROPERTY_INITIALIZATION(ControlSizeTrigger, MinWidth);

ControlSizeTrigger::ControlSizeTrigger()
{
    SetActive(false);
}

ControlSizeTrigger::~ControlSizeTrigger()
{
    UnregisterSizeChanged(Source);
}

void ControlSizeTrigger::OnSourcePropertyChanged(FrameworkElement ^ oldValue, FrameworkElement ^ newValue)
{
    UnregisterSizeChanged(oldValue);
    RegisterSizeChanged(newValue);
}

void ControlSizeTrigger::RegisterSizeChanged(FrameworkElement ^ element)
{
    if (element == nullptr)
    {
        return;
    }

    if (element != Source)
    {
        UnregisterSizeChanged(Source);
    }

    m_sizeChangedToken = element->SizeChanged += ref new SizeChangedEventHandler(this, &ControlSizeTrigger::OnSizeChanged);
    UpdateIsActive(element->RenderSize);
}

void ControlSizeTrigger::UnregisterSizeChanged(FrameworkElement ^ element)
{
    if (element != nullptr && m_sizeChangedToken.Value != 0)
    {
        element->SizeChanged -= m_sizeChangedToken;
        m_sizeChangedToken.Value = 0;
    }
}

void ControlSizeTrigger::OnSizeChanged(Object ^ sender, SizeChangedEventArgs ^ e)
{
    UpdateIsActive(e->NewSize);
}

void ControlSizeTrigger::UpdateIsActive(Size sourceSize)
{
    if (MinHeight >= 0)
    {
        SetActive(sourceSize.Height >= MinHeight && (MinWidth < 0 || sourceSize.Width >= MinWidth));
    }
    else
    {
        SetActive(MinWidth >= 0 && sourceSize.Width >= MinWidth);
    }
}
