#pragma once

#include "../Common/Utils.h"

namespace CalculatorApp::ViewModel
{
public
    ref class GridDisplayItems sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        GridDisplayItems();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Expression);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Direction);
    };

public
    ref class KeyGraphFeaturesItem sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        KeyGraphFeaturesItem();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Title);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, DisplayItems);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<GridDisplayItems ^> ^, GridItems);
        OBSERVABLE_PROPERTY_RW(bool, IsText);
    };


public
    ref class EquationViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        EquationViewModel();

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Expression);
        OBSERVABLE_PROPERTY_RW(Windows::UI::Xaml::Media::SolidColorBrush ^, LineColor);

        // Key Graph Features
        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(bool, IsAnalysisUpdated);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Domain);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Range);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, XIntercept);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, YIntercept);
        OBSERVABLE_PROPERTY_RW(int, Parity);
        OBSERVABLE_PROPERTY_RW(int, PeriodicityDirection);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, PeriodicityExpression);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, Minima);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, Maxima);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, InflectionPoints);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, VerticalAsymptotes);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, HorizontalAsymptotes);
        OBSERVABLE_PROPERTY_RW(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^, ObliqueAsymptotes);
        OBSERVABLE_PROPERTY_RW(int, TooComplexFeatures);
        OBSERVABLE_PROPERTY_RW(int, AnalysisError);
        OBSERVABLE_PROPERTY_R(Platform::String ^, AnalysisErrorString);
        OBSERVABLE_PROPERTY_R(bool, AnalysisErrorVisible);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<CalculatorApp::ViewModel::KeyGraphFeaturesItem ^> ^, KeyGraphFeaturesItems)

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
        void SetKeyGraphFeaturesItems(Platform::String ^ title, Platform::String ^ expression, Platform::String ^ errorString);
        void SetKeyGraphFeaturesItems(Platform::String ^ title, Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^ expressionVector,
            Platform::String ^ errorString);
        void SetMonotoncityStringProperty();
        void SetTooComplexFeaturesErrorProperty();
        void OnIsAnalysisUpdatedChanged();

        Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::String ^> ^ m_Monotonicity;
        Windows::ApplicationModel::Resources::ResourceLoader ^ m_resourceLoader;
    };

}
