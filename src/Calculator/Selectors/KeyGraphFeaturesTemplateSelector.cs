// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel;

using Windows.UI.Xaml;

namespace CalculatorApp
{
    namespace TemplateSelectors
    {
        public sealed class KeyGraphFeaturesTemplateSelector : Windows.UI.Xaml.Controls.DataTemplateSelector
        {
            public KeyGraphFeaturesTemplateSelector()
            {
            }

            public Windows.UI.Xaml.DataTemplate RichEditTemplate { get; set; }
            public Windows.UI.Xaml.DataTemplate GridTemplate { get; set; }
            public Windows.UI.Xaml.DataTemplate TextBlockTemplate { get; set; }

            protected override DataTemplate SelectTemplateCore(object item)
            {
                var kgfItem = (KeyGraphFeaturesItem)item;

                if (!kgfItem.IsText)
                {
                    if (kgfItem.DisplayItems.Count != 0)
                    {
                        return RichEditTemplate;
                    }
                    else if (kgfItem.GridItems.Count != 0)
                    {
                        return GridTemplate;
                    }
                }

                return TextBlockTemplate;
            }

            protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
            {
                return SelectTemplateCore(item);
            }
        }
    }
}
