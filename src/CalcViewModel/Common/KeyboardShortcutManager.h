// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Utils.h"
#include "MyVirtualKey.h"

namespace CalculatorApp
{
    namespace Common
    {
        public ref class KeyboardShortcutManager sealed
        {
        public:
            KeyboardShortcutManager() {}

            DEPENDENCY_PROPERTY_OWNER(KeyboardShortcutManager);

            DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(Platform::String^, Character);
            DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(MyVirtualKey, VirtualKey);
            DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(MyVirtualKey, VirtualKeyControlChord);
            DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(MyVirtualKey, VirtualKeyShiftChord);
            DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(MyVirtualKey, VirtualKeyAltChord);
            DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(MyVirtualKey, VirtualKeyControlShiftChord);
            DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(MyVirtualKey, VirtualKeyInverseChord);
            DEPENDENCY_PROPERTY_ATTACHED_WITH_CALLBACK(MyVirtualKey, VirtualKeyControlInverseChord);

        internal:

            static void Initialize();

            // Sometimes, like with popups, escape is treated as special and even
            // though it is handled we get it passed through to us. In those cases
            // we need to be able to ignore it (looking at e->Handled isn't sufficient
            // because that always returns true).
            // The onlyOnce flag is used to indicate whether we should only ignore the
            // next escape, or keep ignoring until you explicitly HonorEscape.
            static void IgnoreEscape(bool onlyOnce);
            static void HonorEscape();
            static void HonorShortcuts(bool allow);
            static void HandledEnter(bool ishandled);
            static void UpdateDropDownState(bool);
            static void ShiftButtonChecked(bool checked);
            static void UpdateDropDownState(Windows::UI::Xaml::Controls::Flyout^ aboutPageFlyout);

            static void RegisterNewAppViewId();
            static void OnWindowClosed(int viewId);

        private:

            static void OnCharacterPropertyChanged(
                Windows::UI::Xaml::DependencyObject^ target,
                Platform::String^ oldValue,
                Platform::String^ newValue);

            static void OnVirtualKeyPropertyChanged(
                Windows::UI::Xaml::DependencyObject^ target,
                MyVirtualKey oldValue,
                MyVirtualKey newValue);

            static void OnVirtualKeyControlChordPropertyChanged(
                Windows::UI::Xaml::DependencyObject^ target,
                MyVirtualKey oldValue,
                MyVirtualKey newValue);

            static void OnVirtualKeyShiftChordPropertyChanged(
                Windows::UI::Xaml::DependencyObject^ target,
                MyVirtualKey oldValue,
                MyVirtualKey newValue);

            static void OnVirtualKeyInverseChordPropertyChanged(
                Windows::UI::Xaml::DependencyObject^ target,
                MyVirtualKey oldValue,
                MyVirtualKey newValue);

            static void OnVirtualKeyControlInverseChordPropertyChanged(
                Windows::UI::Xaml::DependencyObject^ target,
                MyVirtualKey oldValue,
                MyVirtualKey newValue);

            static void OnVirtualKeyAltChordPropertyChanged(
                Windows::UI::Xaml::DependencyObject^ target,
                MyVirtualKey oldValue,
                MyVirtualKey newValue);

            static void OnVirtualKeyControlShiftChordPropertyChanged(
                Windows::UI::Xaml::DependencyObject^ target,
                MyVirtualKey oldValue,
                MyVirtualKey newValue);

            static void OnCharacterReceivedHandler(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CharacterReceivedEventArgs^ args);
            static void OnKeyDownHandler(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args);
            static void OnKeyUpHandler(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args);
            static void OnAcceleratorKeyActivated(Windows::UI::Core::CoreDispatcher^, Windows::UI::Core::AcceleratorKeyEventArgs^ args);
        };
    }
}
