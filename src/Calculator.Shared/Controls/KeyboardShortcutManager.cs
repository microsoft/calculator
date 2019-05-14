// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


using Windows.UI.Xaml;

namespace CalculatorApp
{
    namespace Common
    {
        public static class KeyboardShortcutManager
        {
            public static string GetCharacter(DependencyObject obj) => (string)obj.GetValue(CharacterProperty);

            public static void SetCharacter(DependencyObject obj, string value) => obj.SetValue(CharacterProperty, value);

            // Using a DependencyProperty as the backing store for Character.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty CharacterProperty =
                DependencyProperty.RegisterAttached("Character", typeof(string), typeof(KeyboardShortcutManager), new PropertyMetadata(""));


            public static MyVirtualKey GetVirtualKey(DependencyObject obj) => (MyVirtualKey)obj.GetValue(VirtualKeyProperty);

            public static void SetVirtualKey(DependencyObject obj, MyVirtualKey value) => obj.SetValue(VirtualKeyProperty, value);

            // Using a DependencyProperty as the backing store for VirtualKey.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty VirtualKeyProperty =
                DependencyProperty.RegisterAttached("VirtualKey", typeof(MyVirtualKey), typeof(KeyboardShortcutManager), new PropertyMetadata(MyVirtualKey.None));




            public static MyVirtualKey GetVirtualKeyControlChord(DependencyObject obj) => (MyVirtualKey)obj.GetValue(VirtualKeyControlChordProperty);

            public static void SetVirtualKeyControlChord(DependencyObject obj, MyVirtualKey value) => obj.SetValue(VirtualKeyControlChordProperty, value);

            // Using a DependencyProperty as the backing store for VirtualKeyControlChord.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty VirtualKeyControlChordProperty =
                DependencyProperty.RegisterAttached("VirtualKeyControlChord", typeof(MyVirtualKey), typeof(KeyboardShortcutManager), new PropertyMetadata(MyVirtualKey.None));




            public static MyVirtualKey GetVirtualKeyShiftChord(DependencyObject obj) => (MyVirtualKey)obj.GetValue(VirtualKeyShiftChordProperty);

            public static void SetVirtualKeyShiftChord(DependencyObject obj, MyVirtualKey value) => obj.SetValue(VirtualKeyShiftChordProperty, value);

            // Using a DependencyProperty as the backing store for VirtualKeyShiftChord.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty VirtualKeyShiftChordProperty =
                DependencyProperty.RegisterAttached("VirtualKeyShiftChord", typeof(MyVirtualKey), typeof(KeyboardShortcutManager), new PropertyMetadata(MyVirtualKey.None));




            public static MyVirtualKey GetVirtualKeyAltChord(DependencyObject obj) => (MyVirtualKey)obj.GetValue(VirtualKeyAltChordProperty);

            public static void SetVirtualKeyAltChord(DependencyObject obj, MyVirtualKey value) => obj.SetValue(VirtualKeyAltChordProperty, value);

            // Using a DependencyProperty as the backing store for VirtualKeyAltChord.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty VirtualKeyAltChordProperty =
                DependencyProperty.RegisterAttached("VirtualKeyAltChord", typeof(MyVirtualKey), typeof(KeyboardShortcutManager), new PropertyMetadata(MyVirtualKey.None));




            public static MyVirtualKey GetVirtualKeyControlShiftChord(DependencyObject obj) => (MyVirtualKey)obj.GetValue(VirtualKeyControlShiftChordProperty);

            public static void SetVirtualKeyControlShiftChord(DependencyObject obj, MyVirtualKey value) => obj.SetValue(VirtualKeyControlShiftChordProperty, value);

            // Using a DependencyProperty as the backing store for VirtualKeyControlShiftChord.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty VirtualKeyControlShiftChordProperty =
                DependencyProperty.RegisterAttached("VirtualKeyControlShiftChord", typeof(MyVirtualKey), typeof(KeyboardShortcutManager), new PropertyMetadata(MyVirtualKey.None));




            public static MyVirtualKey GetVirtualKeyInverseChord(DependencyObject obj) => (MyVirtualKey)obj.GetValue(VirtualKeyInverseChordProperty);

            public static void SetVirtualKeyInverseChord(DependencyObject obj, MyVirtualKey value) => obj.SetValue(VirtualKeyInverseChordProperty, value);

            // Using a DependencyProperty as the backing store for VirtualKeyInverseChord.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty VirtualKeyInverseChordProperty =
                DependencyProperty.RegisterAttached("VirtualKeyInverseChord", typeof(MyVirtualKey), typeof(KeyboardShortcutManager), new PropertyMetadata(MyVirtualKey.None));



            public static MyVirtualKey GetVirtualKeyControlInverseChord(DependencyObject obj) => (MyVirtualKey)obj.GetValue(VirtualKeyControlInverseChordProperty);

            public static void SetVirtualKeyControlInverseChord(DependencyObject obj, MyVirtualKey value) => obj.SetValue(VirtualKeyControlInverseChordProperty, value);

            // Using a DependencyProperty as the backing store for VirtualKeyControlInverseChord.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty VirtualKeyControlInverseChordProperty =
                DependencyProperty.RegisterAttached("VirtualKeyControlInverseChord", typeof(MyVirtualKey), typeof(KeyboardShortcutManager), new PropertyMetadata(MyVirtualKey.None));
        }
    }
}
