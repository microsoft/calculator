// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace ViewState
    {
        extern Plat::StringReference Snap;
        extern Plat::StringReference DockedView;

        bool IsValidViewState(Plat::String ^ viewState);
    }
}
