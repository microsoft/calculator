// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ViewState.h"
#include "pch.h"

namespace CalculatorApp {
namespace ViewState {
Platform::StringReference Snap(L"Snap");
Platform::StringReference DockedView(L"DockedView");

bool IsValidViewState(Platform::String ^ viewState) {
  return viewState->Equals(ViewState::Snap) ||
         viewState->Equals(ViewState::DockedView);
}
} // namespace ViewState
} // namespace CalculatorApp
