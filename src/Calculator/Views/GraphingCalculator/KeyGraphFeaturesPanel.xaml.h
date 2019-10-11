#pragma once

#include "Views\GraphingCalculator\KeyGraphFeaturesPanel.g.h"
#include "CalcViewModel\GraphingCalculator\EquationViewModel.h"
#include "Controls/MathRichEditBox.h"
#include "Controls/EquationTextBox.h"

namespace CalculatorApp
{
    enum KeyGraphFeaturesFlag
    {
        Domain = 1,
        Range = 2,
        Parity = 4,
        Periodicity = 8,
        Zeros = 16,
        YIntercept = 32,
        Minima = 64,
        Maxima = 128,
        InflectionPoints = 256,
        VerticalAsymptotes = 512,
        HorizontalAsymptotes = 1024,
        ObliqueAsymptotes = 2048,
        MonotoneIntervals = 4096
    };

 public
    ref class MonotonicityItem sealed
    {
    public:
        property Platform::String ^ Expression;
        property Platform::String ^ Direction;
    };

     public
    ref class ExpressionItem sealed
    {
    public:
        property Platform::String ^ Expression;
    };

public
    ref class KeyGraphFeaturesPanel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        KeyGraphFeaturesPanel();

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
        OBSERVABLE_PROPERTY_R(Platform::String ^, Domain);
        OBSERVABLE_PROPERTY_R(Platform::String ^, Range);
        OBSERVABLE_PROPERTY_R(Platform::String ^, XIntercept);
        OBSERVABLE_PROPERTY_R(Platform::String ^, YIntercept);
        OBSERVABLE_PROPERTY_R(Platform::String ^, Parity);
        OBSERVABLE_PROPERTY_R(Platform::String ^, Periodicity);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<ExpressionItem ^> ^, Minima);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<ExpressionItem ^> ^, Maxima);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<ExpressionItem ^> ^, InflectionPoints);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<ExpressionItem ^> ^, VerticalAsymptotes);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<ExpressionItem ^> ^, HorizontalAsymptotes);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<ExpressionItem ^> ^, ObliqueAsymptotes);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<CalculatorApp::MonotonicityItem ^> ^, Monotonicity);
        OBSERVABLE_PROPERTY_R(Platform::String ^, TooComplexFeatures);
        OBSERVABLE_PROPERTY_R(Platform::String ^, MonotonicityError);
        OBSERVABLE_PROPERTY_R(bool, AnalysisNotSupported);

        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(CalculatorApp::ViewModel::EquationViewModel ^, ViewModel);

        event Windows::UI::Xaml::RoutedEventHandler ^ KeyGraphFeaturesClosed;

    private:
        void OnPropertyChanged(Platform::String ^ propertyName);

        void SetParityStringProperty();
        void SetPeriodicityStringProperty();
        Windows::Foundation::Collections::IObservableVector<ExpressionItem^> ^ SetVectorValue(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^ vector, Platform::String ^ errorString);
        void SetMonotoncityStringProperty();
        void SetTooComplexFeaturesErrorProperty();
        void EquationButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void SetEquationTextBoxProperties();
        void EquationInputTextBox_Loaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ResetKGFControlVisibility();

        Windows::ApplicationModel::Resources::ResourceLoader ^ m_resourceLoader;
    };
}
