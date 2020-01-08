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

    EquationViewModel::EquationViewModel(Equation ^ equation, int functionLabelIndex, Windows::UI::Color color)
        : m_AnalysisErrorVisible{ false }
        , m_FunctionLabelIndex{ functionLabelIndex }
        , m_KeyGraphFeaturesItems{ ref new Vector<KeyGraphFeaturesItem ^>() }
        , m_resourceLoader{ Windows::ApplicationModel::Resources::ResourceLoader::GetForCurrentView() }
    {
        if (equation == nullptr)
        {
            throw ref new InvalidArgumentException(L"Equation cannot be null");
        }

        GraphEquation = equation;
        LineColor = color;
        IsLineEnabled = true;
    }

    void EquationViewModel::PopulateKeyGraphFeatures(KeyGraphFeaturesInfo ^ graphEquation)
    {
        if (graphEquation->AnalysisError != 0)
        {
            AnalysisErrorVisible = true;
            if (graphEquation->AnalysisError == static_cast<int>(AnalysisErrorType::AnalysisCouldNotBePerformed))
            {
                AnalysisErrorString = m_resourceLoader->GetString(L"KGFAnalysisCouldNotBePerformed");
            }
            else if (graphEquation->AnalysisError == static_cast<int>(AnalysisErrorType::AnalysisNotSupported))
            {
                AnalysisErrorString = m_resourceLoader->GetString(L"KGFAnalysisNotSupported");
            }
            return;
        }

        KeyGraphFeaturesItems->Clear();

        AddKeyGraphFeature(m_resourceLoader->GetString(L"Domain"), graphEquation->Domain, m_resourceLoader->GetString(L"KGFDomainNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"Range"), graphEquation->Range, m_resourceLoader->GetString(L"KGFRangeNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"XIntercept"), graphEquation->XIntercept, m_resourceLoader->GetString(L"KGFXInterceptNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"YIntercept"), graphEquation->YIntercept, m_resourceLoader->GetString(L"KGFYInterceptNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"Minima"), graphEquation->Minima, m_resourceLoader->GetString(L"KGFMinimaNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"Maxima"), graphEquation->Maxima, m_resourceLoader->GetString(L"KGFMaximaNone"));
        AddKeyGraphFeature(m_resourceLoader->GetString(L"InflectionPoints"), graphEquation->InflectionPoints, m_resourceLoader->GetString(L"KGFInflectionPointsNone"));
        AddKeyGraphFeature(
            m_resourceLoader->GetString(L"VerticalAsymptotes"), graphEquation->VerticalAsymptotes, m_resourceLoader->GetString(L"KGFVerticalAsymptotesNone"));
        AddKeyGraphFeature(
            m_resourceLoader->GetString(L"HorizontalAsymptotes"), graphEquation->HorizontalAsymptotes, m_resourceLoader->GetString(L"KGFHorizontalAsymptotesNone"));
        AddKeyGraphFeature(
            m_resourceLoader->GetString(L"ObliqueAsymptotes"), graphEquation->ObliqueAsymptotes, m_resourceLoader->GetString(L"KGFObliqueAsymptotesNone"));
        AddParityKeyGraphFeature(graphEquation);
        AddPeriodicityKeyGraphFeature(graphEquation);
        AddMonotoncityKeyGraphFeature(graphEquation);
        AddTooComplexKeyGraphFeature(graphEquation);

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

    void EquationViewModel::AddParityKeyGraphFeature(KeyGraphFeaturesInfo ^ graphEquation)
    {
        KeyGraphFeaturesItem ^ parityItem = ref new KeyGraphFeaturesItem();
        parityItem->Title = m_resourceLoader->GetString(L"Parity");
        switch (graphEquation->Parity)
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

    void EquationViewModel::AddPeriodicityKeyGraphFeature(KeyGraphFeaturesInfo ^ graphEquation)
    {
        KeyGraphFeaturesItem ^ periodicityItem = ref new KeyGraphFeaturesItem();
        periodicityItem->Title = m_resourceLoader->GetString(L"Periodicity");
        switch (graphEquation->PeriodicityDirection)
        {
        case 0:
            // Periodicity is not supported or is too complex to calculate.
            // Return out of this function without adding periodicity to KeyGraphFeatureItems.
            // SetTooComplexFeaturesErrorProperty will set the too complex error when periodicity is supported and unknown
            return;
        case 1:
            if (graphEquation->PeriodicityExpression == L"")
            {
                periodicityItem->DisplayItems->Append(m_resourceLoader->GetString(L"KGFPeriodicityUnknown"));
                periodicityItem->IsText = true;
            }
            else
            {
                periodicityItem->DisplayItems->Append(graphEquation->PeriodicityExpression);
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

    void EquationViewModel::AddMonotoncityKeyGraphFeature(KeyGraphFeaturesInfo ^ graphEquation)
    {
        KeyGraphFeaturesItem ^ monotonicityItem = ref new KeyGraphFeaturesItem();
        monotonicityItem->Title = m_resourceLoader->GetString(L"Monotonicity");
        if (graphEquation->Monotonicity->Size != 0)
        {
            for (auto item : graphEquation->Monotonicity)
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

    void EquationViewModel::AddTooComplexKeyGraphFeature(KeyGraphFeaturesInfo ^ graphEquation)
    {
        if (graphEquation->TooComplexFeatures <= 0)
        {
            return;
        }

        Platform::String ^ separator = ref new String(LocalizationSettings::GetInstance().GetListSeparator().c_str());

        wstring error;
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Domain) == KeyGraphFeaturesFlag::Domain)
        {
            error.append((m_resourceLoader->GetString(L"Domain") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Range) == KeyGraphFeaturesFlag::Range)
        {
            error.append((m_resourceLoader->GetString(L"Range") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Zeros) == KeyGraphFeaturesFlag::Zeros)
        {
            error.append((m_resourceLoader->GetString(L"XIntercept") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::YIntercept) == KeyGraphFeaturesFlag::YIntercept)
        {
            error.append((m_resourceLoader->GetString(L"YIntercept") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Parity) == KeyGraphFeaturesFlag::Parity)
        {
            error.append((m_resourceLoader->GetString(L"Parity") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Periodicity) == KeyGraphFeaturesFlag::Periodicity)
        {
            error.append((m_resourceLoader->GetString(L"Periodicity") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Minima) == KeyGraphFeaturesFlag::Minima)
        {
            error.append((m_resourceLoader->GetString(L"Minima") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::Maxima) == KeyGraphFeaturesFlag::Maxima)
        {
            error.append((m_resourceLoader->GetString(L"Maxima") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::InflectionPoints) == KeyGraphFeaturesFlag::InflectionPoints)
        {
            error.append((m_resourceLoader->GetString(L"InflectionPoints") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::VerticalAsymptotes) == KeyGraphFeaturesFlag::VerticalAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"VerticalAsymptotes") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::HorizontalAsymptotes) == KeyGraphFeaturesFlag::HorizontalAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"HorizontalAsymptotes") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::ObliqueAsymptotes) == KeyGraphFeaturesFlag::ObliqueAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"ObliqueAsymptotes") + separator + L" ")->Data());
        }
        if ((graphEquation->TooComplexFeatures & KeyGraphFeaturesFlag::MonotoneIntervals) == KeyGraphFeaturesFlag::MonotoneIntervals)
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
