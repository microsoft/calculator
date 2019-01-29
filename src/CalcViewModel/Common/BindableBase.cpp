// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BindableBase.h"

using namespace CalculatorApp::Common;

using namespace Platform;
using namespace Windows::UI::Xaml::Data;

/// <summary>
/// Notifies listeners that a property value has changed.
/// </summary>
/// <param name="propertyName">Name of the property used to notify listeners.</param>
void BindableBase::OnPropertyChanged(String^ propertyName)
{
    PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
}

Windows::UI::Xaml::Data::ICustomProperty^ BindableBase::GetCustomProperty(Platform::String^ name) 
{
    return nullptr;
}

Windows::UI::Xaml::Data::ICustomProperty^ BindableBase::GetIndexedProperty(Platform::String^ name, Windows::UI::Xaml::Interop::TypeName type) 
{
    return nullptr;
}

Platform::String^ BindableBase::GetStringRepresentation() 
{
    return this->ToString(); 
}
