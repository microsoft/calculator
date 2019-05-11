// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Common
    {
        template <typename TTarget>
        ref class DelegateCommand : public Windows::UI::Xaml::Input::ICommand
        {
            internal :

                typedef void (TTarget::*CommandHandlerFunc)(Platform::Object ^);

            DelegateCommand(TTarget ^ target, CommandHandlerFunc func)
                : m_weakTarget(target)
                , m_function(func)
            {
            }

        private:
            // Explicit, and private, implementation of ICommand, this way of programming makes it so
            // the ICommand methods will only be available if the ICommand interface is requested via a dynamic_cast
            // The ICommand interface is meant to be consumed by Xaml and not by the app, this is a defensive measure against
            // code in the app calling Execute.
            virtual void ExecuteImpl(Platform::Object ^ parameter) sealed = Windows::UI::Xaml::Input::ICommand::Execute
            {
                TTarget ^ target = m_weakTarget.Resolve<TTarget>();
                if (target)
                {
                    (target->*m_function)(parameter);
                }
            }

            virtual bool CanExecuteImpl(Platform::Object ^ parameter) sealed = Windows::UI::Xaml::Input::ICommand::CanExecute
            {
                return true;
            }

            virtual event Windows::Foundation::EventHandler<Platform::Object^>^ CanExecuteChangedImpl
            {
                virtual Windows::Foundation::EventRegistrationToken add(Windows::Foundation::EventHandler<Platform::Object^>^ handler) sealed = Windows::UI::Xaml::Input::ICommand::CanExecuteChanged::add
                {
                    return m_canExecuteChanged += handler;
                }
                virtual void remove(Windows::Foundation::EventRegistrationToken token) sealed = Windows::UI::Xaml::Input::ICommand::CanExecuteChanged::remove
                {
                    m_canExecuteChanged -= token;
                }
            }

        private:

            event Windows::Foundation::EventHandler<Platform::Object^>^ m_canExecuteChanged;

            CommandHandlerFunc m_function;
            Platform::WeakReference m_weakTarget;
        };

        template <typename TTarget, typename TFuncPtr>
            DelegateCommand<TTarget> ^ MakeDelegate(TTarget ^ target, TFuncPtr&& function) {
                return ref new DelegateCommand<TTarget>(target, std::forward<TFuncPtr>(function));
            }

    }
}
