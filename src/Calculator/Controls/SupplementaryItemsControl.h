// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Controls
    {
    public
        ref class SupplementaryItemsControl sealed : public Windows::UI::Xaml::Controls::ItemsControl
        {
        public:
            SupplementaryItemsControl()
            {
            }

        protected:
            virtual Windows::UI::Xaml::DependencyObject ^ GetContainerForItemOverride() override;
            virtual void PrepareContainerForItemOverride(Windows::UI::Xaml::DependencyObject ^ element, Platform::Object ^ item) override;
        };

    public
        ref class SupplementaryContentPresenter sealed : public Windows::UI::Xaml::Controls::ContentPresenter
        {
        public:
            SupplementaryContentPresenter()
            {
            }

        protected:
            virtual Windows::UI::Xaml::Automation::Peers::AutomationPeer ^ OnCreateAutomationPeer() override;
        };

        ref class SupplementaryContentPresenterAP sealed : public Windows::UI::Xaml::Automation::Peers::FrameworkElementAutomationPeer
        {
        protected:
            virtual Windows::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() override
            {
                return Windows::UI::Xaml::Automation::Peers::AutomationControlType::Text;
            }

            virtual Windows::Foundation::Collections::IVector<Windows::UI::Xaml::Automation::Peers::AutomationPeer ^> ^ GetChildrenCore() override
            {
                return nullptr;
            }

            internal : SupplementaryContentPresenterAP(SupplementaryContentPresenter ^ owner)
                : Windows::UI::Xaml::Automation::Peers::FrameworkElementAutomationPeer(owner)
            {
            }
        };
    }
}
