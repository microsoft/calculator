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
    EquationViewModel::EquationViewModel()
        : m_LineColor{ nullptr }
        , m_Expression{ "" }
        , m_Domain{ "" }
        , m_Range{ "" }
        , m_XIntercept{ "" }
        , m_YIntercept{ "" }
        , m_Parity{ -1 }
        , m_Periodicity{ ref new Map<String ^, String ^>() }
        , m_ParityString{ "" }
        , m_PeriodicityString{ "" }
        , m_Minima{ ref new Vector<String ^>() }
        , m_Maxima{ ref new Vector<String ^>() }
        , m_InflectionPoints{ ref new Vector<String ^>() }
        , m_Monotonicity{ ref new Map<String ^, String ^>() }
        , m_MonotonicityString{ ref new Vector<String ^>() }
        , m_VerticalAsymptotes{ ref new Vector<String ^>() }
        , m_HorizontalAsymptotes{ ref new Vector<String ^>() }
        , m_ObliqueAsymptotes{ ref new Vector<String ^>() }
        , m_TooComplexFeatures{ -1 }
        , m_AnalysisError{ 0 }
        , m_AnalysisErrorVisible{ false }
        , m_AnalysisErrorString{ "" }
        , m_TooComplexFeaturesString{ "" }
        , m_resourceLoader{ Windows::ApplicationModel::Resources::ResourceLoader::GetForCurrentView() }
    {
    }

    void EquationViewModel::OnPropertyChanged(String ^ propertyName)
    {
        if (propertyName == L"AnalysisError" && AnalysisError != 0)
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
        else if (propertyName == L"XIntercept")
        {
            XIntercept = XIntercept != L"" ? XIntercept : m_resourceLoader->GetString(L"KGFXInterceptNone");
        }
        else if (propertyName == L"YIntercept")
        {
            YIntercept = YIntercept != L"" ? YIntercept : m_resourceLoader->GetString(L"KGFYInterceptNone");
        }
        else if (propertyName == L"Minima")
        {
            if (Minima->Size == 0)
            {
                Minima->Append(m_resourceLoader->GetString(L"KGFMinimaNone"));
            }
        }
        else if (propertyName == L"Maxima")
        {
            if (Maxima->Size == 0)
            {
                Maxima->Append(m_resourceLoader->GetString(L"KGFMaximaNone"));
            }
        }
        else if (propertyName == L"InflectionPoints")
        {
            if (InflectionPoints->Size == 0)
            {
                InflectionPoints->Append(m_resourceLoader->GetString(L"KGFInflectionPointsNone"));
            }
        }
        else if (propertyName == L"VerticalAsymptotes")
        {
            if (VerticalAsymptotes->Size == 0)
            {
                VerticalAsymptotes->Append(m_resourceLoader->GetString(L"KGFVerticalAsymptotesNone"));
            }
        }
        else if (propertyName == L"HorizontalAsymptotes")
        {
            if (HorizontalAsymptotes->Size == 0)
            {
                HorizontalAsymptotes->Append(m_resourceLoader->GetString(L"KGFHorizontalAsymptotesNone"));
            }
        }
        else if (propertyName == L"ObliqueAsymptotes")
        {
            if (ObliqueAsymptotes->Size == 0)
            {
                ObliqueAsymptotes->Append(m_resourceLoader->GetString(L"KGFObliqueAsymptotesNone"));
            }
        }

        else if (propertyName == L"Parity")
        {
            SetParityStringProperty();
        }
        else if (propertyName == L"Periodicity")
        {
            SetPeriodicityStringProperty();
        }
        else if (propertyName == L"Monotonicity")
        {
            SetMonotoncityStringProperty();
        }
        else if (propertyName == L"TooComplexFeatures")
        {
            SetTooComplexFeaturesErrorProperty();
        }

        AnalysisErrorVisible = false;
    }

    void EquationViewModel::SetParityStringProperty()
    {
        switch (Parity)
        {
        case 0:
            ParityString = m_resourceLoader->GetString(L"KGFParityUnknown");
            break;
        case 1:
            ParityString = m_resourceLoader->GetString(L"KGFParityOdd");
            break;
        case 2:
            ParityString = m_resourceLoader->GetString(L"KGFParityEven");
            break;
        case 3:
            ParityString = m_resourceLoader->GetString(L"KGFParityNeither");
            break;
        default:
            ParityString = m_resourceLoader->GetString(L"KGFParityError");
        }
    }
    void EquationViewModel::SetPeriodicityStringProperty()
    {
        if (Periodicity->Size != 0)
        {
            auto periodicityType = Periodicity->First()->Current->Key->Data();
            switch (*periodicityType)
            {
            case '0':
                PeriodicityString = m_resourceLoader->GetString(L"KGFPeriodicityUnknown");
                break;
            case '1':
                PeriodicityString = Periodicity->First()->Current->Value;
                if (PeriodicityString == L"")
                {
                    PeriodicityString = m_resourceLoader->GetString(L"KGFPeriodicityError");
                }
                break;
            case '2':
                PeriodicityString = m_resourceLoader->GetString(L"KGFPeriodicityNotPeriodic");
                break;
            default:
                PeriodicityString = m_resourceLoader->GetString(L"KGFPeriodicityError");
            }
        }
        else
        {
            PeriodicityString = m_resourceLoader->GetString(L"KGFPeriodicityError");
        }
    }

    void EquationViewModel::SetMonotoncityStringProperty()
    {
        Monotonicity->Clear();
        if (Monotonicity->Size != 0)
        {
            wstring directionStartTag = L"<mml:mi><mml:mspace/><mml:mspace/><mml:mspace/><mml:mspace/><mml:mspace/>";
            wstring directionEndTag = L"</mml:mi>";
            for (auto item : Monotonicity)
            {
                String ^ monotonicityItem = ref new String();

                wstring itemWString(item->Key->Data());
                auto index = itemWString.find(L"</mml:math>");

                wstring direction;

                auto monotonicityType = item->Value->Data();
                switch (*monotonicityType)
                {
                case '0':
                    direction = directionStartTag + m_resourceLoader->GetString(L"KGFMonotonicityUnknown")->Data() + directionEndTag;
                    break;
                case '1':
                    direction = directionStartTag + m_resourceLoader->GetString(L"KGFMonotonicityIncreasing")->Data() + directionEndTag;
                    break;
                case '2':
                    direction = directionStartTag + m_resourceLoader->GetString(L"KGFMonotonicityDecreasing")->Data() + directionEndTag;
                    break;
                case '3':
                    direction = directionStartTag + m_resourceLoader->GetString(L"KGFMonotonicityConstant")->Data() + directionEndTag;
                    break;
                default:
                    direction = directionStartTag + m_resourceLoader->GetString(L"KGFMonotonicityError")->Data() + directionEndTag;
                    break;
                }

                itemWString.insert(index, direction);

                monotonicityItem = ref new String(itemWString.c_str());
                MonotonicityString->Append(monotonicityItem);
            }
        }
        else
        {
            String ^ monotonicityErrorItem = ref new String();
            monotonicityErrorItem = m_resourceLoader->GetString(L"KGFMonotonicityError");
            MonotonicityString->Append(monotonicityErrorItem);
        }
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
            error.append((m_resourceLoader->GetString(L"Domain/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Range) == KeyGraphFeaturesFlag::Range)
        {
            error.append((m_resourceLoader->GetString(L"Range/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Zeros) == KeyGraphFeaturesFlag::Zeros)
        {
            error.append((m_resourceLoader->GetString(L"XIntercept/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::YIntercept) == KeyGraphFeaturesFlag::YIntercept)
        {
            error.append((m_resourceLoader->GetString(L"YIntercept/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Parity) == KeyGraphFeaturesFlag::Parity)
        {
            error.append((m_resourceLoader->GetString(L"Parity/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Periodicity) == KeyGraphFeaturesFlag::Periodicity)
        {
            error.append((m_resourceLoader->GetString(L"Periodicity/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Minima) == KeyGraphFeaturesFlag::Minima)
        {
            error.append((m_resourceLoader->GetString(L"Minima/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::Maxima) == KeyGraphFeaturesFlag::Maxima)
        {
            error.append((m_resourceLoader->GetString(L"Maxima/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::InflectionPoints) == KeyGraphFeaturesFlag::InflectionPoints)
        {
            error.append((m_resourceLoader->GetString(L"InflectionPoints/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::VerticalAsymptotes) == KeyGraphFeaturesFlag::VerticalAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"VerticalAsymptotes/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::HorizontalAsymptotes) == KeyGraphFeaturesFlag::HorizontalAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"HorizontalAsymptotes/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::ObliqueAsymptotes) == KeyGraphFeaturesFlag::ObliqueAsymptotes)
        {
            error.append((m_resourceLoader->GetString(L"ObliqueAsymptotes/Text") + separator + L" ")->Data());
        }
        else if ((TooComplexFeatures & KeyGraphFeaturesFlag::MonotoneIntervals) == KeyGraphFeaturesFlag::MonotoneIntervals)
        {
            error.append((m_resourceLoader->GetString(L"Monotonicity/Text") + separator + L" ")->Data());
        }

        TooComplexFeaturesString = ref new String(error.substr(0, (error.length() - (separator->Length() + 1))).c_str());
    }

}
