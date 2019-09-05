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

        #pragma region Key Graph Features

        #pragma region Platform::String ^ Zeros DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ ZerosProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_zerosProperty;
            }
        }
        property Platform::String^ Zeros
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_zerosProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_zerosProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String ^ YIntercept DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ YInterceptProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_yInterceptProperty;
            }
        }
        property Platform::String^ YIntercept
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_yInterceptProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_yInterceptProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String ^ Parity DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ ParityProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_parityProperty;
            }
        }
        property Platform::String^ Parity
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_parityProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_parityProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String ^ Periodicity DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ PeriodicityProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_periodicityProperty;
            }
        }
        property Platform::String^ Periodicity
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_periodicityProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_periodicityProperty, value);
            }
        }
        #pragma endregion

        #pragma region Windows::Foundation::Collections::IObservableVector <Platform::String^>^ Minima DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ MinimaProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_minimaProperty;
            }
        }
        property Windows::Foundation::Collections::IObservableVector<Platform::String^>^ Minima
        {
            Windows::Foundation::Collections::IObservableVector<Platform::String^>^ get()
            {
                return static_cast<Windows::Foundation::Collections::IObservableVector<Platform::String^>^>(GetValue(s_minimaProperty));
            }
            void set(Windows::Foundation::Collections::IObservableVector<Platform::String^>^ value)
            {
                SetValue(s_minimaProperty, value);
            }
        }
        #pragma endregion

        #pragma region Windows::Foundation::Collections::IObservableVector < Platform::String ^ > ^ Maxima DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ MaximaProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_maximaProperty;
            }
        }
        property Windows::Foundation::Collections::IObservableVector<Platform::String^>^ Maxima
        {
            Windows::Foundation::Collections::IObservableVector<Platform::String^>^ get()
            {
                return static_cast<Windows::Foundation::Collections::IObservableVector<Platform::String^>^>(GetValue(s_maximaProperty));
            }
            void set(Windows::Foundation::Collections::IObservableVector<Platform::String^>^ value)
            {
                SetValue(s_maximaProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String ^ Domain DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ DomainProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_domainProperty;
            }
        }
        property Platform::String^ Domain
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_domainProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_domainProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String ^ Range DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ RangeProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_rangeProperty;
            }
        }
        property Platform::String^ Range
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_rangeProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_rangeProperty, value);
            }
        }
        #pragma endregion

        #pragma region Windows::Foundation::Collections::IObservableVector < Platform::String ^ > ^ InflectionPoints DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ InflectionPointsProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_inflectionPointsProperty;
            }
        }
        property Windows::Foundation::Collections::IObservableVector<Platform::String^>^ InflectionPoints
        {
            Windows::Foundation::Collections::IObservableVector<Platform::String^>^ get()
            {
                return static_cast<Windows::Foundation::Collections::IObservableVector<Platform::String^>^>(GetValue(s_inflectionPointsProperty));
            }
            void set(Windows::Foundation::Collections::IObservableVector<Platform::String^>^ value)
            {
                SetValue(s_inflectionPointsProperty, value);
            }
        }
        #pragma endregion

        #pragma region Windows::Foundation::Collections::IObservableVector < Platform::String ^ > ^ Monotonicity DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ MonotonicityProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_monotonicityProperty;
            }
        }
        property Windows::Foundation::Collections::IObservableVector<Platform::String^>^ Monotonicity
        {
            Windows::Foundation::Collections::IObservableVector<Platform::String^>^ get()
            {
                return static_cast<Windows::Foundation::Collections::IObservableVector<Platform::String^>^>(GetValue(s_monotonicityProperty));
            }
            void set(Windows::Foundation::Collections::IObservableVector<Platform::String^>^ value)
            {
                SetValue(s_monotonicityProperty, value);
            }
        }
        #pragma endregion

        #pragma endregion

    internal:
        event PropertyChangedEventHandler^ PropertyChanged;
        event PropertyChangedEventHandler^ KeyGraphFeaturesPropertyChanged;

        std::wstring GetRequest();

    private:
        static void OnCustomDependencyPropertyChanged(Windows::UI::Xaml::DependencyObject ^ obj, Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args);
        static void OnKeyGraphFeaturesPropertyChanged(Windows::UI::Xaml::DependencyObject ^ obj, Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ args);

        std::wstring GetRequestHeader();
        std::wstring GetExpression();
        std::wstring GetLineColor();

    private:
        static Windows::UI::Xaml::DependencyProperty ^ s_expressionProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_lineColorProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_zerosProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_yInterceptProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_parityProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_periodicityProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_minimaProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_maximaProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_domainProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_rangeProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_inflectionPointsProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_monotonicityProperty;
    };
}
