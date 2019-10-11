// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewState.h"

namespace CalculatorApp
{
    namespace ViewState
    {
        Plat::StringReference Snap(L"Snap");
        Plat::StringReference DockedView(L"DockedView");

        bool IsValidViewState(Plat::String ^ viewState)
        {
            return viewState->Equals(ViewState::Snap) || viewState->Equals(ViewState::DockedView);
        }
    }
}
