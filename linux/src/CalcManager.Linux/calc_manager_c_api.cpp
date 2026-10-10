// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux C ABI shim over CalcManager. See calc_manager_c_api.h.
#include "calc_manager_c_api.h"

#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "CalculatorManager.h"
#include "CalculatorResource.h"
#include "Command.h"
#include "Header Files/ICalcDisplay.h"
#include "Header Files/RadixType.h"

#include "engine_strings_en.inc"
#include "utf8_util.h"

namespace
{
    constexpr char kDefaultDecimal[] = ".";
    constexpr char kDefaultThousand[] = ",";
    constexpr char kDefaultGrouping[] = "3;0";

    // --- Resource provider ------------------------------------------------------
    // English engine strings (generated from en-US CEngineStrings.resw) plus
    // locale separators supplied at create() time. Mirrors the C# engine
    // resource provider contract (sDecimal/sThousand/sGrouping + numeric ids).

    class LinuxResourceProvider : public CalculationManager::IResourceProvider
    {
    public:
        LinuxResourceProvider(std::string decimalSep, std::string thousandSep, std::string grouping)
            : m_decimal(std::move(decimalSep))
            , m_thousand(std::move(thousandSep))
            , m_grouping(std::move(grouping))
        {
        }

        std::wstring GetCEngineString(std::wstring_view id) override
        {
            std::string key = calc_linux::ToUtf8(std::wstring(id));
            if (key == "sDecimal")
            {
                return calc_linux::ToWide(m_decimal);
            }
            if (key == "sThousand")
            {
                return calc_linux::ToWide(m_thousand);
            }
            if (key == "sGrouping")
            {
                return calc_linux::ToWide(m_grouping);
            }
            auto it = kCEngineStringsEn.find(key);
            if (it != kCEngineStringsEn.end())
            {
                return calc_linux::ToWide(it->second);
            }
            return L"";
        }

    private:
        std::string m_decimal;
        std::string m_thousand;
        std::string m_grouping;
    };

    // Maps a token's command index to a CalculationManager::CommandType id
    // (0 Unary, 1 Binary, 2 Operand, 3 Parentheses), or -1 when the token
    // has no command. Mirrors how the C# VM classifies tokens for display.
    int CommandTypeFor(
        int commandIndex,
        std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& commands)
    {
        if (commands == nullptr || commandIndex < 0 || commandIndex >= static_cast<int>(commands->size()))
        {
            return -1;
        }
        const auto& command = (*commands)[static_cast<size_t>(commandIndex)];
        if (command == nullptr)
        {
            return -1;
        }
        return static_cast<int>(command->GetCommandType());
    }

