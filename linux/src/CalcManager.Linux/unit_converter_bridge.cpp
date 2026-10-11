// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Unit converter bridge implementation. See unit_converter_bridge.h.
#include "unit_converter_bridge.h"

#include <cstring>
#include <unordered_map>

#include "utf8_util.h"

namespace calc_linux
{
    namespace
    {
        std::wstring W(const char* utf8)
        {
            return ToWide(utf8 != nullptr ? utf8 : "");
        }
    }

    UnitConversionManager::Category ToNativeCategory(const UCCategory& category)
    {
        return UnitConversionManager::Category(category.id, W(category.name), category.supportsNegative != 0);
    }

    UnitConversionManager::Unit ToNativeUnit(const UCUnit& unit)
    {
        UnitConversionManager::Unit result;
        result.id = unit.id;
        result.name = W(unit.name);
        result.accessibleName = W(unit.accessibleName);
        result.abbreviation = W(unit.abbreviation);
        result.isConversionSource = unit.isSource != 0;
        result.isConversionTarget = unit.isTarget != 0;
        result.isWhimsical = unit.isWhimsical != 0;
        return result;
    }

    UCCategory FromNativeCategory(const UnitConversionManager::Category& category)
    {
        // NOTE: name pointer aliases a thread_local cache; valid during the call only.
        thread_local std::string nameCache;
        nameCache = ToUtf8(category.name);
        UCCategory result{};
        result.id = category.id;
        result.name = nameCache.c_str();
        result.supportsNegative = category.supportsNegative ? 1 : 0;
        return result;
    }

    BridgeDataLoader::BridgeDataLoader(const UnitConverterCbs& callbacks)
        : m_callbacks(callbacks)
    {
    }

    void BridgeDataLoader::LoadData()
    {
        // Static data is built C#-side at construction; nothing to do here.
    }

    std::vector<UnitConversionManager::Category> BridgeDataLoader::GetOrderedCategories()
    {
        std::vector<UnitConversionManager::Category> result;
        if (m_callbacks.getCategories == nullptr)
        {
            return result;
        }
        int count = m_callbacks.getCategories(nullptr, 0, m_callbacks.userData);
        if (count <= 0)
        {
            return result;
        }
        std::vector<UCCategory> raw(static_cast<size_t>(count));
        int written = m_callbacks.getCategories(raw.data(), count, m_callbacks.userData);
        for (int i = 0; i < written && i < count; i++)
        {
            result.push_back(ToNativeCategory(raw[i]));
        }
        return result;
    }

    std::vector<UnitConversionManager::Unit> BridgeDataLoader::GetOrderedUnits(const UnitConversionManager::Category& category)
    {
        std::vector<UnitConversionManager::Unit> result;
        if (m_callbacks.getUnits == nullptr)
        {
            return result;
        }
        int count = m_callbacks.getUnits(category.id, nullptr, 0, m_callbacks.userData);
        if (count <= 0)
        {
            return result;
        }
        std::vector<UCUnit> raw(static_cast<size_t>(count));
        int written = m_callbacks.getUnits(category.id, raw.data(), count, m_callbacks.userData);
        for (int i = 0; i < written && i < count; i++)
        {
            result.push_back(ToNativeUnit(raw[i]));
        }
        return result;
    }

    std::unordered_map<UnitConversionManager::Unit, UnitConversionManager::ConversionData, UnitConversionManager::UnitHash>
    BridgeDataLoader::LoadOrderedRatios(const UnitConversionManager::Unit& unit)
    {
        std::unordered_map<UnitConversionManager::Unit, UnitConversionManager::ConversionData, UnitConversionManager::UnitHash> result;
        if (m_callbacks.getRatios == nullptr)
        {
            return result;
        }
        int count = m_callbacks.getRatios(unit.id, nullptr, 0, m_callbacks.userData);
        if (count <= 0)
        {
            return result;
        }
        std::vector<UCRatio> raw(static_cast<size_t>(count));
        int written = m_callbacks.getRatios(unit.id, raw.data(), count, m_callbacks.userData);
        for (int i = 0; i < written && i < count; i++)
        {
            result[ToNativeUnit(raw[i].target)] = UnitConversionManager::ConversionData(raw[i].ratio, raw[i].offset, raw[i].offsetFirst != 0);
        }
        return result;
    }

    bool BridgeDataLoader::SupportsCategory(const UnitConversionManager::Category& target)
    {
        if (m_callbacks.supportsCategory == nullptr)
        {
            return true;
        }
        return m_callbacks.supportsCategory(target.id, m_callbacks.userData) != 0;
    }

    BridgeVMCallback::BridgeVMCallback(const UnitConverterCbs& callbacks)
        : m_callbacks(callbacks)
    {
    }

    void BridgeVMCallback::DisplayCallback(const std::wstring& from, const std::wstring& to)
    {
        if (m_callbacks.onDisplay == nullptr)
        {
            return;
        }
        std::string fromUtf8 = ToUtf8(from);
        std::string toUtf8 = ToUtf8(to);
        m_callbacks.onDisplay(fromUtf8.c_str(), toUtf8.c_str(), m_callbacks.userData);
    }

