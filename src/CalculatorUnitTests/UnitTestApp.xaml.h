// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// App.xaml.h
// Declaration of the App class.
//

#pragma once

#include "UnitTestApp.g.h"
#include "CalcViewModel/DateCalculatorViewModel.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"
#include "CalcViewModel/UnitConverterViewModel.h"
#include "CalcViewModel/MemoryItemViewModel.h"

namespace CalculatorUnitTests
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    ref class App sealed
    {
    protected:
        virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

    internal:
        App();

    private:
        void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
        void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
    };
}
