// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <collection.h>

namespace CalculatorApp
{
    namespace Common
    {
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class LayoutAwarePage : Windows::UI::Xaml::Controls::Page
        {
        internal:
            LayoutAwarePage();

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
            virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
            virtual void LoadState(Platform::Object^ navigationParameter,
                Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState);
            virtual void SaveState(Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState);

        private:
            Platform::String^ _pageKey;
        };
    }
}
