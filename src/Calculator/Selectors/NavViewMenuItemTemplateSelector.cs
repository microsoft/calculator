// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;

using System;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp.TemplateSelectors
{
    internal sealed class NavViewMenuItemTemplateSelector : DataTemplateSelector
    {
        public DataTemplate CategoryItemTemplate { get; set; }
        public DataTemplate CategoryGroupItemTemplate { get; set; }

        protected override DataTemplate SelectTemplateCore(object item)
        {
            if (item is NavCategory)
            {
                return CategoryItemTemplate;
            }
            else if (item is NavCategoryGroup)
            {
                return CategoryGroupItemTemplate;
            }
            else
            {
                throw new NotSupportedException($"typeof(item) must be {nameof(NavCategory)} or {nameof(NavCategoryGroup)}.");
            }
        }

        protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
        {
            return SelectTemplateCore(item);
        }
    }
}

