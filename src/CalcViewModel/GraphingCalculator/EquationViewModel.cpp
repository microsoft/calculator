#include "pch.h"
#include "EquationViewModel.h"
#include "CalcViewModel\Common\LocalizationSettings.h"
#include "CalcViewModel\GraphingCalculatorEnums.h"

using namespace CalculatorApp::Common;
using namespace Graphing;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation::Collections;

namespace CalculatorApp::ViewModel
{
    GridDisplayItems::GridDisplayItems()
        : m_Expression{ "" }
        , m_Direction{ "" }
    {
    }

    KeyGraphFeaturesItem::KeyGraphFeaturesItem()
        : m_Title{ "" }
        , m_DisplayItems{ ref new Vector<String ^>() }
        , m_GridItems{ ref new Vector<GridDisplayItems ^>() }
        , m_IsText{ false }
    {
    }

    EquationViewModel::EquationViewModel()
        : m_LineColor{ nullptr }
        , m_Expression{ "" }
        , m_IsAnalysisUpdated{ false }
        , m_Domain{ "" }
        , m_Range{ "" }
        , m_XIntercept{ "" }
        , m_YIntercept{ "" }
        , m_Parity{ -1 }
        , m_PeriodicityDirection{ -1 }
        , m_PeriodicityExpression{ "" }
        , m_Minima{ ref new Vector<String ^>() }
        , m_Maxima{ ref new Vector<String ^>() }
        , m_InflectionPoints{ ref new Vector<String ^>() }
        , m_Monotonicity{ ref new Map<String ^, String ^>() }
        , m_VerticalAsymptotes{ ref new Vector<String ^>() }
        , m_HorizontalAsymptotes{ ref new Vector<String ^>() }
        , m_ObliqueAsymptotes{ ref new Vector<String ^>() }
        , m_TooComplexFeatures{ -1 }
        , m_AnalysisError{ 0 }
        , m_AnalysisErrorVisible{ false }
        , m_KeyGraphFeaturesItems{ ref new Vector<KeyGraphFeaturesItem ^>() }
        , m_resourceLoader{ Windows::ApplicationModel::Resources::ResourceLoader::GetForCurrentView() }
    {
    }

    void EquationViewModel::OnPropertyChanged(String ^ propertyName)
    {
        if (propertyName == L"IsAnalysisUpdated" && IsAnalysisUpdated)
        {
            if (AnalysisError != 0)
            {
                AnalysisErrorVisible = true;
                if (AnalysisError == static_cast<int>(AnalysisErrorType::AnalysisCouldNotBePerformed))
                {
                    AnalysisErrorString = m_resourceLoader->GetString(L"KGFAnalysisCouldNotBePerformed");
                }
                else if (AnalysisError == static_cast<int>(AnalysisErrorType::AnalysisNotSupported))
                {
                    AnalysisErrorString = m_resourceLoader->GetString(L"KGFAnalysisNotSupported");
                }
                return;
            }
            KeyGraphFeaturesItems->Clear();
            SetKeyGraphFeaturesItems(m_resourceLoader->GetString(L"Domain"), Domain, m_resourceLoader->GetString(L"KGFDomainNone"));
            SetKeyGraphFeaturesItems(m_resourceLoader->GetString(L"Range"), Range, m_resourceLoader->GetString(L"KGFRangeNone"));
            SetKeyGraphFeaturesItems(m_resourceLoader->GetString(L"XIntercept"), XIntercept, m_resourceLoader->GetString(L"KGFXInterceptNone"));
            SetKeyGraphFeaturesItems(m_resourceLoader->GetString(L"YIntercept"), XIntercept, m_resourceLoader->GetString(L"KGFYInterceptNone"));
            SetKeyGraphFeaturesItems(m_resourceLoader->GetString(L"Minima"), Minima, m_resourceLoader->GetString(L"KGFMinimaNone"));
            SetKeyGraphFeaturesItems(m_resourceLoader->GetString(L"Maxima"), Maxima, m_resourceLoader->GetString(L"KGFMaximaNone"));
            SetKeyGraphFeaturesItems(
                m_resourceLoader->GetString(L"InflectionPoints"), InflectionPoints, m_resourceLoader->GetString(L"KGFInflectionPointsNone"));
            SetKeyGraphFeaturesItems(
                m_resourceLoader->GetString(L"VerticalAsymptotes"), VerticalAsymptotes, m_resourceLoader->GetString(L"KGFVerticalAsymptotesNone"));
            SetKeyGraphFeaturesItems(
                m_resourceLoader->GetString(L"HorizontalAsymptotes"), HorizontalAsymptotes, m_resourceLoader->GetString(L"KGFHorizontalAsymptotesNone"));
            SetKeyGraphFeaturesItems(
                m_resourceLoader->GetString(L"ObliqueAsymptotes"), ObliqueAsymptotes, m_resourceLoader->GetString(L"KGFObliqueAsymptotesNone"));
            SetParityStringProperty();
            SetPeriodicityStringProperty();
            SetMonotoncityStringProperty();
            SetTooComplexFeaturesErrorProperty();

            AnalysisErrorVisible = false;
            IsAnalysisUpdated = false;
        }
    }