    // Flattens structured commands to replayable engine command ids.
    // Must match StandardCalculatorViewModel.GetCommandsFromExpressionCommands:
    // unary -> all codes; binary/paren -> single code; operand -> codes with
    // CommandSIGN inserted after the first non-zero code when negative.
    void FlattenCommands(
        const std::vector<std::shared_ptr<IExpressionCommand>>& commands,
        std::vector<int>& out)
    {
        constexpr int commandSign = static_cast<int>(CalculationManager::Command::CommandSIGN);
        constexpr int commandZero = static_cast<int>(CalculationManager::Command::Command0);
        for (const auto& command : commands)
        {
            if (command == nullptr)
            {
                continue;
            }
            switch (command->GetCommandType())
            {
            case CalculationManager::CommandType::UnaryCommand:
                if (auto* unary = dynamic_cast<IUnaryCommand*>(command.get()))
                {
                    const auto& codes = unary->GetCommands();
                    if (codes != nullptr)
                    {
                        out.insert(out.end(), codes->begin(), codes->end());
                    }
                }
                break;
            case CalculationManager::CommandType::BinaryCommand:
                if (auto* binary = dynamic_cast<IBinaryCommand*>(command.get()))
                {
                    out.push_back(binary->GetCommand());
                }
                break;
            case CalculationManager::CommandType::Parentheses:
                if (auto* paren = dynamic_cast<IParenthesisCommand*>(command.get()))
                {
                    out.push_back(paren->GetCommand());
                }
                break;
            case CalculationManager::CommandType::OperandCommand:
                if (auto* opnd = dynamic_cast<IOpndCommand*>(command.get()))
                {
                    const auto& codes = opnd->GetCommands();
                    if (codes != nullptr)
                    {
                        bool needSign = opnd->IsNegative();
                        for (int code : *codes)
                        {
                            out.push_back(code);
                            if (needSign && code != commandZero)
                            {
                                out.push_back(commandSign);
                                needSign = false;
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    // --- Display forwarder -------------------------------------------------------

    class DisplayForwarder : public ICalcDisplay
    {
    public:
        explicit DisplayForwarder(CalcDisplayCallbacks callbacks, CalcExpressionExFn onExpressionEx = nullptr)
            : m_callbacks(callbacks)
            , m_onExpressionEx(onExpressionEx)
        {
        }

        void SetPrimaryDisplay(const std::wstring& text, bool isError) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_primaryUtf8 = calc_linux::ToUtf8(text);
            m_isError = isError;
            if (m_callbacks.onPrimaryDisplay != nullptr)
            {
                m_callbacks.onPrimaryDisplay(m_primaryUtf8.c_str(), isError ? 1 : 0, m_callbacks.userData);
            }
        }

        void SetIsInError(bool isError) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_isError = isError;
            if (m_callbacks.onIsInError != nullptr)
            {
                m_callbacks.onIsInError(isError ? 1 : 0, m_callbacks.userData);
            }
        }

        void SetExpressionDisplay(
            std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
            std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& commands) override
        {
            if ((m_callbacks.onExpression == nullptr && m_onExpressionEx == nullptr) || tokens == nullptr)
            {
                return;
            }
            std::vector<std::string> utf8;
            std::vector<const char*> ptrs;
            std::vector<int> indices;
            std::vector<int> cmdTypes;
            utf8.reserve(tokens->size());
            for (const auto& pair : *tokens)
            {
                utf8.push_back(calc_linux::ToUtf8(pair.first));
                indices.push_back(pair.second);
                cmdTypes.push_back(CommandTypeFor(pair.second, commands));
            }
            ptrs.reserve(utf8.size());
            for (const auto& s : utf8)
            {
                ptrs.push_back(s.c_str());
            }
            const char** tokenPtrs = ptrs.empty() ? nullptr : ptrs.data();
            const int* indexPtrs = indices.empty() ? nullptr : indices.data();
            if (m_callbacks.onExpression != nullptr)
            {
                m_callbacks.onExpression(tokenPtrs, indexPtrs, static_cast<int>(ptrs.size()), m_callbacks.userData);
            }
            if (m_onExpressionEx != nullptr)
            {
                m_onExpressionEx(
                    tokenPtrs,
                    indexPtrs,
                    cmdTypes.empty() ? nullptr : cmdTypes.data(),
                    static_cast<int>(ptrs.size()),
                    m_callbacks.userData);
            }
        }

        void SetParenthesisNumber(unsigned int count) override
        {
            if (m_callbacks.onParenthesis != nullptr)
            {
                m_callbacks.onParenthesis(count, m_callbacks.userData);
            }
        }

        void OnNoRightParenAdded() override
        {
            if (m_callbacks.onNoRightParenAdded != nullptr)
            {
                m_callbacks.onNoRightParenAdded(m_callbacks.userData);
            }
        }

        void MaxDigitsReached() override
        {
            if (m_callbacks.onMaxDigitsReached != nullptr)
            {
                m_callbacks.onMaxDigitsReached(m_callbacks.userData);
            }
        }

        void BinaryOperatorReceived() override
        {
            if (m_callbacks.onBinaryOperatorReceived != nullptr)
            {
                m_callbacks.onBinaryOperatorReceived(m_callbacks.userData);
            }
        }

        void OnHistoryItemAdded(unsigned int index) override
        {
            if (m_callbacks.onHistoryItemAdded != nullptr)
            {
                m_callbacks.onHistoryItemAdded(index, m_callbacks.userData);
            }
        }

        void SetMemorizedNumbers(const std::vector<std::wstring>& numbers) override
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_memorized.clear();
                m_memorized.reserve(numbers.size());
                for (const auto& number : numbers)
                {
                    m_memorized.push_back(calc_linux::ToUtf8(number));
                }
            }
            if (m_callbacks.onMemoryChanged != nullptr)
            {
                m_callbacks.onMemoryChanged(-1, m_callbacks.userData);
            }
        }

        void MemoryItemChanged(unsigned int index) override
        {
            if (m_callbacks.onMemoryChanged != nullptr)
            {
                m_callbacks.onMemoryChanged(static_cast<int>(index), m_callbacks.userData);
            }
        }

        void InputChanged() override
        {
            if (m_callbacks.onInputChanged != nullptr)
            {
                m_callbacks.onInputChanged(m_callbacks.userData);
            }
        }

        std::string GetPrimaryUtf8()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_primaryUtf8;
        }

        std::vector<std::string> GetMemorizedCopy()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_memorized;
        }

        bool IsInError()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_isError;
        }

    private:
        CalcDisplayCallbacks m_callbacks;
        CalcExpressionExFn m_onExpressionEx;
        std::mutex m_mutex;
        std::string m_primaryUtf8 = "0";
        bool m_isError = false;
        std::vector<std::string> m_memorized;
    };

