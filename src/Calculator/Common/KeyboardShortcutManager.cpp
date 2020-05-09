// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "KeyboardShortcutManager.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "CalcViewModel/ApplicationViewModel.h"
#include "CalcViewModel/Common/LocalizationSettings.h"

using namespace Concurrency;
using namespace Platform;
using namespace std;
using namespace std::chrono;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::System;
using namespace Utils;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;

namespace MUXC = Microsoft::UI::Xaml::Controls;

DEPENDENCY_PROPERTY_INITIALIZATION(KeyboardShortcutManager, Character);
DEPENDENCY_PROPERTY_INITIALIZATION(KeyboardShortcutManager, VirtualKey);
DEPENDENCY_PROPERTY_INITIALIZATION(KeyboardShortcutManager, VirtualKeyControlChord);
DEPENDENCY_PROPERTY_INITIALIZATION(KeyboardShortcutManager, VirtualKeyShiftChord);
DEPENDENCY_PROPERTY_INITIALIZATION(KeyboardShortcutManager, VirtualKeyAltChord);
DEPENDENCY_PROPERTY_INITIALIZATION(KeyboardShortcutManager, VirtualKeyControlShiftChord);

static multimap<int, multimap<wchar_t, WeakReference>> s_CharacterForButtons;
static multimap<int, multimap<MyVirtualKey, WeakReference>> s_VirtualKeysForButtons;
static multimap<int, multimap<MyVirtualKey, WeakReference>> s_VirtualKeyControlChordsForButtons;
static multimap<int, multimap<MyVirtualKey, WeakReference>> s_VirtualKeyShiftChordsForButtons;
static multimap<int, multimap<MyVirtualKey, WeakReference>> s_VirtualKeyAltChordsForButtons;
static multimap<int, multimap<MyVirtualKey, WeakReference>> s_VirtualKeyControlShiftChordsForButtons;

static map<int, bool> s_IsDropDownOpen;

static map<int, bool> s_ignoreNextEscape;
static map<int, bool> s_keepIgnoringEscape;
static map<int, bool> s_fHonorShortcuts;
static map<int, bool> s_fDisableShortcuts;

static reader_writer_lock s_keyboardShortcutMapLock;

namespace CalculatorApp
{
    namespace Common
    {
        // Lights up all of the buttons in the given range
        // The range is defined by a pair of iterators
        template <typename T>
        void LightUpButtons(const T& buttons)
        {
            auto iterator = buttons.first;
            for (; iterator != buttons.second; ++iterator)
            {
                auto button = iterator->second.Resolve<ButtonBase>();
                if (button && button->IsEnabled)
                {
                    LightUpButton(button);
                }
            }
        }

        void LightUpButton(ButtonBase ^ button)
        {
            // If the button is a toggle button then we don't need
            // to change the UI of the button
            if (dynamic_cast<ToggleButton ^>(button))
            {
                return;
            }

            // The button will go into the visual Pressed state with this call
            VisualStateManager::GoToState(button, "Pressed", true);

            // This timer will fire after lightUpTime and make the button
            // go back to the normal state.
            // This timer will only fire once after which it will be destroyed
            auto timer = ref new DispatcherTimer();
            TimeSpan lightUpTime{};
            lightUpTime.Duration = 500000L; // Half second (in 100-ns units)
            timer->Interval = lightUpTime;

            WeakReference timerWeakReference(timer);
            WeakReference buttonWeakReference(button);
            timer->Tick += ref new EventHandler<Object ^>([buttonWeakReference, timerWeakReference](Object ^, Object ^) {
                auto button = buttonWeakReference.Resolve<ButtonBase>();
                if (button)
                {
                    VisualStateManager::GoToState(button, "Normal", true);
                }

                // Cancel the timer after we're done so it only fires once
                auto timer = timerWeakReference.Resolve<DispatcherTimer>();
                if (timer)
                {
                    timer->Stop();
                }
            });
            timer->Start();
        }

        // Looks for the first button reference that it can resolve
        // and execute its command.
        // NOTE: It is assumed that all buttons associated with a particular
        // key have the same command
        template <typename T>
        void RunFirstEnabledButtonCommand(const T& buttons)
        {
            auto buttonIterator = buttons.first;
            for (; buttonIterator != buttons.second; ++buttonIterator)
            {
                auto button = buttonIterator->second.Resolve<ButtonBase>();
                if (button && button->IsEnabled)
                {
                    RunButtonCommand(button);
                    break;
                }
            }
        }

