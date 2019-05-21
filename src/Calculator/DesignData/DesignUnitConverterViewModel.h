// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

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
        ref class CategoryViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            CategoryViewModel(Platform::String ^ name)
                : m_Name(name)
                , m_NegateVisibility(Windows::UI::Xaml::Visibility::Collapsed)
            {
            }

            CategoryViewModel(Platform::String ^ name, Windows::UI::Xaml::Visibility negateVisibility)
                : m_Name(name)
                , m_NegateVisibility(negateVisibility)
            {
            }

            OBSERVABLE_OBJECT();

            OBSERVABLE_PROPERTY_RW(Platform::String ^, Name);
            OBSERVABLE_PROPERTY_RW(Windows::UI::Xaml::Visibility, NegateVisibility);
        };

    public
        ref class UnitViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            UnitViewModel(Platform::String ^ unit, Platform::String ^ abbr)
                : m_Name(unit)
                , m_Abbreviation(abbr)
            {
            }

            OBSERVABLE_OBJECT();

            OBSERVABLE_PROPERTY_RW(Platform::String ^, Name);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, Abbreviation);
        };

    public
        ref class UnitConverterSupplementaryResultViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            UnitConverterSupplementaryResultViewModel(Platform::String ^ value, Platform::String ^ unit, Platform::String ^ abbr)
                : m_Value(value)
            {
                m_Unit = ref new UnitViewModel(unit, abbr);
            }

            OBSERVABLE_OBJECT();

            OBSERVABLE_PROPERTY_RW(Platform::String ^, Value);
            OBSERVABLE_PROPERTY_RW(UnitViewModel ^, Unit);
        };

    public
        ref class UnitConverterViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            UnitConverterViewModel()
                : m_Value1("Åy24")
                , m_Value2("Åy183")
                , m_Value1Active(true)
                , m_Value2Active(false)
            {
                m_SupplementaryResults = ref new Platform::Collections::Vector<UnitConverterSupplementaryResultViewModel ^>();
                m_SupplementaryResults->Append(ref new UnitConverterSupplementaryResultViewModel("128", "Kilograms", "Kgs"));
                m_SupplementaryResults->Append(ref new UnitConverterSupplementaryResultViewModel("42.55", "Liters", "ÅyL"));
                m_SupplementaryResults->Append(ref new UnitConverterSupplementaryResultViewModel("1.5e3", "Gallons", "G"));
                m_SupplementaryResults->Append(ref new UnitConverterSupplementaryResultViewModel("1929", "Gigabyte", "GB"));

                m_Categories = ref new Platform::Collections::Vector<CategoryViewModel ^>();
                m_Categories->Append(ref new CategoryViewModel("Volume"));
                m_Categories->Append(ref new CategoryViewModel("Temperature", Windows::UI::Xaml::Visibility::Visible));
                m_CurrentCategory = ref new CategoryViewModel("ÅyTime");
                m_Categories->Append(m_CurrentCategory);
                m_Categories->Append(ref new CategoryViewModel("Speed"));

                m_Units = ref new Platform::Collections::Vector<UnitViewModel ^>();
                m_Unit1 = ref new UnitViewModel("ÅySeconds", "S");
                m_Unit2 = ref new UnitViewModel("ÅyMinutes", "M");
                m_Units->Append(ref new UnitViewModel("Miliseconds", "MS"));
                m_Units->Append(m_Unit1);
                m_Units->Append(m_Unit2);
                m_Units->Append(ref new UnitViewModel("Hours", "HRs"));
            }

            OBSERVABLE_OBJECT();

            OBSERVABLE_PROPERTY_RW(Platform::String ^, Value1);
            OBSERVABLE_PROPERTY_RW(Platform::String ^, Value2);
            OBSERVABLE_PROPERTY_R(Windows::UI::Xaml::Interop::IBindableObservableVector ^, Categories);
            OBSERVABLE_PROPERTY_RW(CategoryViewModel ^, CurrentCategory);
            OBSERVABLE_PROPERTY_R(Windows::UI::Xaml::Interop::IBindableObservableVector ^, Units);
            OBSERVABLE_PROPERTY_RW(UnitViewModel ^, Unit1);
            OBSERVABLE_PROPERTY_RW(UnitViewModel ^, Unit2);
            OBSERVABLE_PROPERTY_RW(bool, Value1Active);
            OBSERVABLE_PROPERTY_RW(bool, Value2Active);
            OBSERVABLE_PROPERTY_R(Windows::UI::Xaml::Interop::IBindableObservableVector ^, SupplementaryResults);
        };

#endif
    }
}
