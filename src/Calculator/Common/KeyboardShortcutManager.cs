// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;

using MUXC = Microsoft.UI.Xaml.Controls;

namespace CalculatorApp
{
    namespace Common
    {
        internal static class KeyboardShortcutManagerLocals
        {
            // Lights up all of the buttons in the given range
            // The range is defined by a pair of iterators
            public static void LightUpButtons(IEnumerable<WeakReference> buttons)
            {
                foreach (var button in buttons)
                {
                    if (button.Target is ButtonBase btn && btn.IsEnabled)
                    {
                        LightUpButton(btn);
                    }
                }
            }

            public static void LightUpButton(ButtonBase button)
            {
                // If the button is a toggle button then we don't need
                // to change the UI of the button
                if (button is ToggleButton)
                {
                    return;
                }

                // The button will go into the visual Pressed state with this call
                VisualStateManager.GoToState(button, "Pressed", true);

                // This timer will fire after lightUpTime and make the button
                // go back to the normal state.
                // This timer will only fire once after which it will be destroyed
                var timer = new DispatcherTimer();
                TimeSpan lightUpTime = TimeSpan.FromMilliseconds(50); // 5e5 100-ns
                timer.Interval = lightUpTime;

                var timerWeakReference = new WeakReference(timer);
                var buttonWeakReference = new WeakReference(button);
                timer.Tick += (sender, args) =>
                {
                    if (buttonWeakReference.Target is ButtonBase btn)
                    {
                        VisualStateManager.GoToState(button, "Normal", true);
                    }

                    if (timerWeakReference.Target is DispatcherTimer tmr)
                    {
                        tmr.Stop();
                    }
                };
                timer.Start();
            }

            // Looks for the first button reference that it can resolve
            // and execute its command.
            // NOTE: It is assumed that all buttons associated with a particular
            // key have the same command
            public static void RunFirstEnabledButtonCommand(IEnumerable<WeakReference> buttons)
            {
                foreach (var button in buttons)
                {
                    if (button.Target is ButtonBase btn && btn.IsEnabled)
                    {
                        RunButtonCommand(btn);
                        break;
                    }
                }
            }

            public static void RunButtonCommand(ButtonBase button)
            {
                if (button.IsEnabled)
                {
                    var command = button.Command;
                    var parameter = button.CommandParameter;
                    if (command != null && command.CanExecute(parameter))
                    {
                        command.Execute(parameter);
                    }

                    if (button is RadioButton radio)
                    {
                        radio.IsChecked = true;
                        return;
                    }

                    if (button is ToggleButton toggle)
                    {
                        toggle.IsChecked = !(toggle.IsChecked != null && toggle.IsChecked.Value);
                        return;
                    }
                }
            }
        }

        public sealed class KeyboardShortcutManager : DependencyObject
        {
            public KeyboardShortcutManager()
            {
            }

            public static readonly DependencyProperty CharacterProperty =
                DependencyProperty.RegisterAttached(
                    "Character",
                    typeof(string),
                    typeof(KeyboardShortcutManager),
                    new PropertyMetadata(string.Empty, (sender, args) =>
                    {
                        OnCharacterPropertyChanged(sender, (string)args.OldValue, (string)args.NewValue);
                    }));

            public static string GetCharacter(DependencyObject target)
            {
                return (string)target.GetValue(CharacterProperty);
            }

            public static void SetCharacter(DependencyObject target, string value)
            {
                target.SetValue(CharacterProperty, value);
            }

            public static readonly DependencyProperty VirtualKeyProperty =
                DependencyProperty.RegisterAttached(
                    "VirtualKey",
                    typeof(MyVirtualKey),
                    typeof(KeyboardShortcutManager),
                    new PropertyMetadata(default(MyVirtualKey), (sender, args) =>
                    {
                        OnVirtualKeyPropertyChanged(sender, (MyVirtualKey)args.OldValue, (MyVirtualKey)args.NewValue);
                    }));

            public static MyVirtualKey GetVirtualKey(DependencyObject target)
            {
                return (MyVirtualKey)target.GetValue(VirtualKeyProperty);
            }

