#include "pch.h"
#include "KeyGraphFeaturesPanel.xaml.h"
#include "Controls/MathRichEditBox.h"
#include "CalcViewModel/Common/LocalizationSettings.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation::Collections;

KeyGraphFeaturesPanel::KeyGraphFeaturesPanel()
    : m_Domain{ "" }
    , m_Range{ "" }
    , m_XIntercept{ "" }
    , m_YIntercept{ "" }
    , m_Parity{ "" }
    , m_Periodicity{ "" }
    , m_Minima{ ref new Vector<ExpressionItem ^>() }
    , m_Maxima{ ref new Vector<ExpressionItem ^>() }
    , m_InflectionPoints{ ref new Vector<ExpressionItem ^>() }
    , m_Monotonicity{ ref new Vector<MonotonicityItem ^>() }
    , m_VerticalAsymptotes{ ref new Vector<ExpressionItem ^>() }
    , m_HorizontalAsymptotes{ ref new Vector<ExpressionItem ^>() }
    , m_ObliqueAsymptotes{ ref new Vector<ExpressionItem ^>() }
    , m_resourceLoader{ Windows::ApplicationModel::Resources::ResourceLoader::GetForCurrentView() }
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

        if (ViewModel->AnalysisNotSupported)
        {
            AnalysisNotSupported = true;
        }
        else
        {
            AnalysisNotSupported = false;
            ResetKGFControlVisibility();

            Domain = ViewModel->Domain;
            Range = ViewModel->Range;
            XIntercept = ViewModel->XIntercept != L"" ? ViewModel->XIntercept : m_resourceLoader->GetString(L"KGFXInterceptNone");
            YIntercept = ViewModel->YIntercept != L"" ? ViewModel->YIntercept : m_resourceLoader->GetString(L"KGFYInterceptNone");
            SetParityStringProperty();
            SetPeriodicityStringProperty();
            Minima = SetVectorValue(ViewModel->Minima, m_resourceLoader->GetString(L"KGFMinimaNone"));
            Maxima = SetVectorValue(ViewModel->Maxima, m_resourceLoader->GetString(L"KGFMaximaNone"));
            InflectionPoints = SetVectorValue(ViewModel->InflectionPoints, m_resourceLoader->GetString(L"KGFInflectionPointsNone"));
            VerticalAsymptotes = SetVectorValue(ViewModel->VerticalAsymptotes, m_resourceLoader->GetString(L"KGFVerticalAsymptotesNone"));
            HorizontalAsymptotes = SetVectorValue(ViewModel->HorizontalAsymptotes, m_resourceLoader->GetString(L"KGFHorizontalAsymptotesNone"));
            ObliqueAsymptotes = SetVectorValue(ViewModel->ObliqueAsymptotes, m_resourceLoader->GetString(L"KGFObliqueAsymptotesNone"));
            SetMonotoncityStringProperty();
            SetTooComplexFeaturesErrorProperty();
        }
    }
}

void KeyGraphFeaturesPanel::SetParityStringProperty()
{
    switch (ViewModel->Parity)
    {
    case 0:
        Parity = m_resourceLoader->GetString(L"KGFParityUnknown");
        break;
    case 1:
        Parity = m_resourceLoader->GetString(L"KGFParityOdd");
        break;
    case 2:
        Parity = m_resourceLoader->GetString(L"KGFParityEven");
        break;
    case 3:
        Parity = m_resourceLoader->GetString(L"KGFParityNeither");
        break;
    default:
        Parity = m_resourceLoader->GetString(L"KGFParityError");
    }
}
void KeyGraphFeaturesPanel::SetPeriodicityStringProperty()
{
    if (ViewModel->Periodicity->Size != 0)
    {
        auto periodicityType = ViewModel->Periodicity->First()->Current->Key->Data();
        switch (*periodicityType)
        {
        case '0':
            Periodicity = m_resourceLoader->GetString(L"KGFPeriodicityUnknown");
            break;
        case '1':
            Periodicity = ViewModel->Periodicity->First()->Current->Value;
            break;
        case '2':
            Periodicity = m_resourceLoader->GetString(L"KGFPeriodicityNotPeriodic");
            break;
        default:
            Periodicity = m_resourceLoader->GetString(L"KGFPeriodicityError");
        }
    }
    else
    {
        Periodicity = m_resourceLoader->GetString(L"KGFPeriodicityError");
    }
}

