// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Light C++/CX port of Microsoft.Toolkit.Uwp.UI.Extensions.VisualTree from the Windows Community toolkit
// Original version here:
// https://raw.githubusercontent.com/windows-toolkit/WindowsCommunityToolkit/master/Microsoft.Toolkit.Uwp.UI/Extensions/Tree/VisualTree.cs

namespace Calculator::Utils
{
    /// <summary>
    /// Defines a collection of extensions methods for UI.
    /// </summary>
    ref class VisualTree sealed
    {
        /// <summary>
        /// Find descendant <see cref="Windows::UI::Xaml::FrameworkElement ^"/> control using its name.
        /// </summary>
        /// <param name="element">Parent element.</param>
        /// <param name="name">Name of the control to find</param>
        /// <returns>Descendant control or null if not found.</returns>
        internal : static Windows::UI::Xaml::FrameworkElement ^ FindDescendantByName(Windows::UI::Xaml::DependencyObject ^ element, Platform::String ^ name);

        /// <summary>
        /// Find first descendant control of a specified type.
        /// </summary>
        /// <param name="element">Parent element.</param>
        /// <param name="type">Type of descendant.</param>
        /// <returns>Descendant control or null if not found.</returns>
        static Windows::UI::Xaml::DependencyObject
            ^ FindDescendant(Windows::UI::Xaml::DependencyObject ^ element, Windows::UI::Xaml::Interop::TypeName typeName);


        /// <summary>
        /// Find visual ascendant <see cref="Windows::UI::Xaml::FrameworkElement ^"/> control using its name.
        /// </summary>
        /// <param name="element">Parent element.</param>
        /// <param name="name">Name of the control to find</param>
        /// <returns>Descendant control or null if not found.</returns>
        static Windows::UI::Xaml::FrameworkElement ^ FindAscendantByName(Windows::UI::Xaml::DependencyObject ^ element, Platform::String ^ name);
        
        /// <summary>
        /// Find first visual ascendant control of a specified type.
        /// </summary>
        /// <param name="element">Child element.</param>
        /// <param name="type">Type of ascendant to look for.</param>
        /// <returns>Ascendant control or null if not found.</returns>
        static Platform::Object ^ FindAscendant(Windows::UI::Xaml::DependencyObject ^ element, Windows::UI::Xaml::Interop::TypeName type);

    };
}
