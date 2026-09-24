// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/CalculatorResource.h"

namespace CalculatorApp::ViewModel::Common
{
    // Minimal engine resource provider for the native CalcManager unit tests.
    // Self-contained (no CalcViewModel dependency): engine strings come from the
    // CEngineStrings resources, and the number separators are read from the
    // current user locale.
    class EngineResourceProvider : public CalculationManager::IResourceProvider
    {
    public:
        EngineResourceProvider();
        std::wstring GetCEngineString(std::wstring_view id) override;

    private:
        Windows::ApplicationModel::Resources::ResourceLoader ^ m_resLoader;
    };
}
