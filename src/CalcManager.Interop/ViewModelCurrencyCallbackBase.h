#pragma once
#include "ViewModelCurrencyCallbackBase.g.h"

namespace winrt::CalcManager::Interop::implementation
{
    struct ViewModelCurrencyCallbackBase : ViewModelCurrencyCallbackBaseT<ViewModelCurrencyCallbackBase>
    {
        ViewModelCurrencyCallbackBase() = default;

        void CurrencyDataLoadFinished(bool) {}
        void CurrencySymbolsCallback(hstring const&, hstring const&) {}
        void CurrencyRatiosCallback(hstring const&, hstring const&) {}
        void CurrencyTimestampCallback(hstring const&, bool) {}
        void NetworkBehaviorChanged(int32_t) {}
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct ViewModelCurrencyCallbackBase : ViewModelCurrencyCallbackBaseT<ViewModelCurrencyCallbackBase, implementation::ViewModelCurrencyCallbackBase>
    {
    };
}
