// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace ViewState
    {
        extern Platform::StringReference Snap;
        extern Platform::StringReference DockedView;

        bool IsValidViewState(Platform::String ^ viewState);
    }
}
