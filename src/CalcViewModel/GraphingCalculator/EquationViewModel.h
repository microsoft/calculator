#pragma once

#include "../Common/Utils.h"

namespace CalculatorApp::ViewModel
{

    public
    ref class EquationViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        EquationViewModel();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Expression);
        OBSERVABLE_PROPERTY_RW(Windows::UI::Xaml::Media::SolidColorBrush ^, LineColor);

        // Key Graph Features
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Domain);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Range);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, XIntercept);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, YIntercept);
        OBSERVABLE_PROPERTY_RW(int, Parity);
        OBSERVABLE_PROPERTY_R(Platform::String ^, ParityString);
        OBSERVABLE_PROPERTY_R(Platform::String ^, PeriodicityString);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, Minima);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, Maxima);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, InflectionPoints);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, VerticalAsymptotes);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, HorizontalAsymptotes);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, ObliqueAsymptotes);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, MonotonicityString);
        OBSERVABLE_PROPERTY_RW(int, TooComplexFeatures);
        OBSERVABLE_PROPERTY_RW(int, AnalysisError);
        OBSERVABLE_PROPERTY_R(Platform::String ^, TooComplexFeaturesString);
        OBSERVABLE_PROPERTY_R(Platform::String ^, AnalysisErrorString);
        OBSERVABLE_PROPERTY_R(bool, AnalysisErrorVisible);

        property Windows::Foundation::Collections::IObservableMap<Platform::String^, Platform::String^>^ Periodicity
        {
            Windows::Foundation::Collections::IObservableMap<Platform::String^, Platform::String ^> ^ get()
            {
                return m_Periodicity;
            }
            void set(Windows::Foundation::Collections::IObservableMap<Platform::String^, Platform::String ^> ^ value)
            {
                if (m_Periodicity != value)
                {
                    m_Periodicity = value;
                }
            }
        }

        property Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::String^> ^ Monotonicity
        {
            Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::String^> ^ get()
            {
                return m_Monotonicity;
            }
            void set(Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::String^> ^ value)
            {
                if (m_Monotonicity != value)
                {
                    m_Monotonicity = value;
                }
            }
        }

    private:
        void OnPropertyChanged(Platform::String ^ propertyName);
        void SetParityStringProperty();
        void SetPeriodicityStringProperty();
        Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^ SetVectorValue(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^ vector, Platform::String ^ errorString);
        void SetMonotoncityStringProperty();
        void SetTooComplexFeaturesErrorProperty();




        Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::String ^> ^ m_Periodicity;
        Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::String ^> ^ m_Monotonicity;
        Windows::ApplicationModel::Resources::ResourceLoader ^ m_resourceLoader;
    };

}
