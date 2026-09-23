// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Windows.Input;

namespace CalculatorApp.ViewModel.Common
{
    public sealed class DelegateCommand : ICommand
    {
        private readonly Action<object> _handler;

        public DelegateCommand(Action<object> handler)
        {
            _handler = handler ?? throw new ArgumentNullException(nameof(handler));
        }

#pragma warning disable CS0067 // Event is never used
        public event EventHandler CanExecuteChanged;
#pragma warning restore CS0067

        public bool CanExecute(object parameter) => true;

        public void Execute(object parameter) => _handler.Invoke(parameter);
    }
}
