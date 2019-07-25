// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AspectRatioTrigger.h"

using namespace CalculatorApp::Views::StateTriggers;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

DEPENDENCY_PROPERTY_INITIALIZATION(AspectRatioTrigger, Source);
DEPENDENCY_PROPERTY_INITIALIZATION(AspectRatioTrigger, NumeratorAspect);
DEPENDENCY_PROPERTY_INITIALIZATION(AspectRatioTrigger, Threshold);
DEPENDENCY_PROPERTY_INITIALIZATION(AspectRatioTrigger, ActiveIfEqual);

AspectRatioTrigger::AspectRatioTrigger()
{
    SetActive(false);
}

AspectRatioTrigger::~AspectRatioTrigger()
{
    UnregisterSizeChanged(Source);
}

void AspectRatioTrigger::OnSourcePropertyChanged(FrameworkElement ^ oldValue, FrameworkElement ^ newValue)
{
    UnregisterSizeChanged(oldValue);
    RegisterSizeChanged(newValue);
}

void AspectRatioTrigger::RegisterSizeChanged(FrameworkElement ^ element)
{
    if (element == nullptr)
    {
        return;
    }

    if (element != Source)
    {
        UnregisterSizeChanged(Source);
    }

    m_sizeChangedToken = element->SizeChanged += ref new SizeChangedEventHandler(this, &AspectRatioTrigger::OnSizeChanged);
}

void AspectRatioTrigger::UnregisterSizeChanged(FrameworkElement ^ element)
{
    if ((element != nullptr) && (m_sizeChangedToken.Value != 0))
    {
        element->SizeChanged -= m_sizeChangedToken;
        m_sizeChangedToken.Value = 0;
    }
}

void AspectRatioTrigger::OnSizeChanged(Object ^ sender, SizeChangedEventArgs ^ e)
{
    UpdateIsActive(e->NewSize);
}

void AspectRatioTrigger::UpdateIsActive(Size sourceSize)
{
    double numerator, denominator;
    if (NumeratorAspect == Aspect::Height)
    {
        numerator = sourceSize.Height;
        denominator = sourceSize.Width;
    }
    else
    {
        numerator = sourceSize.Width;
        denominator = sourceSize.Height;
    }

    bool isActive = false;
    if (denominator > 0)
    {
        double ratio = numerator / denominator;
        double threshold = abs(Threshold);

        isActive = ((ratio > threshold) || (ActiveIfEqual && (ratio == threshold)));
    }

    SetActive(isActive);
}
