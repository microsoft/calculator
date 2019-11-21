// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
using namespace GraphControl;

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

    EquationViewModel::EquationViewModel(GraphControl::Equation ^ equation)
        : m_AnalysisErrorVisible{ false }
        , m_FunctionLabelIndex{ 0 }
        , m_KeyGraphFeaturesItems{ ref new Vector<KeyGraphFeaturesItem ^>() }
        , m_resourceLoader{ Windows::ApplicationModel::Resources::ResourceLoader::GetForCurrentView() }
    {
        if (equation == nullptr)
        {
            throw ref new InvalidArgumentException(L"Equation cannot be null");
        }

        GraphEquation = equation;
    }

    void EquationViewModel::PopulateKeyGraphFeatures()
    {
        if (GraphEquation->AnalysisError != 0)
        {
            AnalysisErrorVisible = true;
            if (GraphEquation->AnalysisError == static_cast<int>(AnalysisErrorType::AnalysisCouldNotBePerformed))
            {
                AnalysisErrorString = m_resourceLoader->GetString(L"KGFAnalysisCouldNotBePerformed");
            }
            else if (GraphEquation->AnalysisError == static_cast<int>(AnalysisErrorType::AnalysisNotSupported))
            {
                AnalysisErrorString = m_resourceLoader->GetString(L"KGFAnalysisNotSupported");
            }
            return;
        }

        KeyGraphFeaturesItems->Clear();

        AddKeyGraphFeature(m_resourceLoader->GetString(L"Domain"), GraphEquation->Domain, m_resourceLoader->GetString(L"KGFDomainNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"Range"), GraphEquation->Range, m_resourceLoader->GetString(L"KGFRangeNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"XIntercept"), GraphEquation->XIntercept, m_resourceLoader->GetString(L"KGFXInterceptNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"YIntercept"), GraphEquation->YIntercept, m_resourceLoader->GetString(L"KGFYInterceptNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"Minima"), GraphEquation->Minima, m_resourceLoader->GetString(L"KGFMinimaNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"Maxima"), GraphEquation->Maxima, m_resourceLoader->GetString(L"KGFMaximaNone"));
        AddKeyGraphFeature(
            m_resourceLoader->GetString(L"InflectionPoints"), GraphEquation->InflectionPoints, m_resourceLoader->GetString(L"KGFInflectionPointsNone"));
        AddKeyGraphFeature(
            m_resourceLoader->GetString(L"VerticalAsymptotes"), GraphEquation->VerticalAsymptotes, m_resourceLoader->GetString(L"KGFVerticalAsymptotesNone"));
        AddKeyGraphFeature(
            m_resourceLoader->GetString(L"HorizontalAsymptotes"),
            GraphEquation->HorizontalAsymptotes,
            m_resourceLoader->GetString(L"KGFHorizontalAsymptotesNone"));
        AddKeyGraphFeature(
            m_resourceLoader->GetString(L"ObliqueAsymptotes"), GraphEquation->ObliqueAsymptotes, m_resourceLoader->GetString(L"KGFObliqueAsymptotesNone"));
        AddParityKeyGraphFeature();
        AddPeriodicityKeyGraphFeature();
        AddMonotoncityKeyGraphFeature();
        AddTooComplexKeyGraphFeature();

        AnalysisErrorVisible = false;
    }

    void EquationViewModel::AddKeyGraphFeature(String ^ title, String ^ expression, String ^ errorString)
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

    void EquationViewModel::AddKeyGraphFeature(String ^ title, IVector<String ^> ^ expressionVector, String ^ errorString)
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

    void EquationViewModel::AddParityKeyGraphFeature()
    {
        KeyGraphFeaturesItem ^ parityItem = ref new KeyGraphFeaturesItem();
        parityItem->Title = m_resourceLoader->GetString(L"Parity");
        switch (GraphEquation->Parity)
        {
        case 0:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityUnknown"));
            break;
        case 1:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityOdd"));
            break;
        case 2:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityEven"));
            break;
        case 3:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityNeither"));
            break;
        default:
            parityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFParityUnknown"));
        }
        parityItem->IsText = true;

        KeyGraphFeaturesItems->Append(parityItem);
    }

    void EquationViewModel::AddPeriodicityKeyGraphFeature()
    {
        KeyGraphFeaturesItem ^ periodicityItem = ref new KeyGraphFeaturesItem();
        periodicityItem->Title = m_resourceLoader->GetString(L"Periodicity");
        switch (GraphEquation->PeriodicityDirection)
        {
        case 0:
            // Periodicity is not supported or is too complex to calculate.
            // Return out of this function without adding periodicity to KeyGraphFeatureItems.
            // SetTooComplexFeaturesErrorProperty will set the too complex error when periodicity is supported and unknown
            return;
        case 1:
            if (GraphEquation->PeriodicityExpression == L"")
            {
                periodicityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFPeriodicityUnknown"));
                periodicityItem->IsText = true;
            }
            else
            {
                periodicityItem->DisplayItems->Append(GraphEquation->PeriodicityExpression);
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

    void EquationViewModel::AddMonotoncityKeyGraphFeature()
    {
        KeyGraphFeaturesItem ^ monotonicityItem = ref new KeyGraphFeaturesItem();
        monotonicityItem->Title = m_resourceLoader->GetString(L"Monotonicity");
        if (GraphEquation->Monotonicity->Size != 0)
        {
            for (auto item : GraphEquation->Monotonicity)
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

    void EquationViewModel::AddTooComplexKeyGraphFeature()
    {
        if (GraphEquation->TooComplexFeatures <= 0)
        {
            return;
        }

        Platform::String ^ separator = ref new String(LocalizationSettings::GetInstance().GetListSeparator().c_str());

        wstring error;
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Domain) == KeyGraphFeaturesFlag::Domain)
        {
            error.append((m_resourceLoader->GetString(L"Domain") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Range) == KeyGraphFeaturesFlag::Range)
        {
            error.append((m_resourceLoader->GetString(L"Range") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Zeros) == KeyGraphFeaturesFlag::Zeros)
        {
            error.append((m_resourceLoader->GetString(L"XIntercept") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::YIntercept) == KeyGraphFeaturesFlag::YIntercept)
        {
            error.append((m_resourceLoader->GetString(L"YIntercept") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Parity) == KeyGraphFeaturesFlag::Parity)
        {
            error.append((m_resourceLoader->GetString(L"Parity") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Periodicity) == KeyGraphFeaturesFlag::Periodicity)
        {
            error.append((m_resourceLoader->GetString(L"Periodicity") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Minima) == KeyGraphFeaturesFlag::Minima)
        {
            error.append((m_resourceLoader->GetString(L"Minima") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Maxima) == KeyGraphFeaturesFlag::Maxima)
        {
            error.append((m_resourceLoader->GetString(L"Maxima") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::InflectionPoints) == KeyGraphFeaturesFlag::InflectionPoints)
        {
            error.append((m_resourceLoader->GetString(L"InflectionPoints") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::VerticalAsymptotes) == KeyGraphFeaturesFlag::VerticalAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"VerticalAsymptotes") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::HorizontalAsymptotes) == KeyGraphFeaturesFlag::HorizontalAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"HorizontalAsymptotes") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::ObliqueAsymptotes) == KeyGraphFeaturesFlag::ObliqueAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"ObliqueAsymptotes") + separator + L" ")->Data());
        }
        if ((GraphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::MonotoneIntervals) == KeyGraphFeaturesFlag::MonotoneIntervals)
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
