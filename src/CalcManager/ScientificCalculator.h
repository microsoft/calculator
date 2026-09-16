// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "Command.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class ICalcDisplay;
namespace CalculationManager
{
    class CalculatorHistory;
    class IResourceProvider;
    class ScientificCalculator
    {
    public:
        ScientificCalculator(IResourceProvider*, ICalcDisplay*, std::shared_ptr<CalculatorHistory>);
        ~ScientificCalculator();
        ScientificCalculator(const ScientificCalculator&) = delete;
        ScientificCalculator& operator=(const ScientificCalculator&) = delete;
        static bool IsCommandSupported(Command) noexcept;
        void ProcessCommand(Command);
        void Reset();
        bool IsInputEmpty() const;
        bool IsEngineRecording() const;
        void SetPrecision(int32_t);
        wchar_t DecimalSeparator() const;
        std::wstring GetResult() const;
        void DisplayError(int32_t);
        void MemorizeNumber();
        void MemorizedNumberLoad(unsigned index);
        void MemorizedNumberAdd(unsigned index);
        void MemorizedNumberSubtract(unsigned index);
        void MemorizedNumberClear(unsigned index);
        void MemorizedNumberClearAll();
        void PublishMemory();
        std::string GetExpression() const;
        void LoadExpression(const std::string& expression);
        std::wstring SaveState() const;
        void RestoreState(const std::wstring& state);
        void EditToken(unsigned index, Command command, bool append = false);
        Command Angle() const;
        bool ScientificFormat() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
