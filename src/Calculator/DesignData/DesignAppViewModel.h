// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DesignStandardCalculatorViewModel.h"
#include "DesignUnitConverterViewModel.h"

namespace Numbers
{
    namespace DesignData
    {
#ifdef _DEBUG
        // These class are to be consumed exclusively by Blend and the VS designer
        // with these classes Blend will be able to populate the controls
        // with the hardcoded strings so whoever is working on the UI can actually see how the app would look like
        // with semi-realistic data.
        // This data is to only be compiled in the debug build and it will not affect app functionality at all
        // so it does not need to be tested. It will have to be kept in sync with UnitConverterViewModel though
        // to ensure that the design experience is correct.
        // This class's code is run in the designer process so the less code it has the better.

    public
        ref class AppViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            AppViewModel()
                : m_IsStandardMode(true)
                , m_IsScientificMode(false)
                , m_IsConverterMode(false)
                , m_CalculatorViewModel(ref new StandardCalculatorViewModel())
                , m_ConverterViewModel(ref new UnitConverterViewModel())
            {
            }

            OBSERVABLE_OBJECT();

            OBSERVABLE_PROPERTY_RW(StandardCalculatorViewModel ^, CalculatorViewModel);
            OBSERVABLE_PROPERTY_RW(UnitConverterViewModel ^, ConverterViewModel);
            OBSERVABLE_PROPERTY_RW(bool, IsStandardMode);
            OBSERVABLE_PROPERTY_RW(bool, IsScientificMode);
            OBSERVABLE_PROPERTY_RW(bool, IsConverterMode);
        };
#endif
    }
}