    void EquationViewModel::SetKeyGraphFeaturesItems(String ^ title, String ^ expression, String ^ errorString)
    {
        KeyGraphFeaturesItem ^ item = ref new KeyGraphFeaturesItem();
        item->Title = title;
        if (expression != L"")
        {

            item->DisplayItems->Append(expression);
            item->IsText = false;
        }
        else
        {
            item->DisplayItems->Append(errorString);
            item->IsText = true;
        }
        KeyGraphFeaturesItems->Append(item);
    }

    void EquationViewModel::SetKeyGraphFeaturesItems(String ^ title, IObservableVector<String ^> ^ expressionVector, String ^ errorString)
    {
        KeyGraphFeaturesItem ^ item = ref new KeyGraphFeaturesItem();
        item->Title = title;
        if (expressionVector->Size != 0)
        {
            for (auto expression : expressionVector)
            {
                item->DisplayItems->Append(expression);
            }
            item->IsText = false;
        }
        else
        {
            item->DisplayItems->Append(errorString);
            item->IsText = true;
        }
        KeyGraphFeaturesItems->Append(item);
    }

    void EquationViewModel::SetParityStringProperty()
    {
        KeyGraphFeaturesItem ^ parityItem = ref new KeyGraphFeaturesItem();
        parityItem->Title = m_resourceLoader->GetString(L"Parity");
        switch (Parity)
        {
        case 0:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityUnknown"));
            parityItem->IsText = true;
            break;
        case 1:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityOdd"));
            parityItem->IsText = true;
            break;
        case 2:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityEven"));
            parityItem->IsText = true;
            break;
        case 3:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityNeither"));
            parityItem->IsText = true;
            break;
        default:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityUnknown"));
            parityItem->IsText = true;
        }

        KeyGraphFeaturesItems->Append(parityItem);
    }
    void EquationViewModel::SetPeriodicityStringProperty()
    {
        KeyGraphFeaturesItem ^ periodicityItem = ref new KeyGraphFeaturesItem();
        periodicityItem->Title = m_resourceLoader->GetString(L"Periodicity");
        switch (PeriodicityDirection)
        {
        case 0:
            // Periodicity is not supported or is too complex to calculate.
            // Return out of this function without adding periodicity to KeyGraphFeatureItems.
            // SetTooComplexFeaturesErrorProperty will set the too complex error when periodicity is supported and unknown
            return;
        case 1:
            if (PeriodicityExpression == L"")
            {
                periodicityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFPeriodicityUnknown"));
                periodicityItem->IsText = true;
            }
            else
            {
                periodicityItem->DisplayItems->Append(PeriodicityExpression);
                periodicityItem->IsText = false;
            }
            break;
        case 2:
            periodicityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFPeriodicityNotPeriodic"));
            periodicityItem->IsText = false;
            break;
        default:
            periodicityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFPeriodicityError"));
            periodicityItem->IsText = true;
        }

        KeyGraphFeaturesItems->Append(periodicityItem);
    }

