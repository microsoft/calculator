#pragma once

namespace GraphControl
{
    namespace EquationProperties
    {
        extern Platform::String^ Expression;
        extern Platform::String^ LineColor;
    }

    ref class Equation;
    delegate void PropertyChangedEventHandler(Equation^ sender, Platform::String^ propertyName);

    [Windows::UI::Xaml::Data::Bindable]
    public ref class Equation sealed : public Windows::UI::Xaml::FrameworkElement
    {
    public:
        Equation() {}

        static void RegisterDependencyProperties();

        #pragma region Platform::String^ Expression DependencyProperty
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

        #pragma region Windows::UI::Color LineColor DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ LineColorProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_lineColorProperty;
            }
        }
        property Windows::UI::Color LineColor
        {
            Windows::UI::Color get()
            {
                return static_cast<Windows::UI::Color>(GetValue(s_lineColorProperty));
            }
            void set(Windows::UI::Color value)
            {
                SetValue(s_lineColorProperty, value);
            }
        }
        #pragma endregion

    internal:
        event PropertyChangedEventHandler^ PropertyChanged;

        std::wstring GetRequest();

    private:
        static void OnCustomDependencyPropertyChanged(Windows::UI::Xaml::DependencyObject^ obj, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ args);

        std::wstring GetRequestHeader();
        std::wstring GetExpression();
        std::wstring GetLineColor();

    private:
        static Windows::UI::Xaml::DependencyProperty^ s_expressionProperty;
        static Windows::UI::Xaml::DependencyProperty^ s_lineColorProperty;
    };
}
