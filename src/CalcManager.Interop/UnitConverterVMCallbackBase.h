#pragma once
#include "UnitConverterVMCallbackBase.g.h"

namespace winrt::CalcManager::Interop::implementation
{
    struct UnitConverterVMCallbackBase : UnitConverterVMCallbackBaseT<UnitConverterVMCallbackBase>
    {
        UnitConverterVMCallbackBase() = default;

        void DisplayCallback(hstring const&, hstring const&) {}
        void SuggestedValueCallback(array_view<CalcManager::Interop::SuggestedValueWrapper const> /*values*/) {}
        void MaxDigitsReached() {}
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct UnitConverterVMCallbackBase : UnitConverterVMCallbackBaseT<UnitConverterVMCallbackBase, implementation::UnitConverterVMCallbackBase>
    {
    };
}
