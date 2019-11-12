// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace GraphControl
{
    namespace EquationProperties
    {
        extern Platform::String ^ Expression;
        extern Platform::String ^ LineColor;
    }

    ref class Equation;
    delegate void PropertyChangedEventHandler(Equation ^ sender, Platform::String ^ propertyName);

    [Windows::UI::Xaml::Data::Bindable] public ref class Equation sealed : public Windows::UI::Xaml::FrameworkElement
    {
    public:
        Equation()
        {
        }

        static void RegisterDependencyProperties();

#pragma region Platform::String ^ Expression DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ ExpressionProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_expressionProperty;
            }
        }
        property Platform::String^ Expression
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_expressionProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_expressionProperty, value);
            }
        }
#pragma endregion

#pragma region Windows::UI::Xaml::Media::SolidColorBrush ^ LineColor DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ LineColorProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_lineColorProperty;
            }
        }
        property Windows::UI::Xaml::Media::SolidColorBrush^ LineColor
        {
            Windows::UI::Xaml::Media::SolidColorBrush^ get()
            {
                return static_cast<Windows::UI::Xaml::Media::SolidColorBrush^>(GetValue(s_lineColorProperty));
            }
            void set(Windows::UI::Xaml::Media::SolidColorBrush^ value)
            {
                if (value == nullptr || LineColor == nullptr || (value->Color.A != LineColor->Color.A) || (value->Color.R != LineColor->Color.R) || (value->Color.G != LineColor->Color.G)
                    || (value->Color.B != LineColor->Color.B))
                {
                    SetValue(s_lineColorProperty, value);
                }
            }
        }
#pragma endregion

    internal:
        event PropertyChangedEventHandler^ PropertyChanged;

        std::wstring GetRequest();

    private:
        static void OnCustomDependencyPropertyChanged(Windows::UI::Xaml::DependencyObject ^ obj, Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args);

        std::wstring GetRequestHeader();
        std::wstring GetExpression();
        std::wstring GetLineColor();

    private:
        static Windows::UI::Xaml::DependencyProperty ^ s_expressionProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_lineColorProperty;
    };
}
