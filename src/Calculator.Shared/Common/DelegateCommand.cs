// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Windows.Input;

namespace CalculatorApp
{
    namespace Common
    {
        public delegate void CommandHandlerFunc (object param);

        public class DelegateCommand : ICommand
        {
            public DelegateCommand(CommandHandlerFunc func)
            {
                m_function = func; 
            }

            // the ICommand methods will only be available if the ICommand interface is requested via a dynamic_cast
            // The ICommand interface is meant to be consumed by Xaml and not by the app, this is a defensive measure against
            // code in the app calling Execute.
            public void Execute(object parameter) => m_function(parameter);

            public bool CanExecute(object parameter) => true;

            CommandHandlerFunc m_function;

            public event EventHandler CanExecuteChanged;
        }
    }
}