    struct CalcManagerInstance
    {
        // Order matters: manager borrows both.
        DisplayForwarder forwarder;
        LinuxResourceProvider resources;
        std::unique_ptr<CalculationManager::CalculatorManager> manager;

        explicit CalcManagerInstance(
            CalcDisplayCallbacks callbacks,
            CalcExpressionExFn onExpressionEx,
            std::string decimalSep,
            std::string thousandSep,
            std::string grouping)
            : forwarder(callbacks, onExpressionEx)
            , resources(std::move(decimalSep), std::move(thousandSep), std::move(grouping))
            , manager(std::make_unique<CalculationManager::CalculatorManager>(&forwarder, &resources))
        {
        }
    };

    const char* FirstNonNull(const char* value, const char* fallback)
    {
        return value != nullptr ? value : fallback;
    }

} // namespace

extern "C"
{
    CalcManagerHandle calc_manager_create(
        const CalcDisplayCallbacks* callbacks,
        const char* decimalSep,
        const char* thousandSep,
        const char* grouping)
    {
        if (callbacks == nullptr || callbacks->onPrimaryDisplay == nullptr)
        {
            return nullptr;
        }
        try
        {
            auto* instance = new CalcManagerInstance(
                *callbacks,
                nullptr,
                FirstNonNull(decimalSep, kDefaultDecimal),
                FirstNonNull(thousandSep, kDefaultThousand),
                FirstNonNull(grouping, kDefaultGrouping));
            return reinterpret_cast<CalcManagerHandle>(instance);
        }
        catch (...)
        {
            return nullptr;
        }
    }

    void calc_manager_destroy(CalcManagerHandle h)
    {
        delete reinterpret_cast<CalcManagerInstance*>(h);
    }

    void calc_manager_reset(CalcManagerHandle h, int clearMemory)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->Reset(clearMemory != 0);
        }
    }

    void calc_manager_set_standard_mode(CalcManagerHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->SetStandardMode();
        }
    }

    void calc_manager_set_scientific_mode(CalcManagerHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->SetScientificMode();
        }
    }

    void calc_manager_set_programmer_mode(CalcManagerHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->SetProgrammerMode();
        }
    }

    void calc_manager_send_command(CalcManagerHandle h, int commandId)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->SendCommand(static_cast<CalculationManager::Command>(commandId));
        }
    }

    void calc_manager_memorize(CalcManagerHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->MemorizeNumber();
        }
    }

    void calc_manager_memory_load(CalcManagerHandle h, unsigned int index)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->MemorizedNumberLoad(index);
        }
    }

    void calc_manager_memory_add(CalcManagerHandle h, unsigned int index)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->MemorizedNumberAdd(index);
        }
    }

    void calc_manager_memory_subtract(CalcManagerHandle h, unsigned int index)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->MemorizedNumberSubtract(index);
        }
    }

    void calc_manager_memory_clear(CalcManagerHandle h, unsigned int index)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->MemorizedNumberClear(index);
        }
    }

    void calc_manager_memory_clear_all(CalcManagerHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->MemorizedNumberClearAll();
        }
    }

    int calc_manager_is_engine_recording(CalcManagerHandle h)
    {
        return (h != nullptr && reinterpret_cast<CalcManagerInstance*>(h)->manager->IsEngineRecording()) ? 1 : 0;
    }

    int calc_manager_is_input_empty(CalcManagerHandle h)
    {
        return (h != nullptr && reinterpret_cast<CalcManagerInstance*>(h)->manager->IsInputEmpty()) ? 1 : 0;
    }

    int calc_manager_is_in_error(CalcManagerHandle h)
    {
        return (h != nullptr && reinterpret_cast<CalcManagerInstance*>(h)->forwarder.IsInError()) ? 1 : 0;
    }

    int calc_manager_get_primary_display(CalcManagerHandle h, char* buffer, int capacity)
    {
        if (h == nullptr)
        {
            return calc_linux::CopyToBuffer("0", buffer, capacity);
        }
        return calc_linux::CopyToBuffer(reinterpret_cast<CalcManagerInstance*>(h)->forwarder.GetPrimaryUtf8(), buffer, capacity);
    }

    void calc_manager_set_radix(CalcManagerHandle h, int radixType)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->SetRadix(static_cast<RadixType>(radixType));
        }
    }

    void calc_manager_set_precision(CalcManagerHandle h, int precision)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->SetPrecision(precision);
        }
    }

    void calc_manager_update_max_int_digits(CalcManagerHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->UpdateMaxIntDigits();
        }
    }

    int calc_manager_get_result_for_radix(
        CalcManagerHandle h,
        unsigned int radix,
        int precision,
        int groupDigits,
        char* buffer,
        int capacity)
    {
        if (h == nullptr)
        {
            return calc_linux::CopyToBuffer("0", buffer, capacity);
        }
        std::wstring wide =
            reinterpret_cast<CalcManagerInstance*>(h)->manager->GetResultForRadix(radix, precision, groupDigits != 0);
        return calc_linux::CopyToBuffer(calc_linux::ToUtf8(wide), buffer, capacity);
    }

    int calc_manager_get_decimal_separator(CalcManagerHandle h, char* buffer, int capacity)
    {
        if (h == nullptr)
        {
            return calc_linux::CopyToBuffer(kDefaultDecimal, buffer, capacity);
        }
        wchar_t sep = reinterpret_cast<CalcManagerInstance*>(h)->manager->DecimalSeparator();
        return calc_linux::CopyToBuffer(calc_linux::ToUtf8(std::wstring(1, sep)), buffer, capacity);
    }

    unsigned int calc_manager_history_count(CalcManagerHandle h)
    {
        if (h == nullptr)
        {
            return 0;
        }
        return static_cast<unsigned int>(reinterpret_cast<CalcManagerInstance*>(h)->manager->GetHistoryItems().size());
    }

    void calc_manager_history_clear(CalcManagerHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->ClearHistory();
        }
    }

    int calc_manager_get_current_degree_mode(CalcManagerHandle h)
    {
        if (h == nullptr)
        {
            return 0;
        }
        return static_cast<int>(reinterpret_cast<CalcManagerInstance*>(h)->manager->GetCurrentDegreeMode());
    }

    void calc_manager_set_in_history_load_mode(CalcManagerHandle h, int enabled)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->SetInHistoryItemLoadMode(enabled != 0);
        }
    }

    // --- Task 4 additive section --------------------------------------------------

    CalcManagerHandle calc_manager_create_ex(
        const CalcDisplayCallbacksEx* callbacks,
        const char* decimalSep,
        const char* thousandSep,
        const char* grouping)
    {
        if (callbacks == nullptr || callbacks->base.onPrimaryDisplay == nullptr)
        {
            return nullptr;
        }
        try
        {
            auto* instance = new CalcManagerInstance(
                callbacks->base,
                callbacks->onExpressionEx,
                FirstNonNull(decimalSep, kDefaultDecimal),
                FirstNonNull(thousandSep, kDefaultThousand),
                FirstNonNull(grouping, kDefaultGrouping));
            return reinterpret_cast<CalcManagerHandle>(instance);
        }
        catch (...)
        {
            return nullptr;
        }
    }

} // extern "C"