        void RunButtonCommand(ButtonBase ^ button)
        {
            if (button->IsEnabled)
            {
                auto command = button->Command;
                auto parameter = button->CommandParameter;
                if (command && command->CanExecute(parameter))
                {
                    command->Execute(parameter);
                }

                auto radio = dynamic_cast<RadioButton ^>(button);
                if (radio)
                {
                    radio->IsChecked = true;
                    return;
                }

                auto toggle = dynamic_cast<ToggleButton ^>(button);
                if (toggle)
                {
                    toggle->IsChecked = !(toggle->IsChecked != nullptr && toggle->IsChecked->Value);
                    return;
                }
            }
        }
    }
}

void KeyboardShortcutManager::IgnoreEscape(bool onlyOnce)
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    int viewId = Utils::GetWindowId();

    if (s_ignoreNextEscape.find(viewId) != s_ignoreNextEscape.end())
    {
        s_ignoreNextEscape[viewId] = true;
    }

    if (s_keepIgnoringEscape.find(viewId) != s_keepIgnoringEscape.end())
    {
        s_keepIgnoringEscape[viewId] = !onlyOnce;
    }
}

void KeyboardShortcutManager::HonorEscape()
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    int viewId = Utils::GetWindowId();

    if (s_ignoreNextEscape.find(viewId) != s_ignoreNextEscape.end())
    {
        s_ignoreNextEscape[viewId] = false;
    }

    if (s_keepIgnoringEscape.find(viewId) != s_keepIgnoringEscape.end())
    {
        s_keepIgnoringEscape[viewId] = false;
    }
}

void KeyboardShortcutManager::OnCharacterPropertyChanged(DependencyObject ^ target, String ^ oldValue, String ^ newValue)
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    auto button = safe_cast<ButtonBase ^>(target);

    int viewId = Utils::GetWindowId();
    auto iterViewMap = s_CharacterForButtons.find(viewId);

    if (iterViewMap != s_CharacterForButtons.end())
    {
        if (oldValue)
        {
            iterViewMap->second.erase(oldValue->Data()[0]);
        }

        if (newValue)
        {
            if (newValue == L".")
            {
                wchar_t decSep = LocalizationSettings::GetInstance().GetDecimalSeparator();
                iterViewMap->second.insert(std::make_pair(decSep, WeakReference(button)));
            }
            else
            {
                iterViewMap->second.insert(std::make_pair(newValue->Data()[0], WeakReference(button)));
            }
        }
    }
    else
    {
        s_CharacterForButtons.insert(std::make_pair(viewId, std::multimap<wchar_t, WeakReference>()));

        if (newValue == L".")
        {
            wchar_t decSep = LocalizationSettings::GetInstance().GetDecimalSeparator();
            s_CharacterForButtons.find(viewId)->second.insert(std::make_pair(decSep, WeakReference(button)));
        }
        else
        {
            s_CharacterForButtons.find(viewId)->second.insert(std::make_pair(newValue->Data()[0], WeakReference(button)));
        }
    }
}

void KeyboardShortcutManager::OnVirtualKeyPropertyChanged(DependencyObject ^ target, MyVirtualKey /*oldValue*/, MyVirtualKey newValue)
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    auto button = static_cast<ButtonBase ^>(target);

    int viewId = Utils::GetWindowId();
    auto iterViewMap = s_VirtualKeysForButtons.find(viewId);

    // Check if the View Id has already been registered
    if (iterViewMap != s_VirtualKeysForButtons.end())
    {
        iterViewMap->second.insert(std::make_pair(newValue, WeakReference(button)));
    }
    else
    {
        // If the View Id is not already registered, then register it and make the entry
        s_VirtualKeysForButtons.insert(std::make_pair(viewId, std::multimap<MyVirtualKey, WeakReference>()));
        s_VirtualKeysForButtons.find(viewId)->second.insert(std::make_pair(newValue, WeakReference(button)));
    }
}

