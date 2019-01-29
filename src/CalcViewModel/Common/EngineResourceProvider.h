// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    class EngineResourceProvider : public CalculationManager::IResourceProvider
    {
    public:
        EngineResourceProvider();
        virtual std::wstring GetCEngineString(const std::wstring& id) override;

    private:
        Windows::ApplicationModel::Resources::ResourceLoader^ m_resLoader;
    };
}
