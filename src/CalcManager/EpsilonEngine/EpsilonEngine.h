// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "../Command.h"

#include <cstdint>
#include <memory>
#include <string>

class ICalcDisplay;

namespace CalculationManager
{
    class CalculatorHistory;
    class IResourceProvider;

    class EpsilonEngine
    {
    public:
        EpsilonEngine(
            IResourceProvider* resourceProvider,
            ICalcDisplay* displayCallback,
            std::shared_ptr<CalculatorHistory> history);
        ~EpsilonEngine();

        EpsilonEngine(const EpsilonEngine&) = delete;
        EpsilonEngine& operator=(const EpsilonEngine&) = delete;

        static bool IsCommandSupported(Command command) noexcept;
        void ProcessCommand(Command command);
        void Reset();
        bool IsInputEmpty() const;
        bool IsEngineRecording() const;
        void SetPrecision(int32_t precision);
        wchar_t DecimalSeparator() const;
        std::wstring GetResult() const;
        void DisplayError(int32_t errorCode);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