void KeyboardShortcutManager::OnVirtualKeyControlChordPropertyChanged(DependencyObject ^ target, MyVirtualKey /*oldValue*/, MyVirtualKey newValue)
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    Control ^ control = dynamic_cast<ButtonBase ^>(target);

    if (control == nullptr)
    {
        // Handling Ctrl+E shortcut for Date Calc, target would be NavigationView^ in that case
        control = safe_cast<MUXC::NavigationView ^>(target);
    }

    int viewId = Utils::GetWindowId();
    auto iterViewMap = s_VirtualKeyControlChordsForButtons.find(viewId);

    // Check if the View Id has already been registered
    if (iterViewMap != s_VirtualKeyControlChordsForButtons.end())
    {
        iterViewMap->second.insert(std::make_pair(newValue, WeakReference(control)));
    }
    else
    {
        // If the View Id is not already registered, then register it and make the entry
        s_VirtualKeyControlChordsForButtons.insert(std::make_pair(viewId, std::multimap<MyVirtualKey, WeakReference>()));
        s_VirtualKeyControlChordsForButtons.find(viewId)->second.insert(std::make_pair(newValue, WeakReference(control)));
    }
}

void KeyboardShortcutManager::OnVirtualKeyShiftChordPropertyChanged(DependencyObject ^ target, MyVirtualKey /*oldValue*/, MyVirtualKey newValue)
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    auto button = safe_cast<ButtonBase ^>(target);

    int viewId = Utils::GetWindowId();
    auto iterViewMap = s_VirtualKeyShiftChordsForButtons.find(viewId);

    // Check if the View Id has already been registered
    if (iterViewMap != s_VirtualKeyShiftChordsForButtons.end())
    {
        iterViewMap->second.insert(std::make_pair(newValue, WeakReference(button)));
    }
    else
    {
        // If the View Id is not already registered, then register it and make the entry
        s_VirtualKeyShiftChordsForButtons.insert(std::make_pair(viewId, std::multimap<MyVirtualKey, WeakReference>()));
        s_VirtualKeyShiftChordsForButtons.find(viewId)->second.insert(std::make_pair(newValue, WeakReference(button)));
    }
}

void KeyboardShortcutManager::OnVirtualKeyAltChordPropertyChanged(DependencyObject ^ target, MyVirtualKey /*oldValue*/, MyVirtualKey newValue)
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    MUXC::NavigationView ^ navView = safe_cast<MUXC::NavigationView ^>(target);

    int viewId = Utils::GetWindowId();
    auto iterViewMap = s_VirtualKeyAltChordsForButtons.find(viewId);

    // Check if the View Id has already been registered
    if (iterViewMap != s_VirtualKeyAltChordsForButtons.end())
    {
        iterViewMap->second.insert(std::make_pair(newValue, WeakReference(navView)));
    }
    else
    {
        // If the View Id is not already registered, then register it and make the entry
        s_VirtualKeyAltChordsForButtons.insert(std::make_pair(viewId, std::multimap<MyVirtualKey, WeakReference>()));
        s_VirtualKeyAltChordsForButtons.find(viewId)->second.insert(std::make_pair(newValue, WeakReference(navView)));
    }
}

void KeyboardShortcutManager::OnVirtualKeyControlShiftChordPropertyChanged(DependencyObject ^ target, MyVirtualKey /*oldValue*/, MyVirtualKey newValue)
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    auto button = safe_cast<ButtonBase ^>(target);

    int viewId = Utils::GetWindowId();
    auto iterViewMap = s_VirtualKeyControlShiftChordsForButtons.find(viewId);

    // Check if the View Id has already been registered
    if (iterViewMap != s_VirtualKeyControlShiftChordsForButtons.end())
    {
        iterViewMap->second.insert(std::make_pair(newValue, WeakReference(button)));
    }
    else
    {
        // If the View Id is not already registered, then register it and make the entry
        s_VirtualKeyControlShiftChordsForButtons.insert(std::make_pair(viewId, std::multimap<MyVirtualKey, WeakReference>()));
        s_VirtualKeyControlShiftChordsForButtons.find(viewId)->second.insert(std::make_pair(newValue, WeakReference(button)));
    }
}