            public static void SetVirtualKey(DependencyObject target, MyVirtualKey value)
            {
                target.SetValue(VirtualKeyProperty, value);
            }

            public static readonly DependencyProperty VirtualKeyControlChordProperty =
                DependencyProperty.RegisterAttached(
                    "VirtualKeyControlChord",
                    typeof(MyVirtualKey),
                    typeof(KeyboardShortcutManager),
                    new PropertyMetadata(default(MyVirtualKey), (sender, args) =>
                    {
                        OnVirtualKeyControlChordPropertyChanged(sender, (MyVirtualKey)args.OldValue, (MyVirtualKey)args.NewValue);
                    }));

            public static MyVirtualKey GetVirtualKeyControlChord(DependencyObject target)
            {
                return (MyVirtualKey)target.GetValue(VirtualKeyControlChordProperty);
            }

            public static void SetVirtualKeyControlChord(DependencyObject target, MyVirtualKey value)
            {
                target.SetValue(VirtualKeyControlChordProperty, value);
            }

            public static readonly DependencyProperty VirtualKeyShiftChordProperty =
                DependencyProperty.RegisterAttached(
                    "VirtualKeyShiftChord",
                    typeof(MyVirtualKey),
                    typeof(KeyboardShortcutManager),
                    new PropertyMetadata(default(MyVirtualKey), (sender, args) =>
                    {
                        OnVirtualKeyShiftChordPropertyChanged(sender, (MyVirtualKey)args.OldValue, (MyVirtualKey)args.NewValue);
                    }));

            public static MyVirtualKey GetVirtualKeyShiftChord(DependencyObject target)
            {
                return (MyVirtualKey)target.GetValue(VirtualKeyShiftChordProperty);
            }

            public static void SetVirtualKeyShiftChord(DependencyObject target, MyVirtualKey value)
            {
                target.SetValue(VirtualKeyShiftChordProperty, value);
            }

            public static readonly DependencyProperty VirtualKeyAltChordProperty =
                DependencyProperty.RegisterAttached(
                    "VirtualKeyAltChord",
                    typeof(MyVirtualKey),
                    typeof(KeyboardShortcutManager),
                    new PropertyMetadata(default(MyVirtualKey), (sender, args) =>
                    {
                        OnVirtualKeyAltChordPropertyChanged(sender, (MyVirtualKey)args.OldValue, (MyVirtualKey)args.NewValue);
                    }));

            public static MyVirtualKey GetVirtualKeyAltChord(DependencyObject target)
            {
                return (MyVirtualKey)target.GetValue(VirtualKeyAltChordProperty);
            }

            public static void SetVirtualKeyAltChord(DependencyObject target, MyVirtualKey value)
            {
                target.SetValue(VirtualKeyAltChordProperty, value);
            }

            public static readonly DependencyProperty VirtualKeyControlShiftChordProperty =
                DependencyProperty.RegisterAttached(
                    "VirtualKeyControlShiftChord",
                    typeof(MyVirtualKey),
                    typeof(KeyboardShortcutManager),
                    new PropertyMetadata(default(MyVirtualKey), (sender, args) =>
                    {
                        OnVirtualKeyControlShiftChordPropertyChanged(sender, (MyVirtualKey)args.OldValue, (MyVirtualKey)args.NewValue);
                    }));

            public static MyVirtualKey GetVirtualKeyControlShiftChord(DependencyObject target)
            {
                return (MyVirtualKey)target.GetValue(VirtualKeyControlShiftChordProperty);
            }

            public static void SetVirtualKeyControlShiftChord(DependencyObject target, MyVirtualKey value)
            {
                target.SetValue(VirtualKeyControlShiftChordProperty, value);
            }

            internal static void Initialize()
            {
                var coreWindow = Window.Current.CoreWindow;
                coreWindow.CharacterReceived += OnCharacterReceivedHandler;
                coreWindow.KeyDown += OnKeyDownHandler;
                coreWindow.KeyUp += OnKeyUpHandler;
                coreWindow.Dispatcher.AcceleratorKeyActivated += OnAcceleratorKeyActivated;
                KeyboardShortcutManager.RegisterNewAppViewId();
            }

