// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "EngineResourceProvider.h"

using namespace CalculatorApp::ViewModel::Common;
using namespace Platform;
using namespace Windows::ApplicationModel::Resources;
using namespace std;

EngineResourceProvider::EngineResourceProvider()
{
    m_resLoader = ResourceLoader::GetForViewIndependentUse("CEngineStrings");
}

wstring EngineResourceProvider::GetCEngineString(wstring_view id)
{
    // The unit tests force the en-US locale (see UnitTestApp), so the engine
    // number separators are fixed to their en-US values here.
    if (id.compare(L"sDecimal") == 0)
    {
        return L".";
    }

    if (id.compare(L"sThousand") == 0)
    {
        return L",";
    }

    if (id.compare(L"sGrouping") == 0)
    {
        // CalcEngine consumes the Win32 grouping format; "3;0" groups every 3 digits.
        return L"3;0";
    }

    StringReference idRef(id.data(), id.length());
    String ^ str = m_resLoader->GetString(idRef);
    return wstring(str->Data());
}