// In the three event handlers below we will not mark the event as handled
// because this is a supplemental operation and we don't want to interfere with
// the normal keyboard handling.
void KeyboardShortcutManager::OnCharacterReceivedHandler(CoreWindow ^ sender, CharacterReceivedEventArgs ^ args)
{
    int viewId = Utils::GetWindowId();
    auto currentHonorShortcuts = s_fHonorShortcuts.find(viewId);

    if (currentHonorShortcuts == s_fHonorShortcuts.end() || currentHonorShortcuts->second)
    {
        wchar_t character = static_cast<wchar_t>(args->KeyCode);
        auto buttons = s_CharacterForButtons.find(viewId)->second.equal_range(character);
        RunFirstEnabledButtonCommand(buttons);

        LightUpButtons(buttons);
    }
}

const std::multimap<MyVirtualKey, WeakReference>* GetCurrentKeyDictionary(bool altPressed = false)
{
    int viewId = Utils::GetWindowId();

    bool isControlKeyPressed = (Window::Current->CoreWindow->GetKeyState(VirtualKey::Control) & CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down;
    bool isShiftKeyPressed = (Window::Current->CoreWindow->GetKeyState(VirtualKey::Shift) & CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down;

    if (isControlKeyPressed)
    {
        if (altPressed)
        {
            return nullptr;
        }
        else
        {
            if (isShiftKeyPressed)
            {
                return &s_VirtualKeyControlShiftChordsForButtons.find(viewId)->second;
            }
            else
            {
                return &s_VirtualKeyControlChordsForButtons.find(viewId)->second;
            }
        }
    }
    else
    {
        if (altPressed)
        {
            if (!isShiftKeyPressed)
            {
                return &s_VirtualKeyAltChordsForButtons.find(viewId)->second;
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            if (isShiftKeyPressed)
            {
                return &s_VirtualKeyShiftChordsForButtons.find(viewId)->second;
            }
            else
            {
                return &s_VirtualKeysForButtons.find(viewId)->second;
            }
        }
    }
}

void KeyboardShortcutManager::OnKeyDownHandler(CoreWindow ^ sender, KeyEventArgs ^ args)
{
    if (args->Handled)
    {
        return;
    }

    auto key = args->VirtualKey;
    int viewId = Utils::GetWindowId();

    bool isControlKeyPressed = (Window::Current->CoreWindow->GetKeyState(VirtualKey::Control) & CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down;
    bool isShiftKeyPressed = (Window::Current->CoreWindow->GetKeyState(VirtualKey::Shift) & CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down;
    bool isAltKeyPressed = (Window::Current->CoreWindow->GetKeyState(VirtualKey::Menu) & CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down;

    // Handle Ctrl + E for DateCalculator
    if ((key == VirtualKey::E) && isControlKeyPressed && !isShiftKeyPressed && !isAltKeyPressed)
    {
        const auto lookupMap = GetCurrentKeyDictionary(false);
        if (lookupMap == nullptr)
        {
            return;
        }

        auto buttons = lookupMap->equal_range(static_cast<MyVirtualKey>(key));
        auto navView = buttons.first->second.Resolve<MUXC::NavigationView>();
        auto appViewModel = safe_cast<ApplicationViewModel ^>(navView->DataContext);
        appViewModel->Mode = ViewMode::Date;
        auto categoryName = AppResourceProvider::GetInstance()->GetResourceString(L"DateCalculationModeText");
        appViewModel->CategoryName = categoryName;

        auto menuItems = static_cast<IObservableVector<Object ^> ^>(navView->MenuItemsSource);
        auto flatIndex = NavCategory::GetFlatIndex(ViewMode::Date);
        navView->SelectedItem = menuItems->GetAt(flatIndex);
        return;
    }

    auto currentIgnoreNextEscape = s_ignoreNextEscape.find(viewId);
    if (currentIgnoreNextEscape != s_ignoreNextEscape.end())
    {
        if (currentIgnoreNextEscape->second && key == VirtualKey::Escape)
        {
            auto currentKeepIgnoringEscape = s_keepIgnoringEscape.find(viewId);

            if (currentKeepIgnoringEscape != s_keepIgnoringEscape.end())
            {
                if (!currentKeepIgnoringEscape->second)
                {
                    HonorEscape();
                }
                return;
            }
        }
    }

    auto currentHonorShortcuts = s_fHonorShortcuts.find(viewId);
    if (currentHonorShortcuts != s_fHonorShortcuts.end())
    {
        if (currentHonorShortcuts->second)
        {
            const auto myVirtualKey = static_cast<MyVirtualKey>(key);
            const auto lookupMap = GetCurrentKeyDictionary(isAltKeyPressed);
            if (lookupMap == nullptr)
            {
                return;
            }

            auto buttons = lookupMap->equal_range(myVirtualKey);
            if (buttons.first == buttons.second)
            {
                return;
            }

            RunFirstEnabledButtonCommand(buttons);

            // Ctrl+C and Ctrl+V shifts focus to some button because of which enter doesn't work after copy/paste. So don't shift focus if Ctrl+C or Ctrl+V
            // is pressed. When drop down is open, pressing escape shifts focus to clear button. So dont's shift focus if drop down is open. Ctrl+Insert is
            // equivalent to Ctrl+C and Shift+Insert is equivalent to Ctrl+V
            auto currentIsDropDownOpen = s_IsDropDownOpen.find(viewId);
            if (currentIsDropDownOpen == s_IsDropDownOpen.end() || !currentIsDropDownOpen->second)
            {
                // Do not Light Up Buttons when Ctrl+C, Ctrl+V, Ctrl+Insert or Shift+Insert is pressed
                if (!(isControlKeyPressed && (key == VirtualKey::C || key == VirtualKey::V || key == VirtualKey::Insert))
                    && !(isShiftKeyPressed && (key == VirtualKey::Insert)))
                {
                    LightUpButtons(buttons);
                }
            }
        }
    }
}

void KeyboardShortcutManager::OnAcceleratorKeyActivated(CoreDispatcher ^, AcceleratorKeyEventArgs ^ args)
{
    if (args->KeyStatus.IsKeyReleased)
    {
        auto key = args->VirtualKey;
        bool altPressed = args->KeyStatus.IsMenuKeyDown;

        // If the Alt/Menu key is not pressed then we don't care about the key anymore
        if (!altPressed)
        {
            return;
        }

        // Ctrl is pressed in addition to alt, this means Alt Gr is intended.  do not navigate.
        if ((static_cast<short>(Window::Current->CoreWindow->GetKeyState(VirtualKey::Control)) & static_cast<short>(CoreVirtualKeyStates::Down))
            == static_cast<short>(CoreVirtualKeyStates::Down))
        {
            return;
        }

        const auto lookupMap = GetCurrentKeyDictionary(altPressed);
        if (lookupMap != nullptr)
        {
            auto listItems = lookupMap->equal_range(static_cast<MyVirtualKey>(key));
            for (auto listIterator = listItems.first; listIterator != listItems.second; ++listIterator)
            {
                auto item = listIterator->second.Resolve<MUXC::NavigationView>();
                if (item != nullptr)
                {
                    auto navView = safe_cast<MUXC::NavigationView ^>(item);

                    auto menuItems = static_cast<IObservableVector<Object ^> ^>(navView->MenuItemsSource);
                    if (menuItems != nullptr)
                    {
                        auto vm = safe_cast<ApplicationViewModel ^>(navView->DataContext);
                        if (nullptr != vm)
                        {
                            ViewMode toMode = NavCategory::GetViewModeForVirtualKey(static_cast<MyVirtualKey>(key));
                            auto nvi = dynamic_cast<MUXC::NavigationViewItem ^>(menuItems->GetAt(NavCategory::GetFlatIndex(toMode)));
                            if (nvi && nvi->IsEnabled && NavCategory::IsValidViewMode(toMode))
                            {
                                vm->Mode = toMode;
                                navView->SelectedItem = nvi;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}

void KeyboardShortcutManager::Initialize()
{
    auto coreWindow = Window::Current->CoreWindow;
    coreWindow->CharacterReceived +=
        ref new TypedEventHandler<CoreWindow ^, CharacterReceivedEventArgs ^>(&KeyboardShortcutManager::OnCharacterReceivedHandler);
    coreWindow->KeyDown += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(&KeyboardShortcutManager::OnKeyDownHandler);
    coreWindow->Dispatcher->AcceleratorKeyActivated +=
        ref new TypedEventHandler<CoreDispatcher ^, AcceleratorKeyEventArgs ^>(&KeyboardShortcutManager::OnAcceleratorKeyActivated);

    KeyboardShortcutManager::RegisterNewAppViewId();
}

void KeyboardShortcutManager::UpdateDropDownState(bool isOpen)
{
    int viewId = Utils::GetWindowId();

    if (s_IsDropDownOpen.find(viewId) != s_IsDropDownOpen.end())
    {
        s_IsDropDownOpen[viewId] = isOpen;
    }
}

void KeyboardShortcutManager::HonorShortcuts(bool allow)
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    int viewId = Utils::GetWindowId();

    if (s_fHonorShortcuts.find(viewId) != s_fHonorShortcuts.end())
    {
        if (s_fDisableShortcuts.find(viewId) != s_fDisableShortcuts.end())
        {
            if (s_fDisableShortcuts[viewId])
            {
                s_fHonorShortcuts[viewId] = false;
                return;
            }
        }

        s_fHonorShortcuts[viewId] = allow;
    }
}

void KeyboardShortcutManager::RegisterNewAppViewId()
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    int appViewId = Utils::GetWindowId();

    // Check if the View Id has already been registered
    if (s_CharacterForButtons.find(appViewId) == s_CharacterForButtons.end())
    {
        s_CharacterForButtons.insert(std::make_pair(appViewId, std::multimap<wchar_t, WeakReference>()));
    }

    if (s_VirtualKeysForButtons.find(appViewId) == s_VirtualKeysForButtons.end())
    {
        s_VirtualKeysForButtons.insert(std::make_pair(appViewId, std::multimap<MyVirtualKey, WeakReference>()));
    }

    if (s_VirtualKeyControlChordsForButtons.find(appViewId) == s_VirtualKeyControlChordsForButtons.end())
    {
        s_VirtualKeyControlChordsForButtons.insert(std::make_pair(appViewId, std::multimap<MyVirtualKey, WeakReference>()));
    }

    if (s_VirtualKeyShiftChordsForButtons.find(appViewId) == s_VirtualKeyShiftChordsForButtons.end())
    {
        s_VirtualKeyShiftChordsForButtons.insert(std::make_pair(appViewId, std::multimap<MyVirtualKey, WeakReference>()));
    }

    if (s_VirtualKeyAltChordsForButtons.find(appViewId) == s_VirtualKeyAltChordsForButtons.end())
    {
        s_VirtualKeyAltChordsForButtons.insert(std::make_pair(appViewId, std::multimap<MyVirtualKey, WeakReference>()));
    }

    if (s_VirtualKeyControlShiftChordsForButtons.find(appViewId) == s_VirtualKeyControlShiftChordsForButtons.end())
    {
        s_VirtualKeyControlShiftChordsForButtons.insert(std::make_pair(appViewId, std::multimap<MyVirtualKey, WeakReference>()));
    }

    s_IsDropDownOpen[appViewId] = false;
    s_ignoreNextEscape[appViewId] = false;
    s_keepIgnoringEscape[appViewId] = false;
    s_fHonorShortcuts[appViewId] = true;
    s_fDisableShortcuts[appViewId] = false;
}

void KeyboardShortcutManager::OnWindowClosed(int viewId)
{
    // Writer lock for the static maps
    reader_writer_lock::scoped_lock lock(s_keyboardShortcutMapLock);

    s_CharacterForButtons.erase(viewId);

    s_VirtualKeysForButtons.erase(viewId);
    s_VirtualKeyControlChordsForButtons.erase(viewId);
    s_VirtualKeyShiftChordsForButtons.erase(viewId);
    s_VirtualKeyAltChordsForButtons.erase(viewId);
    s_VirtualKeyControlShiftChordsForButtons.erase(viewId);

    s_IsDropDownOpen.erase(viewId);
    s_ignoreNextEscape.erase(viewId);
    s_keepIgnoringEscape.erase(viewId);
    s_fHonorShortcuts.erase(viewId);
    s_fDisableShortcuts.erase(viewId);
}

void KeyboardShortcutManager::DisableShortcuts(bool disable)
{
    int viewId = Utils::GetWindowId();

    if (s_fDisableShortcuts.find(viewId) != s_fDisableShortcuts.end())
    {
        s_fDisableShortcuts[viewId] = disable;
    }

    HonorShortcuts(!disable);
}