            // Sometimes, like with popups, escape is treated as special and even
            // though it is handled we get it passed through to us. In those cases
            // we need to be able to ignore it (looking at e->Handled isn't sufficient
            // because that always returns true).
            // The onlyOnce flag is used to indicate whether we should only ignore the
            // next escape, or keep ignoring until you explicitly HonorEscape.
            public static void IgnoreEscape(bool onlyOnce)
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    int viewId = Utilities.GetWindowId();

                    if (s_ignoreNextEscape.ContainsKey(viewId))
                    {
                        s_ignoreNextEscape[viewId] = true;
                    }

                    if (s_keepIgnoringEscape.ContainsKey(viewId))
                    {
                        s_keepIgnoringEscape[viewId] = !onlyOnce;
                    }
                }
            }

            public static void HonorEscape()
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    int viewId = Utilities.GetWindowId();

                    if (s_ignoreNextEscape.ContainsKey(viewId))
                    {
                        s_ignoreNextEscape[viewId] = false;
                    }

                    if (s_keepIgnoringEscape.ContainsKey(viewId))
                    {
                        s_keepIgnoringEscape[viewId] = false;
                    }
                }
            }

            public static void HonorShortcuts(bool allow)
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    int viewId = Utilities.GetWindowId();

                    if (s_fHonorShortcuts.ContainsKey(viewId))
                    {
                        if (s_fDisableShortcuts.ContainsKey(viewId))
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
            }

            public static void DisableShortcuts(bool disable)
            {
                //deferredEnableShortcut is being used to prevent the mode change from happening before the user input has processed 
                if (s_keyHandlerCount > 0 && !disable)
                {
                    s_deferredEnableShortcut = true;
                }
                else
                {
                    int viewId = Utilities.GetWindowId();
                    if (s_fDisableShortcuts.ContainsKey(viewId))
                    {
                        s_fDisableShortcuts[viewId] = disable;
                    }
                    HonorShortcuts(!disable);
                }
            }

            public static void UpdateDropDownState(bool isOpen)
            {
                int viewId = Utilities.GetWindowId();

                if (s_IsDropDownOpen.ContainsKey(viewId))
                {
                    s_IsDropDownOpen[viewId] = isOpen;
                }
            }

            public static void RegisterNewAppViewId()
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    int appViewId = Utilities.GetWindowId();

                    // Check if the View Id has already been registered
                    if (!s_characterForButtons.ContainsKey(appViewId))
                    {
                        s_characterForButtons.Add(appViewId, new SortedDictionary<char, List<WeakReference>>());
                    }

                    if (!s_virtualKey.ContainsKey(appViewId))
                    {
                        s_virtualKey.Add(appViewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                    }

                    if (!s_VirtualKeyControlChordsForButtons.ContainsKey(appViewId))
                    {
                        s_VirtualKeyControlChordsForButtons.Add(appViewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                    }

                    if (!s_VirtualKeyShiftChordsForButtons.ContainsKey(appViewId))
                    {
                        s_VirtualKeyShiftChordsForButtons.Add(appViewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                    }

                    if (!s_VirtualKeyAltChordsForButtons.ContainsKey(appViewId))
                    {
                        s_VirtualKeyAltChordsForButtons.Add(appViewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                    }

                    if (!s_VirtualKeyControlShiftChordsForButtons.ContainsKey(appViewId))
                    {
                        s_VirtualKeyControlShiftChordsForButtons.Add(appViewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                    }

                    s_IsDropDownOpen[appViewId] = false;
                    s_ignoreNextEscape[appViewId] = false;
                    s_keepIgnoringEscape[appViewId] = false;
                    s_fHonorShortcuts[appViewId] = true;
                    s_fDisableShortcuts[appViewId] = false;
                }
            }

            public static void OnWindowClosed(int viewId)
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    s_characterForButtons.Remove(viewId);

                    s_virtualKey.Remove(viewId);
                    s_VirtualKeyControlChordsForButtons.Remove(viewId);
                    s_VirtualKeyShiftChordsForButtons.Remove(viewId);
                    s_VirtualKeyAltChordsForButtons.Remove(viewId);
                    s_VirtualKeyControlShiftChordsForButtons.Remove(viewId);

                    s_IsDropDownOpen.Remove(viewId);
                    s_ignoreNextEscape.Remove(viewId);
                    s_keepIgnoringEscape.Remove(viewId);
                    s_fHonorShortcuts.Remove(viewId);
                    s_fDisableShortcuts.Remove(viewId);
                }
            }

            private static void OnCharacterPropertyChanged(DependencyObject target, string oldValue, string newValue)
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    var button = (target as ButtonBase);

                    int viewId = Utilities.GetWindowId();
                    if (s_characterForButtons.TryGetValue(viewId, out var iterViewMap))
                    {
                        if (!string.IsNullOrEmpty(oldValue))
                        {
                            iterViewMap.Remove(oldValue[0]);
                        }

                        if (!string.IsNullOrEmpty(newValue))
                        {
                            if (newValue == ".")
                            {
                                char decSep = LocalizationSettings.GetInstance().GetDecimalSeparator();
                                Insert(iterViewMap, decSep, new WeakReference(button));
                            }
                            else
                            {
                                Insert(iterViewMap, newValue[0], new WeakReference(button));
                            }
                        }
                    }
                    else
                    {
                        s_characterForButtons.Add(viewId, new SortedDictionary<char, List<WeakReference>>());

                        if (newValue == ".")
                        {
                            char decSep = LocalizationSettings.GetInstance().GetDecimalSeparator();
                            Insert(s_characterForButtons[viewId], decSep, new WeakReference(button));
                        }
                        else
                        {
                            Insert(s_characterForButtons[viewId], newValue[0], new WeakReference(button));
                        }
                    }
                }
            }

            private static void OnVirtualKeyPropertyChanged(DependencyObject target, MyVirtualKey oldValue, MyVirtualKey newValue)
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    var button = ((ButtonBase)target);

                    int viewId = Utilities.GetWindowId();

                    // Check if the View Id has already been registered
                    if (s_virtualKey.TryGetValue(viewId, out var iterViewMap))
                    {
                        Insert(iterViewMap, newValue, new WeakReference(button));
                    }
                    else
                    {
                        // If the View Id is not already registered, then register it and make the entry
                        s_virtualKey.Add(viewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                        Insert(s_virtualKey[viewId], newValue, new WeakReference(button));
                    }
                }
            }

            private static void OnVirtualKeyControlChordPropertyChanged(DependencyObject target, MyVirtualKey oldValue, MyVirtualKey newValue)
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    Control control = (target as ButtonBase);

                    if (control == null)
                    {
                        // Handling Ctrl+E shortcut for Date Calc, target would be NavigationView^ in that case
                        control = (target as MUXC.NavigationView);
                    }

                    int viewId = Utilities.GetWindowId();

                    // Check if the View Id has already been registered
                    if (s_VirtualKeyControlChordsForButtons.TryGetValue(viewId, out var iterViewMap))
                    {
                        Insert(iterViewMap, newValue, new WeakReference(control));
                    }
                    else
                    {
                        // If the View Id is not already registered, then register it and make the entry
                        s_VirtualKeyControlChordsForButtons.Add(viewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                        Insert(s_VirtualKeyControlChordsForButtons[viewId], newValue, new WeakReference(control));
                    }
                }
            }

            private static void OnVirtualKeyShiftChordPropertyChanged(DependencyObject target, MyVirtualKey oldValue, MyVirtualKey newValue)
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    var button = (target as ButtonBase);

                    int viewId = Utilities.GetWindowId();

                    // Check if the View Id has already been registered
                    if (s_VirtualKeyShiftChordsForButtons.TryGetValue(viewId, out var iterViewMap))
                    {
                        Insert(iterViewMap, newValue, new WeakReference(button));
                    }
                    else
                    {
                        // If the View Id is not already registered, then register it and make the entry
                        s_VirtualKeyShiftChordsForButtons.Add(viewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                        Insert(s_VirtualKeyShiftChordsForButtons[viewId], newValue, new WeakReference(button));
                    }
                }
            }

            private static void OnVirtualKeyAltChordPropertyChanged(DependencyObject target, MyVirtualKey oldValue, MyVirtualKey newValue)
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    MUXC.NavigationView navView = (target as MUXC.NavigationView);

                    int viewId = Utilities.GetWindowId();

                    // Check if the View Id has already been registered
                    if (s_VirtualKeyAltChordsForButtons.TryGetValue(viewId, out var iterViewMap))
                    {
                        Insert(iterViewMap, newValue, new WeakReference(navView));
                    }
                    else
                    {
                        // If the View Id is not already registered, then register it and make the entry
                        s_VirtualKeyAltChordsForButtons.Add(viewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                        Insert(s_VirtualKeyAltChordsForButtons[viewId], newValue, new WeakReference(navView));
                    }
                }
            }

            private static void OnVirtualKeyControlShiftChordPropertyChanged(DependencyObject target, MyVirtualKey oldValue, MyVirtualKey newValue)
            {
                // Writer lock for the static maps
                lock (s_keyboardShortcutMapLockMutex)
                {
                    var button = (target as ButtonBase);

                    int viewId = Utilities.GetWindowId();

                    // Check if the View Id has already been registered
                    if (s_VirtualKeyControlShiftChordsForButtons.TryGetValue(viewId, out var iterViewMap))
                    {
                        Insert(iterViewMap, newValue, new WeakReference(button));
                    }
                    else
                    {
                        // If the View Id is not already registered, then register it and make the entry
                        s_VirtualKeyControlShiftChordsForButtons.Add(viewId, new SortedDictionary<MyVirtualKey, List<WeakReference>>());
                        Insert(s_VirtualKeyControlShiftChordsForButtons[viewId], newValue, new WeakReference(button));
                    }
                }
            }

            private static bool CanNavigateModeByShortcut(MUXC.NavigationView navView, object nvi
                , ApplicationViewModel vm, ViewMode toMode)
            {
                if (nvi != null && nvi is NavCategory navCategory)
                {
                    return navCategory.IsEnabled
                        && navView.Visibility == Visibility.Visible
                        && !vm.IsAlwaysOnTop
                        && NavCategoryStates.IsValidViewMode(toMode);
                }
                return false;
            }

            private static void NavigateModeByShortcut(bool controlKeyPressed, bool shiftKeyPressed, bool altPressed
                , Windows.System.VirtualKey key, ViewMode? toMode)
            {
                var lookupMap = GetCurrentKeyDictionary(controlKeyPressed, shiftKeyPressed, altPressed);
                if (lookupMap != null)
                {
                    var listItems = EqualRange(lookupMap, (MyVirtualKey)key);
                    foreach (var itemRef in listItems)
                    {
                        if (itemRef.Target is MUXC.NavigationView item)
                        {
                            var navView = item;

                            var menuItems = ((List<object>)navView.MenuItemsSource);
                            if (menuItems != null)
                            {
                                var vm = (navView.DataContext as ApplicationViewModel);
                                if (null != vm)
                                {
                                    ViewMode realToMode = toMode.HasValue ? toMode.Value : NavCategoryStates.GetViewModeForVirtualKey(((MyVirtualKey)key));

                                    var nvi = menuItems[NavCategoryStates.GetFlatIndex(realToMode)];
                                    if (CanNavigateModeByShortcut(navView, nvi, vm, realToMode))
                                    {
                                        vm.Mode = realToMode;
                                        navView.SelectedItem = nvi;
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
            }

            // In the three event handlers below we will not mark the event as handled
            // because this is a supplemental operation and we don't want to interfere with
            // the normal keyboard handling.
            private static void OnCharacterReceivedHandler(CoreWindow sender, CharacterReceivedEventArgs args)
            {
                int viewId = Utilities.GetWindowId();
                bool hit = s_fHonorShortcuts.TryGetValue(viewId, out var currentHonorShortcuts);

                if (!hit || currentHonorShortcuts)
                {
                    char character = ((char)args.KeyCode);
                    var buttons = EqualRange(s_characterForButtons[viewId], character);
                    KeyboardShortcutManagerLocals.RunFirstEnabledButtonCommand(buttons);

                    KeyboardShortcutManagerLocals.LightUpButtons(buttons);
                }
            }

            private static void OnKeyDownHandler(CoreWindow sender, KeyEventArgs args)
            {
                s_keyHandlerCount++;
                
                if (args.Handled)
                {
                    return;
                }

                var key = args.VirtualKey;
                int viewId = Utilities.GetWindowId();

                bool isControlKeyPressed = (Window.Current.CoreWindow.GetKeyState(Windows.System.VirtualKey.Control) & CoreVirtualKeyStates.Down) == CoreVirtualKeyStates.Down;
                bool isShiftKeyPressed = (Window.Current.CoreWindow.GetKeyState(Windows.System.VirtualKey.Shift) & CoreVirtualKeyStates.Down) == CoreVirtualKeyStates.Down;
                bool isAltKeyPressed = (Window.Current.CoreWindow.GetKeyState(Windows.System.VirtualKey.Menu) & CoreVirtualKeyStates.Down) == CoreVirtualKeyStates.Down;

                // Handle Ctrl + E for DateCalculator
                if ((key == Windows.System.VirtualKey.E) && isControlKeyPressed && !isShiftKeyPressed && !isAltKeyPressed)
                {
                    NavigateModeByShortcut(true, false, false, key, ViewMode.Date);
                    return;
                }

                if (s_ignoreNextEscape.TryGetValue(viewId, out var currentIgnoreNextEscape))
                {
                    if (currentIgnoreNextEscape && key == Windows.System.VirtualKey.Escape)
                    {
                        if (s_keepIgnoringEscape.TryGetValue(viewId, out var currentKeepIgnoringEscape))
                        {
                            if (!currentKeepIgnoringEscape)
                            {
                                HonorEscape();
                            }
                            return;
                        }
                    }
                }

                if (s_fHonorShortcuts.TryGetValue(viewId, out var currentHonorShortcuts))
                {
                    if (currentHonorShortcuts)
                    {
                        var myVirtualKey = key;
                        var lookupMap = GetCurrentKeyDictionary(isControlKeyPressed, isShiftKeyPressed, isAltKeyPressed);
                        if (lookupMap == null)
                        {
                            return;
                        }

                        var buttons = EqualRange(lookupMap, (MyVirtualKey)myVirtualKey);
                        if (!buttons.Any())
                        {
                            return;
                        }

                        KeyboardShortcutManagerLocals.RunFirstEnabledButtonCommand(buttons);

                        // Ctrl+C and Ctrl+V shifts focus to some button because of which enter doesn't work after copy/paste. So don't shift focus if Ctrl+C or Ctrl+V
                        // is pressed. When drop down is open, pressing escape shifts focus to clear button. So dont's shift focus if drop down is open. Ctrl+Insert is
                        // equivalent to Ctrl+C and Shift+Insert is equivalent to Ctrl+V
                        //var currentIsDropDownOpen = s_IsDropDownOpen.find(viewId);
                        if (!s_IsDropDownOpen.TryGetValue(viewId, out var currentIsDropDownOpen) || !currentIsDropDownOpen)
                        {
                            // Do not Light Up Buttons when Ctrl+C, Ctrl+V, Ctrl+Insert or Shift+Insert is pressed
                            if (!(isControlKeyPressed && (key == Windows.System.VirtualKey.C || key == Windows.System.VirtualKey.V || key == Windows.System.VirtualKey.Insert))
                                & !(isShiftKeyPressed && (key == Windows.System.VirtualKey.Insert)))
                            {
                                KeyboardShortcutManagerLocals.LightUpButtons(buttons);
                            }
                        }
                    }
                }
            }

            private static void OnKeyUpHandler(CoreWindow sender, KeyEventArgs args)
            {
                s_keyHandlerCount--;
                if (s_keyHandlerCount == 0 && s_deferredEnableShortcut)
                {
                    DisableShortcuts(false);
                    s_deferredEnableShortcut = false;
                }             
            }
            
            private static void OnAcceleratorKeyActivated(CoreDispatcher dispatcher, AcceleratorKeyEventArgs args)
            {
                if (args.KeyStatus.IsKeyReleased)
                {
                    var key = args.VirtualKey;
                    bool altPressed = args.KeyStatus.IsMenuKeyDown;

                    // If the Alt/Menu key is not pressed then we don't care about the key anymore
                    if (!altPressed)
                    {
                        return;
                    }

                    bool controlKeyPressed = (Window.Current.CoreWindow.GetKeyState(Windows.System.VirtualKey.Control) & CoreVirtualKeyStates.Down) == CoreVirtualKeyStates.Down;
                    // Ctrl is pressed in addition to alt, this means Alt Gr is intended.  do not navigate.
                    if (controlKeyPressed)
                    {
                        return;
                    }

                    bool shiftKeyPressed = (Window.Current.CoreWindow.GetKeyState(Windows.System.VirtualKey.Shift) & CoreVirtualKeyStates.Down) == CoreVirtualKeyStates.Down;
                    NavigateModeByShortcut(false, shiftKeyPressed, true, key, null);
                }
            }

            private static SortedDictionary<MyVirtualKey, List<WeakReference>> GetCurrentKeyDictionary(bool controlKeyPressed, bool shiftKeyPressed, bool altPressed)
            {
                int viewId = Utilities.GetWindowId();

                if (controlKeyPressed)
                {
                    if (altPressed)
                    {
                        return null;
                    }
                    else
                    {
                        if (shiftKeyPressed)
                        {
                            return s_VirtualKeyControlShiftChordsForButtons[viewId];
                        }
                        else
                        {
                            return s_VirtualKeyControlChordsForButtons[viewId];
                        }
                    }
                }
                else
                {
                    if (altPressed)
                    {
                        if (!shiftKeyPressed)
                        {
                            return s_VirtualKeyAltChordsForButtons[viewId];
                        }
                        else
                        {
                            return null;
                        }
                    }
                    else
                    {
                        if (shiftKeyPressed)
                        {
                            return s_VirtualKeyShiftChordsForButtons[viewId];
                        }
                        else
                        {
                            return s_virtualKey[viewId];
                        }
                    }
                }
            }

            // EqualRange is a helper function to pick a range from std::multimap.
            private static IEnumerable<TValue> EqualRange<TKey, TValue>(SortedDictionary<TKey, List<TValue>> source, TKey key)
            {
                Debug.Assert(source != null);
                if (source.TryGetValue(key, out List<TValue> items))
                {
                    return items;
                }
                else
                {
                    return Enumerable.Empty<TValue>();
                }
            }

            // Insert is a helper function to insert a pair into std::multimap.
            private static void Insert<Tkey, TValue>(SortedDictionary<Tkey, List<TValue>> dest, Tkey key, TValue value)
            {
                if (dest.TryGetValue(key, out List<TValue> items))
                {
                    items.Add(value);
                }
                else
                {
                    items = new List<TValue> { value };
                    dest.Add(key, items);
                }
            }

            private static readonly SortedDictionary<int, SortedDictionary<char, List<WeakReference>>> s_characterForButtons = new SortedDictionary<int, SortedDictionary<char, List<WeakReference>>>();
            private static readonly SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>> s_virtualKey = new SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>>();
            private static readonly SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>> s_VirtualKeyControlChordsForButtons = new SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>>();
            private static readonly SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>> s_VirtualKeyShiftChordsForButtons = new SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>>();
            private static readonly SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>> s_VirtualKeyAltChordsForButtons = new SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>>();
            private static readonly SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>> s_VirtualKeyControlShiftChordsForButtons = new SortedDictionary<int, SortedDictionary<MyVirtualKey, List<WeakReference>>>();

            private static readonly SortedDictionary<int, bool> s_IsDropDownOpen = new SortedDictionary<int, bool>();
            private static readonly SortedDictionary<int, bool> s_ignoreNextEscape = new SortedDictionary<int, bool>();
            private static readonly SortedDictionary<int, bool> s_keepIgnoringEscape = new SortedDictionary<int, bool>();
            private static readonly SortedDictionary<int, bool> s_fHonorShortcuts = new SortedDictionary<int, bool>();
            private static readonly SortedDictionary<int, bool> s_fDisableShortcuts = new SortedDictionary<int, bool>();

            //private static Concurrency.reader_writer_lock s_keyboardShortcutMapLock;
            private static readonly object s_keyboardShortcutMapLockMutex = new object();

            private static int s_keyHandlerCount = 0;
            private static bool s_deferredEnableShortcut = false;
        }
    }
}

