// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Common/KeyboardShortcutManager.h"

namespace CalculatorApp
{
    namespace Controls
    {
        public ref class AppBar sealed : public Windows::UI::Xaml::Controls::AppBar
        {
        public:
            AppBar()
            {}

        protected:
            virtual void OnKeyDown(Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e) override
            {
                Windows::UI::Xaml::Controls::AppBar::OnKeyDown(e);
                if (e->Key == Windows::System::VirtualKey::Escape)
                {
                    Common::KeyboardShortcutManager::IgnoreEscape(true);
                }
            }
        };
    }
}
