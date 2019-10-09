#pragma once

#include "Views\GraphingCalculator\KeyGraphFeaturesPanel.g.h"
#include "CalcViewModel\GraphingCalculator\EquationViewModel.h"

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

    public ref class KeyGraphFeaturesPanel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
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
        OBSERVABLE_PROPERTY_R(Platform::String ^, Minima);
        OBSERVABLE_PROPERTY_R(Platform::String ^, Maxima);
        OBSERVABLE_PROPERTY_R(Platform::String ^, InflectionPoints);
        OBSERVABLE_PROPERTY_R(Platform::String ^, Monotonicity);
        OBSERVABLE_PROPERTY_R(Platform::String ^, VerticalAsymptotes);
        OBSERVABLE_PROPERTY_R(Platform::String ^, HorizontalAsymptotes);
        OBSERVABLE_PROPERTY_R(Platform::String ^, ObliqueAsymptotes);
        OBSERVABLE_PROPERTY_R(Platform::String ^, TooComplexFeaturesError);
        OBSERVABLE_PROPERTY_R(bool, AnalysisNotSupported);

        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(CalculatorApp::ViewModel::EquationViewModel ^, ViewModel);

        event Windows::UI::Xaml::RoutedEventHandler ^ KeyGraphFeaturesClosed;


    private:
        void OnPropertyChanged(Platform::String ^ propertyName);

        void SetParityStringProperty();
        void SetPeriodicityStringProperty();
        Platform::String ^ ConvertVectorToString(Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^ inVector);
        void SetMonotoncityStringProperty();
        void SetTooComplexFeaturesErrorProperty();
        void EquationButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        Windows::ApplicationModel::Resources::ResourceLoader ^ m_resourceLoader;
    };
}