    void EquationViewModel::SetMonotoncityStringProperty()
    {
        KeyGraphFeaturesItem ^ monotonicityItem = ref new KeyGraphFeaturesItem();
        monotonicityItem->Title = m_resourceLoader->GetString(L"Monotonicity");
        if (Monotonicity->Size != 0)
        {
            for (auto item : Monotonicity)
            {
                GridDisplayItems ^ gridItem = ref new GridDisplayItems();
                gridItem->Expression = item->Key;

                auto monotonicityType = item->Value->Data();
                switch (*monotonicityType)
                {
                case '0':
                    gridItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityUnknown");
                    break;
                case '1':
                    gridItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityIncreasing");
                    break;
                case '2':
                    gridItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityDecreasing");
                    break;
                case '3':
                    gridItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityConstant");
                    break;
                default:
                    gridItem->Direction = m_resourceLoader->GetString(L"KGFMonotonicityError");
                    break;
                }

                monotonicityItem->GridItems->Append(gridItem);
            }
            monotonicityItem->IsText = false;
        }
        else
        {
            monotonicityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFMonotonicityError"));
            monotonicityItem->IsText = true;
        }

        KeyGraphFeaturesItems->Append(monotonicityItem);
    }

    void EquationViewModel::SetTooComplexFeaturesErrorProperty()
    {
        if (TooComplexFeatures <= 0)
        {
            return;
        }

        Platform::String ^ separator = ref new String(LocalizationSettings::GetInstance().GetListSeparator().c_str());

        wstring error;
        if ((TooComplexFeatures & KeyGraphFeaturesFlag::Domain) == KeyGraphFeaturesFlag::Domain)
        {
            error.append((m_resourceLoader->GetString(L"Domain") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Range) == KeyGraphFeaturesFlag::Range)
        {
            error.append((m_resourceLoader->GetString(L"Range") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Zeros) == KeyGraphFeaturesFlag::Zeros)
        {
            error.append((m_resourceLoader->GetString(L"XIntercept") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::YIntercept) == KeyGraphFeaturesFlag::YIntercept)
        {
            error.append((m_resourceLoader->GetString(L"YIntercept") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Parity) == KeyGraphFeaturesFlag::Parity)
        {
            error.append((m_resourceLoader->GetString(L"Parity") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Periodicity) == KeyGraphFeaturesFlag::Periodicity)
        {
            error.append((m_resourceLoader->GetString(L"Periodicity") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Minima) == KeyGraphFeaturesFlag::Minima)
        {
            error.append((m_resourceLoader->GetString(L"Minima") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Maxima) == KeyGraphFeaturesFlag::Maxima)
        {
            error.append((m_resourceLoader->GetString(L"Maxima") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::InflectionPoints) == KeyGraphFeaturesFlag::InflectionPoints)
        {
            error.append((m_resourceLoader->GetString(L"InflectionPoints") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::VerticalAsymptotes) == KeyGraphFeaturesFlag::VerticalAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"VerticalAsymptotes") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::HorizontalAsymptotes) == KeyGraphFeaturesFlag::HorizontalAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"HorizontalAsymptotes") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::ObliqueAsymptotes) == KeyGraphFeaturesFlag::ObliqueAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"ObliqueAsymptotes") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::MonotoneIntervals) == KeyGraphFeaturesFlag::MonotoneIntervals)
        {
            error.append((m_resourceLoader->GetString(L"Monotonicity") + separator + L" ")->Data());
        }

        KeyGraphFeaturesItem ^ tooComplexItem = ref new KeyGraphFeaturesItem();
        tooComplexItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFTooComplexFeaturesError"));
        tooComplexItem->DisplayItems->Append(ref new String(error.substr(0, (error.length() - (separator->Length() + 1))).c_str()));
        tooComplexItem->IsText = true;

        KeyGraphFeaturesItems->Append(tooComplexItem);
    }

}
