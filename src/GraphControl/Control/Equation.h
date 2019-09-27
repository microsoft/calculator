#pragma once

namespace GraphControl
{
    namespace EquationProperties
    {
        extern Platform::String ^ Expression;
        extern Platform::String ^ LineColor;
    }

	// function parity for function analysis
    enum class FunctionParityType
    {
        // parity not calculated or not possible to calculate
        FunctionParityType_Unknown = 0,

        // parity is odd
        FunctionParityType_Odd = 1,

        // parity is even
        FunctionParityType_Even = 2,

        // function is not odd nor even
        FunctionParityType_None = 3
    };

    // monotonicity direction for function analysis
    enum class FunctionMonotonicityType
    {
        // unknown or not calculated
        FunctionMonotonicityType_Unknown = 0,

        // ascending monotonicity on interval
        FunctionMonotonicityType_Ascending = 1,

        // descending monotonicity on interval
        FunctionMonotonicityType_Descending = 2,

        // constant monotonicity on interval
        FunctionMonotonicityType_Constant = 3
    };

    // asymptote description for function analysis
    enum class AsymptoteType
    {
        // unknown or not calculated
        AsymptoteType_Unknown = 0,

        // when x goes to positive infinity
        AsymptoteType_PositiveInfinity = 1,

        // when x goes to negative infinity
        AsymptoteType_NegativeInfinity = 2,

        // when x goes to positive or negative infinity
        AsymptoteType_AnyInfinity = 3
    };
    // function periodicity for function analysis
    enum class FunctionPeriodicityType
    {
        // periodicity not calculated or not possible to calculate
        FunctionPeriodicityType_Unknown = 0,

        // parity is odd
        FunctionPeriodicityType_Periodic = 1,

        // parity is even
        FunctionPeriodicityType_NotPeriodic = 2
    };

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

        #pragma region Platform::String ^ XIntercept DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ XInterceptProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_xInterceptProperty;
            }
        }
        property Platform::String^ XIntercept
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_xInterceptProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_xInterceptProperty, value);
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

        #pragma region int Parity DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ ParityProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_parityProperty;
            }
        }
        property int Parity
        {
           int get()
            {
                return static_cast<int>(GetValue(s_parityProperty));
            }
            void set(int value)
            {
                SetValue(s_parityProperty, value);
            }
        }
        #pragma endregion

        #pragma region int Periodicity DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ PeriodicityProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_periodicityProperty;
            }
        }
        property int Periodicity
        {
            int get()
            {
                return static_cast<int>(GetValue(s_periodicityProperty));
            }
            void set(int value)
            {
                SetValue(s_periodicityProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String^ Minima DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ MinimaProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_minimaProperty;
            }
        }
        property Platform::String^ Minima
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_minimaProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_minimaProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String ^ Maxima DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ MaximaProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_maximaProperty;
            }
        }
        property Platform::String^ Maxima
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_maximaProperty));
            }
            void set(Platform::String^ value)
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

        #pragma region Platform::String ^ InflectionPoints DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ InflectionPointsProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_inflectionPointsProperty;
            }
        }
        property Platform::String^ InflectionPoints
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_inflectionPointsProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_inflectionPointsProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String ^ Monotonicity DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ MonotonicityProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_monotonicityProperty;
            }
        }
        property Platform::String^ Monotonicity
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_monotonicityProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_monotonicityProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String ^ VerticalAsymptotes DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ VerticalAsymptotesProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_verticalAsymptotesProperty;
            }
        }
        property Platform::String^ VerticalAsymptotes
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_verticalAsymptotesProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_verticalAsymptotesProperty, value);
            }
        }
        #pragma endregion

       #pragma region Platform::String ^ HorizontalAsymptotes DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ HorizontalAsymptotesProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_horizontalAsymptotesProperty;
            }
        }
        property Platform::String^ HorizontalAsymptotes
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_horizontalAsymptotesProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_horizontalAsymptotesProperty, value);
            }
        }
        #pragma endregion

        #pragma region Platform::String ^ ObliqueAsymptotes DependencyProperty
        static property Windows::UI::Xaml::DependencyProperty^ ObliqueAsymptotesProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get()
            {
                return s_obliqueAsymptotesProperty;
            }
        }
        property Platform::String^ ObliqueAsymptotes
        {
            Platform::String^ get()
            {
                return static_cast<Platform::String^>(GetValue(s_obliqueAsymptotesProperty));
            }
            void set(Platform::String^ value)
            {
                SetValue(s_obliqueAsymptotesProperty, value);
            }
        }
        #pragma endregion

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
        static Windows::UI::Xaml::DependencyProperty ^ s_xInterceptProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_yInterceptProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_parityProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_periodicityProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_minimaProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_maximaProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_domainProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_rangeProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_inflectionPointsProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_monotonicityProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_verticalAsymptotesProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_horizontalAsymptotesProperty;
        static Windows::UI::Xaml::DependencyProperty ^ s_obliqueAsymptotesProperty;
    };
}