namespace
{
    const std::vector<std::shared_ptr<CalculationManager::HISTORYITEM>>* HistoryForMode(
        CalculationManager::CalculatorManager* manager,
        int mode)
    {
        if (manager == nullptr)
        {
            return nullptr;
        }
        auto calcMode = (mode == 0) ? CalculationManager::CalculatorMode::Standard
                                    : CalculationManager::CalculatorMode::Scientific;
        return &manager->GetHistoryItems(calcMode);
    }

    const CalculationManager::HISTORYITEM* HistoryItemAt(
        CalculationManager::CalculatorManager* manager,
        int mode,
        unsigned int index)
    {
        const auto* history = HistoryForMode(manager, mode);
        if (history == nullptr || index >= history->size())
        {
            return nullptr;
        }
        const auto& item = (*history)[index];
        return item.get();
    }
} // namespace

extern "C"
{

    int calc_manager_history_item(
        CalcManagerHandle h,
        int mode,
        unsigned int index,
        char* expression,
        int expressionCapacity,
        char* result,
        int resultCapacity)
    {
        if (h == nullptr)
        {
            return -1;
        }
        const auto* item = HistoryItemAt(reinterpret_cast<CalcManagerInstance*>(h)->manager.get(), mode, index);
        if (item == nullptr)
        {
            return -1;
        }
        calc_linux::CopyToBuffer(calc_linux::ToUtf8(item->historyItemVector.expression), expression, expressionCapacity);
        calc_linux::CopyToBuffer(calc_linux::ToUtf8(item->historyItemVector.result), result, resultCapacity);
        return 0;
    }

    unsigned int calc_manager_history_count_for_mode(CalcManagerHandle h, int mode)
    {
        if (h == nullptr)
        {
            return 0;
        }
        const auto* history = HistoryForMode(reinterpret_cast<CalcManagerInstance*>(h)->manager.get(), mode);
        return history == nullptr ? 0 : static_cast<unsigned int>(history->size());
    }

    unsigned int calc_manager_max_history_size(CalcManagerHandle h)
    {
        if (h == nullptr)
        {
            return 0;
        }
        return static_cast<unsigned int>(reinterpret_cast<CalcManagerInstance*>(h)->manager->MaxHistorySize());
    }

    int calc_manager_remove_history_item(CalcManagerHandle h, unsigned int index)
    {
        if (h == nullptr)
        {
            return 0;
        }
        return reinterpret_cast<CalcManagerInstance*>(h)->manager->RemoveHistoryItem(index) ? 1 : 0;
    }

    unsigned int calc_manager_history_item_token_count(CalcManagerHandle h, int mode, unsigned int index)
    {
        if (h == nullptr)
        {
            return 0;
        }
        const auto* item = HistoryItemAt(reinterpret_cast<CalcManagerInstance*>(h)->manager.get(), mode, index);
        if (item == nullptr || item->historyItemVector.spTokens == nullptr)
        {
            return 0;
        }
        return static_cast<unsigned int>(item->historyItemVector.spTokens->size());
    }

    int calc_manager_history_item_token(
        CalcManagerHandle h,
        int mode,
        unsigned int item,
        unsigned int token,
        char* buffer,
        int capacity,
        int* cmdIndex,
        int* cmdType)
    {
        if (h == nullptr)
        {
            return -1;
        }
        const auto* historyItem =
            HistoryItemAt(reinterpret_cast<CalcManagerInstance*>(h)->manager.get(), mode, item);
        if (historyItem == nullptr || historyItem->historyItemVector.spTokens == nullptr
            || token >= historyItem->historyItemVector.spTokens->size())
        {
            return -1;
        }
        const auto& pair = (*historyItem->historyItemVector.spTokens)[token];
        calc_linux::CopyToBuffer(calc_linux::ToUtf8(pair.first), buffer, capacity);
        if (cmdIndex != nullptr)
        {
            *cmdIndex = pair.second;
        }
        if (cmdType != nullptr)
        {
            *cmdType = CommandTypeFor(pair.second, historyItem->historyItemVector.spCommands);
        }
        return 0;
    }

    unsigned int calc_manager_history_item_commands(
        CalcManagerHandle h,
        int mode,
        unsigned int index,
        int* out,
        unsigned int capacity)
    {
        std::vector<int> flat;
        if (h != nullptr)
        {
            const auto* item = HistoryItemAt(reinterpret_cast<CalcManagerInstance*>(h)->manager.get(), mode, index);
            if (item != nullptr && item->historyItemVector.spCommands != nullptr)
            {
                std::vector<std::shared_ptr<IExpressionCommand>> commands(
                    item->historyItemVector.spCommands->begin(), item->historyItemVector.spCommands->end());
                FlattenCommands(commands, flat);
            }
        }
        if (out != nullptr && capacity > 0)
        {
            size_t copy = flat.size() < capacity ? flat.size() : capacity;
            for (size_t i = 0; i < copy; i++)
            {
                out[i] = flat[i];
            }
        }
        return static_cast<unsigned int>(flat.size());
    }

    unsigned int calc_manager_display_commands(CalcManagerHandle h, int* out, unsigned int capacity)
    {
        std::vector<int> flat;
        if (h != nullptr)
        {
            FlattenCommands(reinterpret_cast<CalcManagerInstance*>(h)->manager->GetDisplayCommandsSnapshot(), flat);
        }
        if (out != nullptr && capacity > 0)
        {
            size_t copy = flat.size() < capacity ? flat.size() : capacity;
            for (size_t i = 0; i < copy; i++)
            {
                out[i] = flat[i];
            }
        }
        return static_cast<unsigned int>(flat.size());
    }

    unsigned int calc_manager_memorized_count(CalcManagerHandle h)
    {
        if (h == nullptr)
        {
            return 0;
        }
        return static_cast<unsigned int>(reinterpret_cast<CalcManagerInstance*>(h)->forwarder.GetMemorizedCopy().size());
    }

    int calc_manager_memorized_at(CalcManagerHandle h, unsigned int index, char* buffer, int capacity)
    {
        if (h == nullptr)
        {
            return -1;
        }
        std::vector<std::string> numbers = reinterpret_cast<CalcManagerInstance*>(h)->forwarder.GetMemorizedCopy();
        if (index >= numbers.size())
        {
            return -1;
        }
        return calc_linux::CopyToBuffer(numbers[index], buffer, capacity);
    }

    void calc_manager_refresh_memorized_numbers(CalcManagerHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<CalcManagerInstance*>(h)->manager->SetMemorizedNumbersString();
        }
    }

} // extern "C"
