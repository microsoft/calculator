#include "pch.h"
#include "KeyGraphFeaturesPanel.xaml.h"
#include "Controls/MathRichEditBox.h"
#include "CalcViewModel/GraphingCalculatorEnums.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

KeyGraphFeaturesPanel::KeyGraphFeaturesPanel()

{
    InitializeComponent();
}

void KeyGraphFeaturesPanel::OnPropertyChanged(String ^ propertyName)
{
    if (propertyName == L"ViewModel")
    {
        if (ViewModel == nullptr)
        {
            return;
        }

        SetEquationTextBoxProperties();
        ResetKGFControlVisibility();
    }
}

void KeyGraphFeaturesPanel::EquationButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    KeyGraphFeaturesClosed(this, ref new RoutedEventArgs());
}

void KeyGraphFeaturesPanel::EquationInputTextBox_Loaded(Object ^ sender, RoutedEventArgs ^ e)
{
    if (ViewModel != nullptr)
    {
        SetEquationTextBoxProperties();
    }
}

void KeyGraphFeaturesPanel::SetEquationTextBoxProperties()
{
    EquationInputTextBox->SetEquationText(ViewModel->Expression);
    EquationInputTextBox->EquationColor = ViewModel->LineColor;
}

void KeyGraphFeaturesPanel::ResetKGFControlVisibility()
{
    Windows::UI::Xaml::Visibility visible = Windows::UI::Xaml::Visibility::Visible;

    DomainEditbox->Visibility = visible;
    DomainTextBlock->Visibility = visible;

    RangeEditBox->Visibility = visible;
    RangeTextBlock->Visibility = visible;

    XInterceptEditBox->Visibility = visible;
    XInterceptTextBlock->Visibility = visible;

    YInterceptEditBox->Visibility = visible;
    YInterceptTextBlock->Visibility = visible;

    ParityEditBox->Visibility = visible;
    ParityTextBlock->Visibility = visible;

    PeriodicityEditBox->Visibility = visible;
    PeriodicityTextBlock->Visibility = visible;

    MinimaList->Visibility = visible;
    MinimaTextBlock->Visibility = visible;

    MaximaList->Visibility = visible;
    MaximaTextBlock->Visibility = visible;

    InflectionPointsList->Visibility = visible;
    InflectionPointsTextBlock->Visibility = visible;

    VerticalAsymptotesList->Visibility = visible;
    VerticalAsymptotesTextBlock->Visibility = visible;

    HorizontalAsymptotesList->Visibility = visible;
    HorizontalAsymptotesTextBlock->Visibility = visible;

    ObliqueAsymptotesList->Visibility = visible;
    ObliqueAsymptotesTextBlock->Visibility = visible;

    MonotonicityList->Visibility = visible;
    MonotonicityTextBlock->Visibility = visible;

    TooComplexFeaturesTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    TooComplexFeaturesErrorTextBlock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

void KeyGraphFeaturesPanel::SetTooComplexFeaturesVisibility()
{
    if (ViewModel->TooComplexFeatures <= 0)
    {
        return;
    }

    TooComplexFeaturesTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;
    TooComplexFeaturesErrorTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;

    Windows::UI::Xaml::Visibility collapsed = Windows::UI::Xaml::Visibility::Collapsed;
    if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Domain) == KeyGraphFeaturesFlag::Domain)
    {
        DomainEditbox->Visibility = collapsed;
        DomainTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Range) == KeyGraphFeaturesFlag::Range)
    {
        RangeEditBox->Visibility = collapsed;
        RangeTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Zeros) == KeyGraphFeaturesFlag::Zeros)
    {
        XInterceptEditBox->Visibility = collapsed;
        XInterceptTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::YIntercept) == KeyGraphFeaturesFlag::YIntercept)
    {
        YInterceptEditBox->Visibility = collapsed;
        YInterceptTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Parity) == KeyGraphFeaturesFlag::Parity)
    {
        ParityEditBox->Visibility = collapsed;
        ParityTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Periodicity) == KeyGraphFeaturesFlag::Periodicity)
    {
        PeriodicityEditBox->Visibility = collapsed;
        PeriodicityTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Minima) == KeyGraphFeaturesFlag::Minima)
    {
        MinimaList->Visibility = collapsed;
        MinimaTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Maxima) == KeyGraphFeaturesFlag::Maxima)
    {
        MaximaList->Visibility = collapsed;
        MaximaTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::InflectionPoints) == KeyGraphFeaturesFlag::InflectionPoints)
    {
        InflectionPointsList->Visibility = collapsed;
        InflectionPointsTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::VerticalAsymptotes) == KeyGraphFeaturesFlag::VerticalAsymptotes)
    {
        VerticalAsymptotesList->Visibility = collapsed;
        VerticalAsymptotesTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::HorizontalAsymptotes) == KeyGraphFeaturesFlag::HorizontalAsymptotes)
    {
        HorizontalAsymptotesList->Visibility = collapsed;
        HorizontalAsymptotesTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::ObliqueAsymptotes) == KeyGraphFeaturesFlag::ObliqueAsymptotes)
    {
        ObliqueAsymptotesList->Visibility = collapsed;
        ObliqueAsymptotesTextBlock->Visibility = collapsed;
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::MonotoneIntervals) == KeyGraphFeaturesFlag::MonotoneIntervals)
    {
        MonotonicityList->Visibility = collapsed;
        MonotonicityTextBlock->Visibility = collapsed;
    }
}
