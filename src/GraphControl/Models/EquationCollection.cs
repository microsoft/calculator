// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;

namespace GraphControl
{
    public delegate void EquationChangedEventHandler(Equation sender);
    public delegate void VisibilityChangedEventHandler(Equation sender);

    public sealed class EquationCollection : ObservableCollection<Equation>
    {
        public event EquationChangedEventHandler EquationChanged;
        public event EquationChangedEventHandler EquationStyleChanged;
        public event EquationChangedEventHandler EquationLineEnabledChanged;

        public EquationCollection()
        {
        }

        protected override void InsertItem(int index, Equation item)
        {
            base.InsertItem(index, item);
            if (item != null)
            {
                item.PropertyChanged += OnEquationPropertyChanged;
            }
        }

        protected override void RemoveItem(int index)
        {
            if (index >= 0 && index < Count)
            {
                var item = this[index];
                if (item != null)
                {
                    item.PropertyChanged -= OnEquationPropertyChanged;
                }
            }
            base.RemoveItem(index);
        }

        protected override void SetItem(int index, Equation item)
        {
            if (index >= 0 && index < Count)
            {
                var oldItem = this[index];
                if (oldItem != null)
                {
                    oldItem.PropertyChanged -= OnEquationPropertyChanged;
                }
            }
            base.SetItem(index, item);
            if (item != null)
            {
                item.PropertyChanged += OnEquationPropertyChanged;
            }
        }

        protected override void ClearItems()
        {
            foreach (var item in this)
            {
                if (item != null)
                {
                    item.PropertyChanged -= OnEquationPropertyChanged;
                }
            }
            base.ClearItems();
        }

        private void OnEquationPropertyChanged(object sender, PropertyChangedEventArgs args)
        {
            if (sender is Equation equation)
            {
                var propertyName = args.PropertyName;
                if (propertyName == Equation.LineColorPropertyName ||
                    propertyName == Equation.IsSelectedPropertyName ||
                    propertyName == Equation.EquationStylePropertyName)
                {
                    EquationStyleChanged?.Invoke(equation);
                }
                else if (propertyName == Equation.ExpressionPropertyName)
                {
                    EquationChanged?.Invoke(equation);
                }
                else if (propertyName == Equation.IsLineEnabledPropertyName)
                {
                    EquationLineEnabledChanged?.Invoke(equation);
                }
            }
        }
    }
}
