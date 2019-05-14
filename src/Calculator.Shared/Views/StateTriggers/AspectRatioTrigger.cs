// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* The AspectRatioTrigger class is a custom trigger for use with a VisualState. The trigger is designed to fire when the
   height/width of the source FrameworkElement is greater than a specified threshold. In order to be a flexible class, it
   exposes a NumeratorAspect property that can be either Height or Width. The property chosen will be the numerator when
   calculating the ratio between the two properties. Additionally, users can configure whether the ratio must be strictly
   greater than the threshold, or if equal should be considered acceptable for the state to trigger. */


using System;
using Windows.Foundation;
using Windows.UI.Xaml;

namespace CalculatorApp.Views.StateTriggers
{
    public enum Aspect
    {
        Height,
        Width
    }

    public sealed class AspectRatioTrigger : Windows.UI.Xaml.StateTriggerBase
    {
        /* The source for which this class will respond to size changed events. */
        public FrameworkElement Source
        {
            get { return (FrameworkElement)GetValue(SourceProperty); }
            set { SetValue(SourceProperty, value); }
        }

        public static readonly DependencyProperty SourceProperty =
            DependencyProperty.Register("Source", typeof(FrameworkElement), typeof(AspectRatioTrigger), new PropertyMetadata(null));

        /* Either Height or Width. The property will determine which aspect is used as the numerator when calculating
           the aspect ratio. */
        public Aspect NumeratorAspect
        {
            get { return (Aspect)GetValue(NumeratorAspectProperty); }
            set { SetValue(NumeratorAspectProperty, value); }
        }

        // Using a DependencyProperty as the backing store for NumeratorAspect.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty NumeratorAspectProperty =
            DependencyProperty.Register("NumeratorAspect", typeof(Aspect), typeof(AspectRatioTrigger), new PropertyMetadata(Aspect.Height));


        /* The threshold that will cause the trigger to fire when the aspect ratio exceeds this value. */
        public double Threshold
        {
            get { return (double)GetValue(ThresholdProperty); }
            set { SetValue(ThresholdProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Threshold.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ThresholdProperty =
            DependencyProperty.Register("Threshold", typeof(double), typeof(AspectRatioTrigger), new PropertyMetadata(0.0));



        /* If true, the trigger will fire if the aspect ratio is greater than or equal to the threshold. */
        public bool ActiveIfEqual
        {
            get { return (bool)GetValue(ActiveIfEqualProperty); }
            set { SetValue(ActiveIfEqualProperty, value); }
        }

        // Using a DependencyProperty as the backing store for ActiveIfEqual.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ActiveIfEqualProperty =
            DependencyProperty.Register("ActiveIfEqual", typeof(bool), typeof(AspectRatioTrigger), new PropertyMetadata(false));


        AspectRatioTrigger()
        {
            SetActive(false);
        }

        void OnSourcePropertyChanged(FrameworkElement oldValue, FrameworkElement newValue)
        {
            UnregisterSizeChanged(oldValue);
            RegisterSizeChanged(newValue);
        }

        void RegisterSizeChanged(FrameworkElement element)
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
        }

        void UnregisterSizeChanged(FrameworkElement element)
        {
            if (element != null)
            {
                element.SizeChanged -= OnSizeChanged;
            }
        }

        void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            UpdateIsActive(e.NewSize);
        }

        void UpdateIsActive(Size sourceSize)
        {
            double numerator, denominator;
            if (NumeratorAspect == Aspect.Height)
            {
                numerator = sourceSize.Height;
                denominator = sourceSize.Width;
            }
            else
            {
                numerator = sourceSize.Width;
                denominator = sourceSize.Height;
            }

            bool isActive = false;
            if (denominator > 0)
            {
                double ratio = numerator / denominator;
                double threshold = Math.Abs(Threshold);

                isActive = ((ratio > threshold) || (ActiveIfEqual && (ratio == threshold)));
            }

            SetActive(isActive);
        }

    }
}
