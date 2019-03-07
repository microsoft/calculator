// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp { namespace Common
{
    public ref class ValidSelectedItemConverter sealed: public Windows::UI::Xaml::Data::IValueConverter
    {
    public:
        ValidSelectedItemConverter()
        { }

    private:

        virtual Platform::Object^ Convert(
            Platform::Object^ value, 
            Windows::UI::Xaml::Interop::TypeName /*targetType*/, 
            Platform::Object^ /*parameter*/, 
            Platform::String^ /*language*/) = Windows::UI::Xaml::Data::IValueConverter::Convert
        {
            // Pass through as we don't want to change the value from the source
            return value;
        }

        virtual Platform::Object^ ConvertBack(
            Platform::Object^ value, 
            Windows::UI::Xaml::Interop::TypeName /*targetType*/, 
            Platform::Object^ /*parameter*/, 
            Platform::String^ /*language*/) = Windows::UI::Xaml::Data::IValueConverter::ConvertBack
        {
            if (value)
            {
                return value;
            }
            // Stop the binding if the object is nullptr
            return Windows::UI::Xaml::DependencyProperty::UnsetValue;
        }
    };

    public ref class ValidSelectedIndexConverter sealed: public Windows::UI::Xaml::Data::IValueConverter
    {
    public:
        ValidSelectedIndexConverter()
        { }

    private:

        virtual Platform::Object^ Convert(
            Platform::Object^ value, 
            Windows::UI::Xaml::Interop::TypeName /*targetType*/, 
            Platform::Object^ /*parameter*/, 
            Platform::String^ /*language*/) = Windows::UI::Xaml::Data::IValueConverter::Convert
        {
            // Pass through as we don't want to change the value from the source
            return value;
        }

        virtual Platform::Object^ ConvertBack(
            Platform::Object^ value, 
            Windows::UI::Xaml::Interop::TypeName /*targetType*/, 
            Platform::Object^ /*parameter*/, 
            Platform::String^ /*language*/) = Windows::UI::Xaml::Data::IValueConverter::ConvertBack
        {
            // The value to be valid has to be a boxed int32 value
            // extract that value and ensure it is valid, ie >= 0
            if (value)
            {
                auto box = dynamic_cast<Windows::Foundation::IPropertyValue^>(value);
                if (box && box->Type == Windows::Foundation::PropertyType::Int32)
                {
                    int index = box->GetInt32();
                    if (index >= 0)
                    {
                        return value;
                    }
                }
            }
            // The value is not valid therefore stop the binding right here
            return Windows::UI::Xaml::DependencyProperty::UnsetValue;
        }
    };
}}
