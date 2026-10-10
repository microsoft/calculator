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

namespace
{
    constexpr char kDefaultDecimal[] = ".";
    constexpr char kDefaultThousand[] = ",";
    constexpr char kDefaultGrouping[] = "3;0";

    // --- UTF-8 <-> wchar_t (UTF-32 on Linux) ----------------------------------
    // Implemented manually: <codecvt> is deprecated in C++17 and iconv would
    // add a link dependency for something the engine barely needs (display
    // strings with BMP math symbols and localized digits).

    void AppendUtf8(std::string& out, unsigned int cp)
    {
        if (cp < 0x80)
        {
            out.push_back(static_cast<char>(cp));
        }
        else if (cp < 0x800)
        {
            out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp < 0x10000)
        {
            out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else
        {
            out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }

    std::string ToUtf8(const std::wstring& wide)
    {
        std::string out;
        out.reserve(wide.size());
        for (wchar_t wc : wide)
        {
            unsigned int cp = static_cast<unsigned int>(wc);
            if (cp >= 0xD800 && cp <= 0xDFFF)
            {
                cp = 0xFFFD; // lone surrogate: never valid from the engine, stay safe
            }
            AppendUtf8(out, cp);
        }
        return out;
    }

    std::wstring ToWide(const std::string& utf8)
    {
        std::wstring out;
        out.reserve(utf8.size());
        size_t i = 0;
        while (i < utf8.size())
        {
            unsigned char c = static_cast<unsigned char>(utf8[i]);
            unsigned int cp = 0xFFFD;
            size_t len = 1;
            if (c < 0x80)
            {
                cp = c;
                len = 1;
            }
            else if ((c >> 5) == 0x6 && i + 1 < utf8.size())
            {
                cp = ((c & 0x1F) << 6) | (utf8[i + 1] & 0x3F);
                len = 2;
            }
            else if ((c >> 4) == 0xE && i + 2 < utf8.size())
            {
                cp = ((c & 0x0F) << 12) | ((utf8[i + 1] & 0x3F) << 6) | (utf8[i + 2] & 0x3F);
                len = 3;
            }
            else if ((c >> 3) == 0x1E && i + 3 < utf8.size())
            {
                cp = ((c & 0x07) << 18) | ((utf8[i + 1] & 0x3F) << 12) | ((utf8[i + 2] & 0x3F) << 6) | (utf8[i + 3] & 0x3F);
                len = 4;
            }
            out.push_back(static_cast<wchar_t>(cp));
            i += len;
        }
        return out;
    }

    int CopyToBuffer(const std::string& text, char* buffer, int capacity)
    {
        int needed = static_cast<int>(text.size()) + 1; // NUL included
        if (buffer != nullptr && capacity > 0)
        {
            size_t copy = text.size();
            if (copy > static_cast<size_t>(capacity) - 1)
            {
                // Truncate on a UTF-8 boundary so we never split a code point.
                copy = static_cast<size_t>(capacity) - 1;
                while (copy > 0 && (static_cast<unsigned char>(text[copy]) >> 6) == 0x2)
                {
                    --copy;
                }
            }
            std::memcpy(buffer, text.data(), copy);
            buffer[copy] = '\0';
        }
        return needed;
    }

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
            std::string key = ToUtf8(std::wstring(id));
            if (key == "sDecimal")
            {
                return ToWide(m_decimal);
            }
            if (key == "sThousand")
            {
                return ToWide(m_thousand);
            }
            if (key == "sGrouping")
            {
                return ToWide(m_grouping);
            }
            auto it = kCEngineStringsEn.find(key);
            if (it != kCEngineStringsEn.end())
            {
                return ToWide(it->second);
            }
            return L"";
        }

    private:
        std::string m_decimal;
        std::string m_thousand;
        std::string m_grouping;
    };

    // --- Display forwarder -------------------------------------------------------

    class DisplayForwarder : public ICalcDisplay
    {
    public:
        explicit DisplayForwarder(CalcDisplayCallbacks callbacks)
            : m_callbacks(callbacks)
        {
        }

        void SetPrimaryDisplay(const std::wstring& text, bool isError) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_primaryUtf8 = ToUtf8(text);
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
            std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& /*commands*/) override
        {
            if (m_callbacks.onExpression == nullptr || tokens == nullptr)
            {
                return;
            }
            std::vector<std::string> utf8;
            std::vector<const char*> ptrs;
            std::vector<int> indices;
            utf8.reserve(tokens->size());
            for (const auto& pair : *tokens)
            {
                utf8.push_back(ToUtf8(pair.first));
                indices.push_back(pair.second);
            }
            ptrs.reserve(utf8.size());
            for (const auto& s : utf8)
            {
                ptrs.push_back(s.c_str());
            }
            m_callbacks.onExpression(
                ptrs.empty() ? nullptr : ptrs.data(),
                indices.empty() ? nullptr : indices.data(),
                static_cast<int>(ptrs.size()),
                m_callbacks.userData);
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

        void SetMemorizedNumbers(const std::vector<std::wstring>& /*numbers*/) override
        {
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

        bool IsInError()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_isError;
        }

    private:
        CalcDisplayCallbacks m_callbacks;
        std::mutex m_mutex;
        std::string m_primaryUtf8 = "0";
        bool m_isError = false;
    };

    struct CalcManagerInstance
    {
        // Order matters: manager borrows both.
        DisplayForwarder forwarder;
        LinuxResourceProvider resources;
        std::unique_ptr<CalculationManager::CalculatorManager> manager;

        explicit CalcManagerInstance(CalcDisplayCallbacks callbacks, std::string decimalSep, std::string thousandSep, std::string grouping)
            : forwarder(callbacks)
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
            return CopyToBuffer("0", buffer, capacity);
        }
        return CopyToBuffer(reinterpret_cast<CalcManagerInstance*>(h)->forwarder.GetPrimaryUtf8(), buffer, capacity);
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
            return CopyToBuffer("0", buffer, capacity);
        }
        std::wstring wide =
            reinterpret_cast<CalcManagerInstance*>(h)->manager->GetResultForRadix(radix, precision, groupDigits != 0);
        return CopyToBuffer(ToUtf8(wide), buffer, capacity);
    }

    int calc_manager_get_decimal_separator(CalcManagerHandle h, char* buffer, int capacity)
    {
        if (h == nullptr)
        {
            return CopyToBuffer(kDefaultDecimal, buffer, capacity);
        }
        wchar_t sep = reinterpret_cast<CalcManagerInstance*>(h)->manager->DecimalSeparator();
        return CopyToBuffer(ToUtf8(std::wstring(1, sep)), buffer, capacity);
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

} // extern "C"
