// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewState.h"

namespace CalculatorApp
{
    namespace ViewState
    {
        Platform::StringReference Snap(L"Snap");
        Platform::StringReference DockedView(L"DockedView");

        bool IsValidViewState(Platform::String ^ viewState)
        {
            return viewState->Equals(ViewState::Snap) || viewState->Equals(ViewState::DockedView);
        }
    }
}
