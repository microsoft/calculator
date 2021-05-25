// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp::ViewModel
{
    namespace Common
    {
        public delegate void DelegateCommandHandler(Platform::Object ^ parameter);

        public ref class DelegateCommand sealed : public Windows::UI::Xaml::Input::ICommand
        {
        public:
            DelegateCommand(DelegateCommandHandler ^ handler)
                : m_handler(handler)
            {}

        private:
            // Explicit, and private, implementation of ICommand, this way of programming makes it so
            // the ICommand methods will only be available if the ICommand interface is requested via a dynamic_cast
            // The ICommand interface is meant to be consumed by Xaml and not by the app, this is a defensive measure against
            // code in the app calling Execute.
            virtual void ExecuteImpl(Platform::Object ^ parameter) sealed = Windows::UI::Xaml::Input::ICommand::Execute
            {
                m_handler->Invoke(parameter);
            }

            virtual bool CanExecuteImpl(Platform::Object ^ parameter) sealed = Windows::UI::Xaml::Input::ICommand::CanExecute
            {
                return true;
            }

            virtual event Windows::Foundation::EventHandler<Platform::Object ^> ^ CanExecuteChangedImpl
            {
                virtual Windows::Foundation::EventRegistrationToken add(Windows::Foundation::EventHandler<Platform::Object ^> ^ handler) sealed = Windows::UI::Xaml::Input::ICommand::CanExecuteChanged::add
                {
                    return m_canExecuteChanged += handler;
                }
                virtual void remove(Windows::Foundation::EventRegistrationToken token) sealed = Windows::UI::Xaml::Input::ICommand::CanExecuteChanged::remove
                {
                    m_canExecuteChanged -= token;
                }
            }

        private:
            DelegateCommandHandler ^ m_handler;

            event Windows::Foundation::EventHandler<Platform::Object ^> ^ m_canExecuteChanged;
        };

        template <typename TTarget, typename TFuncPtr>
        DelegateCommandHandler ^ MakeDelegateCommandHandler(TTarget ^ target, TFuncPtr&& function)
        {
            Platform::WeakReference weakTarget(target);
            return ref new DelegateCommandHandler([weakTarget, function=std::forward<TFuncPtr>(function)](Platform::Object ^ param)
                {
                    TTarget ^ thatTarget = weakTarget.Resolve<TTarget>();
                    if (nullptr != thatTarget)
                    {
                        (thatTarget->*function)(param);
                    }
                }
            );
        }
    }
}

