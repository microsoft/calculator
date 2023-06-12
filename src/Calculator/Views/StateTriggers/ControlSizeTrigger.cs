// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.Foundation;
using Windows.UI.Xaml;

namespace CalculatorApp.Views.StateTriggers
{
    public sealed class ControlSizeTrigger : Windows.UI.Xaml.StateTriggerBase
    {
        public ControlSizeTrigger()
        {
            SetActive(false);
        }

        public FrameworkElement Source
        {
            get => (FrameworkElement)GetValue(SourceProperty);
            set => SetValue(SourceProperty, value);
        }

        // Using a DependencyProperty as the backing store for Source.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty SourceProperty =
            DependencyProperty.Register(nameof(Source), typeof(FrameworkElement), typeof(ControlSizeTrigger), new PropertyMetadata(default(FrameworkElement), (sender, args) =>
            {
                var self = (ControlSizeTrigger)sender;
                self.OnSourcePropertyChanged((FrameworkElement)args.OldValue, (FrameworkElement)args.NewValue);
            }));

        public double MinHeight
        {
            get => (double)GetValue(MinHeightProperty);
            set => SetValue(MinHeightProperty, value);
        }

        // Using a DependencyProperty as the backing store for MinHeight.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty MinHeightProperty =
            DependencyProperty.Register(nameof(MinHeight), typeof(double), typeof(ControlSizeTrigger), new PropertyMetadata(-1));

        public double MinWidth
        {
            get => (double)GetValue(MinWidthProperty);
            set => SetValue(MinWidthProperty, value);
        }

        // Using a DependencyProperty as the backing store for MinWidth.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty MinWidthProperty =
            DependencyProperty.Register(nameof(MinWidth), typeof(double), typeof(ControlSizeTrigger), new PropertyMetadata(-1));

        private void OnSourcePropertyChanged(FrameworkElement oldValue, FrameworkElement newValue)
        {
            UnregisterSizeChanged(oldValue);
            RegisterSizeChanged(newValue);
        }

        private void RegisterSizeChanged(FrameworkElement element)
        {
            if (element == null)
            {
                return;
            }

            if (element != Source)
            {
                UnregisterSizeChanged(Source);
            }

            element.SizeChanged += OnSizeChanged;
            UpdateIsActive(element.RenderSize);
        }

        private void UnregisterSizeChanged(FrameworkElement element)
        {
            if (element != null)
            {
                element.SizeChanged -= OnSizeChanged;
            }
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            UpdateIsActive(e.NewSize);
        }

        private void UpdateIsActive(Size sourceSize)
        {
            if (MinHeight >= 0)
            {
                SetActive(sourceSize.Height >= MinHeight && (MinWidth < 0 || sourceSize.Width >= MinWidth));
            }
            else
            {
                SetActive(MinWidth >= 0 && sourceSize.Width >= MinWidth);
            }
        }
    }
}

