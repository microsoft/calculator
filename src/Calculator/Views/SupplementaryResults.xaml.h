// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// SupplementaryResults.xaml.h
// Declaration of the SupplementaryResults class
//

#pragma once

#include "Views/SupplementaryResults.g.h"
#include "Controls/SupplementaryItemsControl.h"
#include "Controls/OperandTextBox.h"
#include "Controls/OperatorTextBox.h"
#include "CalcViewModel/UnitConverterViewModel.h"

namespace CalculatorApp
{
    public ref class DelighterUnitToStyleConverter sealed : public Windows::UI::Xaml::Data::IValueConverter
    {
    public:
        DelighterUnitToStyleConverter()
        {
            m_delighters = ref new Windows::UI::Xaml::ResourceDictionary();
            m_delighters->Source = ref new Windows::Foundation::Uri(L"ms-appx:///Views/DelighterUnitStyles.xaml");
        }

    internal:
        virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language) = Windows::UI::Xaml::Data::IValueConverter::Convert;
        virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language) = Windows::UI::Xaml::Data::IValueConverter::ConvertBack;

    private:
        Windows::UI::Xaml::ResourceDictionary^ m_delighters;
    };

    public ref class SupplementaryResultDataTemplateSelector sealed : public Windows::UI::Xaml::Controls::DataTemplateSelector
    {
    public:
        SupplementaryResultDataTemplateSelector() {}

        property Windows::UI::Xaml::DataTemplate^ RegularTemplate
        {
            Windows::UI::Xaml::DataTemplate^ get() { return m_regularTemplate; }
            void set(Windows::UI::Xaml::DataTemplate^ value) { m_regularTemplate = value; }
        }

        property Windows::UI::Xaml::DataTemplate^ DelighterTemplate
        {
            Windows::UI::Xaml::DataTemplate^ get() { return m_delighterTemplate; }
            void set(Windows::UI::Xaml::DataTemplate^ value) { m_delighterTemplate = value; }
        }

    protected:
        virtual Windows::UI::Xaml::DataTemplate^ SelectTemplateCore(Platform::Object^ item, Windows::UI::Xaml::DependencyObject^ container) override;

    private:
        Windows::UI::Xaml::DataTemplate^ m_regularTemplate;
        Windows::UI::Xaml::DataTemplate^ m_delighterTemplate;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SupplementaryResults sealed
    {
    public:
        SupplementaryResults();

    private:
        void RefreshData();
        void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnConverterPropertyChanged(Platform::Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e);
        void OnSupplementaryValuesLayoutUpdated(Platform::Object^ sender, Platform::Object^ e);
        void OnWindowSizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
        Windows::Foundation::Collections::IObservableVector<ViewModel::SupplementaryResult^>^ m_data;
    };
}
