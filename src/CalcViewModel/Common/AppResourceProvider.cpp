// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AppResourceProvider.h"

using namespace Platform;
using namespace Windows::ApplicationModel::Resources;
using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;

AppResourceProvider::AppResourceProvider()
{
    m_stringResLoader = ResourceLoader::GetForViewIndependentUse();
    m_cEngineStringResLoader = ResourceLoader::GetForViewIndependentUse(L"CEngineStrings");
}

AppResourceProvider ^ AppResourceProvider::GetInstance()
{
    static AppResourceProvider ^ s_instance = ref new AppResourceProvider();
    return s_instance;
}

String ^ AppResourceProvider::GetResourceString(_In_ String ^ key)
{
    return m_stringResLoader->GetString(key);
}

String ^ AppResourceProvider::GetCEngineString(_In_ String ^ key)
{
    return m_cEngineStringResLoader->GetString(key);
}