    void BridgeVMCallback::SuggestedValueCallback(const std::vector<std::tuple<std::wstring, UnitConversionManager::Unit>>& suggestedValues)
    {
        if (m_callbacks.onSuggested == nullptr)
        {
            return;
        }
        // Own every string up front: nothing here may alias shared caches,
        // because all pointers must stay valid for the whole callback.
        struct OwnedSuggested
        {
            std::string value;
            std::string name;
            std::string abbr;
            std::string acc;
            UCUnit unit{};
            UCSuggested view{};
        };
        std::vector<OwnedSuggested> owned;
        owned.reserve(suggestedValues.size());
        for (const auto& entry : suggestedValues)
        {
            OwnedSuggested item;
            item.value = ToUtf8(std::get<0>(entry));
            const auto& unit = std::get<1>(entry);
            item.name = ToUtf8(unit.name);
            item.abbr = ToUtf8(unit.abbreviation);
            item.acc = ToUtf8(unit.accessibleName);
            item.unit.id = unit.id;
            item.unit.isSource = unit.isConversionSource ? 1 : 0;
            item.unit.isTarget = unit.isConversionTarget ? 1 : 0;
            item.unit.isWhimsical = unit.isWhimsical ? 1 : 0;
            owned.push_back(std::move(item));
        }
        std::vector<UCSuggested> views;
        views.reserve(owned.size());
        for (auto& item : owned)
        {
            // Seat pointers AFTER all moves, at final addresses (SSO-safe).
            item.unit.name = item.name.c_str();
            item.unit.abbreviation = item.abbr.c_str();
            item.unit.accessibleName = item.acc.c_str();
            item.view.value = item.value.c_str();
            item.view.unit = item.unit;
            views.push_back(item.view);
        }
        m_callbacks.onSuggested(views.empty() ? nullptr : views.data(), static_cast<int>(views.size()), m_callbacks.userData);
    }

    void BridgeVMCallback::MaxDigitsReached()
    {
        if (m_callbacks.onMaxDigits != nullptr)
        {
            m_callbacks.onMaxDigits(m_callbacks.userData);
        }
    }

    UnitConverterInstance::UnitConverterInstance(UnitConverterCbs callbacks)
        : callbacks(callbacks)
        , dataLoader(std::make_shared<BridgeDataLoader>(callbacks))
        , vmCallback(std::make_shared<BridgeVMCallback>(callbacks))
        , converter(std::make_unique<UnitConversionManager::UnitConverter>(dataLoader))
    {
        converter->SetViewModelCallback(vmCallback);
    }

} // namespace calc_linux

