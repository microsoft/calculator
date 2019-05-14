// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.ComponentModel;
using Windows.UI.Xaml.Data;

namespace CalculatorApp
{
    namespace ViewModel
    {
        /// <summary>
        /// Model representation of a single item in the Memory list
        /// </summary>
        [Windows.UI.Xaml.Data.Bindable]
        public sealed class MemoryItemViewModel : INotifyPropertyChanged, Windows.UI.Xaml.Data.ICustomPropertyProvider
        {
            private StandardCalculatorViewModel m_calcVM;

            public MemoryItemViewModel(StandardCalculatorViewModel calcVM)
            {
                m_Position = -1;
                m_calcVM = calcVM;
            }

            public event PropertyChangedEventHandler PropertyChanged;
            private void RaisePropertyChanged([System.Runtime.CompilerServices.CallerMemberName]string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));
            private int m_Position;
            public int Position { get => m_Position; set { m_Position = value; RaisePropertyChanged("Position"); } }

            private string m_Value;
            public string Value { get => m_Value; set { m_Value = value; RaisePropertyChanged("Value"); } }

            public ICustomProperty GetCustomProperty(string name) { return null; }

            public ICustomProperty GetIndexedProperty(string name, Type type) { return null; }

            public Type Type
            {
                get
                {
                    return this.GetType();
                }
            }

            public string GetStringRepresentation() { return Value; }

            public void Clear()
            {
                m_calcVM.OnMemoryClear(Position);
            }

            public void MemoryAdd()
            {
                m_calcVM.OnMemoryAdd(Position);
            }

            public void MemorySubtract()
            {
                m_calcVM.OnMemorySubtract(Position);
            }

        };
    }
}