void KeyGraphFeaturesPanel::SetMonotoncityStringProperty()
{
    Monotonicity->Clear();
    if (ViewModel->Monotonicity->Size != 0)
    {
        for (auto item : ViewModel->Monotonicity)
        {
            MonotonicityItem ^ monotonicityItem = ref new MonotonicityItem();

            auto monotonicityType = item->Value->Data();
            switch (*monotonicityType)
            {
            case '0':
                monotonicityItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityUnknown");
                break;
            case '1':
                monotonicityItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityIncreasing");
                break;
            case '2':
                monotonicityItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityDecreasing");
                break;
            case '3':
                monotonicityItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityConstant");
                break;
            default:
                monotonicityItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityError");
                break;
            }

            monotonicityItem->Expression = item->Key;
            Monotonicity->Append(monotonicityItem);
        }
    }
    else
    {
        MonotonicityError += m_resourceLoader->GetString(L"KGFMonotonicityError");
    }
}

IObservableVector<ExpressionItem ^> ^ KeyGraphFeaturesPanel::SetVectorValue(IObservableVector<String ^> ^ vector, String ^ errorString)
{
    IObservableVector<ExpressionItem ^> ^ out = ref new Vector<ExpressionItem ^>();
    if (vector->Size == 0)
    {
        ExpressionItem ^ exp = ref new ExpressionItem();
        exp->Expression = errorString;
        out->Append(exp);
    }
    else
    {
        for (auto i : vector)
        {
            ExpressionItem ^ exp = ref new ExpressionItem();
            exp->Expression = i;
            out->Append(exp);
        }
    }

    return out;
}

void KeyGraphFeaturesPanel::SetTooComplexFeaturesErrorProperty()
{
    if (ViewModel->TooComplexFeatures <= 0)
    {
        return;
    }

    TooComplexFeaturesTextBlock->Visibility = Windows::UI::Xaml::Visibility::Visible;

    Platform::String ^ separator = ref new String(LocalizationSettings::GetInstance().GetListSeparator().c_str());

    wstring error;
    Windows::UI::Xaml::Visibility collapsed = Windows::UI::Xaml::Visibility::Collapsed;
    if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Domain) == KeyGraphFeaturesFlag::Domain)
    {
        DomainEditbox->Visibility = collapsed;
        DomainTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"Domain/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Range) == KeyGraphFeaturesFlag::Range)
    {
        RangeEditBox->Visibility = collapsed;
        RangeTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"Range/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Zeros) == KeyGraphFeaturesFlag::Zeros)
    {
        XInterceptEditBox->Visibility = collapsed;
        XInterceptTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"XIntercept/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::YIntercept) == KeyGraphFeaturesFlag::YIntercept)
    {
        YInterceptEditBox->Visibility = collapsed;
        YInterceptTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"YIntercept/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Parity) == KeyGraphFeaturesFlag::Parity)
    {
        ParityEditBox->Visibility = collapsed;
        ParityTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"Parity/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Periodicity) == KeyGraphFeaturesFlag::Periodicity)
    {
        PeriodicityEditBox->Visibility = collapsed;
        PeriodicityTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"Periodicity/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Minima) == KeyGraphFeaturesFlag::Minima)
    {
        MinimaList->Visibility = collapsed;
        MinimaTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"Minima/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::Maxima) == KeyGraphFeaturesFlag::Maxima)
    {
        MaximaList->Visibility = collapsed;
        MaximaTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"Maxima/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::InflectionPoints) == KeyGraphFeaturesFlag::InflectionPoints)
    {
        InflectionPointsList->Visibility = collapsed;
        InflectionPointsTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"InflectionPoints/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::VerticalAsymptotes) == KeyGraphFeaturesFlag::VerticalAsymptotes)
    {
        VerticalAsymptotesList->Visibility = collapsed;
        VerticalAsymptotesTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"VerticalAsymptotes/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::HorizontalAsymptotes) == KeyGraphFeaturesFlag::HorizontalAsymptotes)
    {
        HorizontalAsymptotesList->Visibility = collapsed;
        HorizontalAsymptotesTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"HorizontalAsymptotes/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::ObliqueAsymptotes) == KeyGraphFeaturesFlag::ObliqueAsymptotes)
    {
        ObliqueAsymptotesList->Visibility = collapsed;
        ObliqueAsymptotesTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"ObliqueAsymptotes/Text") + separator + L" ")->Data());
    }
    else if ((ViewModel->TooComplexFeatures & KeyGraphFeaturesFlag::MonotoneIntervals) == KeyGraphFeaturesFlag::MonotoneIntervals)
    {
        MonotonicityList->Visibility = collapsed;
        MonotonicityTextBlock->Visibility = collapsed;
        error.append((m_resourceLoader->GetString(L"Monotonicity/Text") + separator + L" ")->Data());
    }

    TooComplexFeatures += ref new String(error.substr(0, (error.length() - (separator->Length() + 1))).c_str());
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
    VisualStateManager::GoToState(this->EquationInputTextBox, "KeyGraphFeaturesDisplayed", true);
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
}