extern "C"
{
    UnitConverterHandle unit_converter_create(const UnitConverterCbs* callbacks, const char* /*regionCode*/)
    {
        if (callbacks == nullptr || callbacks->getCategories == nullptr || callbacks->getUnits == nullptr || callbacks->getRatios == nullptr)
        {
            return nullptr;
        }
        try
        {
            auto* instance = new calc_linux::UnitConverterInstance(*callbacks);
            return reinterpret_cast<UnitConverterHandle>(instance);
        }
        catch (...)
        {
            return nullptr;
        }
    }

    void unit_converter_destroy(UnitConverterHandle h)
    {
        delete reinterpret_cast<calc_linux::UnitConverterInstance*>(h);
    }

    void unit_converter_initialize(UnitConverterHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<calc_linux::UnitConverterInstance*>(h)->converter->Initialize();
        }
    }

    int unit_converter_get_categories(UnitConverterHandle h, UCCategory* out, int cap)
    {
        if (h == nullptr)
        {
            return 0;
        }
        auto* instance = reinterpret_cast<calc_linux::UnitConverterInstance*>(h);
        auto categories = instance->converter->GetCategories();
        if (out != nullptr && cap > 0)
        {
            int copy = static_cast<int>(categories.size()) < cap ? static_cast<int>(categories.size()) : cap;
            instance->scratch.clear();
            instance->scratch.reserve(static_cast<size_t>(copy));
            for (int i = 0; i < copy; i++)
            {
                instance->scratch.push_back(calc_linux::ToUtf8(categories[i].name));
                UCCategory c{};
                c.id = categories[i].id;
                c.name = instance->scratch.back().c_str();
                c.supportsNegative = categories[i].supportsNegative ? 1 : 0;
                out[i] = c;
            }
        }
        return static_cast<int>(categories.size());
    }

    int unit_converter_set_category(UnitConverterHandle h, int categoryId, UCUnit* unitsOut, int cap, int* fromId, int* toId)
    {
        if (h == nullptr)
        {
            return 0;
        }
        auto* instance = reinterpret_cast<calc_linux::UnitConverterInstance*>(h);
        // Resolve the full category (name/negate) from the loader list.
        UnitConversionManager::Category selected;
        bool found = false;
        for (const auto& cat : instance->converter->GetCategories())
        {
            if (cat.id == categoryId)
            {
                selected = cat;
                found = true;
                break;
            }
        }
        if (!found)
        {
            return 0;
        }
        auto init = instance->converter->SetCurrentCategory(selected);
        const auto& units = std::get<0>(init);
        if (unitsOut != nullptr && cap > 0)
        {
            int copy = static_cast<int>(units.size()) < cap ? static_cast<int>(units.size()) : cap;
            instance->scratch.clear();
            instance->scratch.reserve(static_cast<size_t>(copy) * 3);
            for (int i = 0; i < copy; i++)
            {
                instance->scratch.push_back(calc_linux::ToUtf8(units[i].name));
                instance->scratch.push_back(calc_linux::ToUtf8(units[i].abbreviation));
                instance->scratch.push_back(calc_linux::ToUtf8(units[i].accessibleName));
                UCUnit u{};
                u.id = units[i].id;
                u.name = instance->scratch[size_t(i) * 3].c_str();
                u.abbreviation = instance->scratch[size_t(i) * 3 + 1].c_str();
                u.accessibleName = instance->scratch[size_t(i) * 3 + 2].c_str();
                u.isSource = units[i].isConversionSource ? 1 : 0;
                u.isTarget = units[i].isConversionTarget ? 1 : 0;
                u.isWhimsical = units[i].isWhimsical ? 1 : 0;
                unitsOut[i] = u;
            }
        }
        if (fromId != nullptr)
        {
            *fromId = std::get<1>(init).id;
        }
        if (toId != nullptr)
        {
            *toId = std::get<2>(init).id;
        }
        return static_cast<int>(units.size());
    }

    int unit_converter_get_current_category(UnitConverterHandle h, UCCategory* out)
    {
        if (h == nullptr || out == nullptr)
        {
            return -1;
        }
        try
        {
            *out = calc_linux::FromNativeCategory(reinterpret_cast<calc_linux::UnitConverterInstance*>(h)->converter->GetCurrentCategory());
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }

    void unit_converter_set_unit_types(UnitConverterHandle h, int fromId, int toId)
    {
        if (h == nullptr)
        {
            return;
        }
        // Rebuild Unit structs from the loader by id (names travel with them).
        auto* instance = reinterpret_cast<calc_linux::UnitConverterInstance*>(h);
        UnitConversionManager::Category current = instance->converter->GetCurrentCategory();
        auto units = instance->dataLoader->GetOrderedUnits(current);
        const UnitConversionManager::Unit* from = nullptr;
        const UnitConversionManager::Unit* to = nullptr;
        for (const auto& unit : units)
        {
            if (unit.id == fromId)
            {
                from = &unit;
            }
            if (unit.id == toId)
            {
                to = &unit;
            }
        }
        if (from != nullptr && to != nullptr)
        {
            instance->converter->SetCurrentUnitTypes(*from, *to);
        }
    }

    void unit_converter_switch_active(UnitConverterHandle h, const char* newValue)
    {
        if (h != nullptr)
        {
            reinterpret_cast<calc_linux::UnitConverterInstance*>(h)->converter->SwitchActive(calc_linux::ToWide(newValue != nullptr ? newValue : "0"));
        }
    }

    int unit_converter_is_switched_active(UnitConverterHandle h)
    {
        return (h != nullptr && reinterpret_cast<calc_linux::UnitConverterInstance*>(h)->converter->IsSwitchedActive()) ? 1 : 0;
    }

    int unit_converter_save_prefs(UnitConverterHandle h, char* buffer, int capacity)
    {
        if (h == nullptr)
        {
            return calc_linux::CopyToBuffer("", buffer, capacity);
        }
        return calc_linux::CopyToBuffer(
            calc_linux::ToUtf8(reinterpret_cast<calc_linux::UnitConverterInstance*>(h)->converter->SaveUserPreferences()), buffer, capacity);
    }

    void unit_converter_restore_prefs(UnitConverterHandle h, const char* prefs)
    {
        if (h != nullptr && prefs != nullptr)
        {
            reinterpret_cast<calc_linux::UnitConverterInstance*>(h)->converter->RestoreUserPreferences(calc_linux::ToWide(prefs));
        }
    }

    void unit_converter_send(UnitConverterHandle h, int command)
    {
        if (h != nullptr)
        {
            reinterpret_cast<calc_linux::UnitConverterInstance*>(h)->converter->SendCommand(static_cast<UnitConversionManager::Command>(command));
        }
    }

    void unit_converter_calculate(UnitConverterHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<calc_linux::UnitConverterInstance*>(h)->converter->Calculate();
        }
    }

    void unit_converter_reset_ratios(UnitConverterHandle h)
    {
        if (h != nullptr)
        {
            reinterpret_cast<calc_linux::UnitConverterInstance*>(h)->converter->ResetCategoriesAndRatios();
        }
    }

} // extern "C"
