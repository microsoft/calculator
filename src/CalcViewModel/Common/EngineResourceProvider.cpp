// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EngineResourceProvider.h"
#include "Common/LocalizationSettings.h"

using namespace CalculatorApp::ViewModel::Common;
using namespace Platform;
using namespace Windows::ApplicationModel::Resources;
using namespace std;

namespace CalculatorApp
{
    EngineResourceProvider::EngineResourceProvider()
    {
        m_resLoader = ResourceLoader::GetForViewIndependentUse("CEngineStrings");
    }

    wstring EngineResourceProvider::GetCEngineString(wstring_view id)
    {
        LocalizationSettings^ localizationSettings = LocalizationSettings::GetInstance();

        if (id.compare(L"sDecimal") == 0)
        {
            return localizationSettings->GetDecimalSeparatorStr();
        }

        if (id.compare(L"sThousand") == 0)
        {
            return localizationSettings->GetNumberGroupingSeparatorStr();
        }

        if (id.compare(L"sGrouping") == 0)
        {
            // The following groupings are the onces that CalcEngine supports.
            //   0;0             0x000          - no grouping
            //   3;0             0x003          - group every 3 digits
            //   3;2;0           0x023          - group 1st 3 and then every 2 digits
            //   4;0             0x004          - group every 4 digits
            //   5;3;2;0         0x235          - group 5, then 3, then every 2
            wstring numberGroupingString = localizationSettings->GetNumberGroupingStr();
            return numberGroupingString;
        }

        StringReference idRef(id.data(), id.length());
        String ^ str = m_resLoader->GetString(idRef);
        return str->Begin();
    }
}
