using System;

namespace CalculatorApp.Common
{
    internal class DelegateCommand<TTarget> : System.Windows.Input.ICommand
    {
        public delegate void CommandHandlerFunc(object obj);

        public DelegateCommand(TTarget target, CommandHandlerFunc func)
        {
            m_weakTarget = new WeakReference(target);
            m_function = func;
        }

        public event EventHandler CanExecuteChanged;

        public bool CanExecute(object parameter)
        {
            return true;
        }

        public void Execute(object parameter)
        {
            object target = m_weakTarget.Target;
            if (target != null && target is TTarget)
            {
                m_function(parameter);
            }
        }

        private CommandHandlerFunc m_function;
        private WeakReference m_weakTarget;
    }
}

