// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

// Light C++/CX port of Microsoft.Toolkit.Uwp.UI.Extensions.VisualTree from the Windows Community toolkit
// Original version here:
// https://raw.githubusercontent.com/windows-toolkit/WindowsCommunityToolkit/master/Microsoft.Toolkit.Uwp.UI/Extensions/Tree/VisualTree.cs

namespace Calculator.Utils
{
    /// <summary>
    /// Defines a collection of extensions methods for UI.
    /// </summary>
    internal static class VisualTree
    {
        /// <summary>
        /// Find descendant <see cref="Windows.UI.Xaml.FrameworkElement ^"/> control using its name.
        /// </summary>
        /// <param name="element">Parent element.</param>
        /// <param name="name">Name of the control to find</param>
        /// <returns>Descendant control or null if not found.</returns>
        internal static FrameworkElement FindDescendantByName(DependencyObject element, string name)
        {
            if (element == null || name == null || name.Length == 0)
            {
                return null;
            }

            if (element is FrameworkElement frameworkElement && name.Equals(frameworkElement.Name))
            {
                return frameworkElement;
            }

            var childCount = VisualTreeHelper.GetChildrenCount(element);
            for (var i = 0; i < childCount; i++)
            {
                var result = FindDescendantByName(VisualTreeHelper.GetChild(element, i), name);
                if (result != null)
                {
                    return result;
                }
            }

            return null;
        }

        /// <summary>
        /// Find first descendant control of a specified type.
        /// </summary>
        /// <param name="element">Parent element.</param>
        /// <param name="typeName">Type of descendant.</param>
        /// <returns>Descendant control or null if not found.</returns>
        private static DependencyObject FindDescendant(DependencyObject element, Type typeName)
        {
            DependencyObject retValue = null;
            var childrenCount = VisualTreeHelper.GetChildrenCount(element);

            for (var i = 0; i < childrenCount; i++)
            {
                var child = VisualTreeHelper.GetChild(element, i);
                if (child.GetType() == typeName)
                {
                    retValue = child;
                    break;
                }

                retValue = FindDescendant(child, typeName);

                if (retValue != null)
                {
                    break;
                }
            }

            return retValue;
        }

        /// <summary>
        /// Find visual ascendant <see cref="Windows.UI.Xaml.FrameworkElement ^"/> control using its name.
        /// </summary>
        /// <param name="element">Parent element.</param>
        /// <param name="name">Name of the control to find</param>
        /// <returns>Descendant control or null if not found.</returns>
        private static FrameworkElement FindAscendantByName(DependencyObject element, string name)
        {
            if (element == null || name == null || name.Length == 0)
            {
                return null;
            }

            var parent = VisualTreeHelper.GetParent(element);

            if (parent == null)
            {
                return null;
            }

            if (parent is FrameworkElement frameworkElement && name.Equals(frameworkElement.Name))
            {
                return frameworkElement;
            }

            return FindAscendantByName(parent, name);
        }

        /// <summary>
        /// Find first visual ascendant control of a specified type.
        /// </summary>
        /// <param name="element">Child element.</param>
        /// <param name="type">Type of ascendant to look for.</param>
        /// <returns>Ascendant control or null if not found.</returns>
        private static object FindAscendant(DependencyObject element, Type typeName)
        {
            var parent = VisualTreeHelper.GetParent(element);

            if (parent == null)
            {
                return null;
            }

            if (parent.GetType() == typeName)
            {
                return parent;
            }

            return FindAscendant(parent, typeName);
        }
    }
}

