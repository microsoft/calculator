// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/TitleBar.g.h"

namespace CalculatorApp
{
    public ref class TitleBar sealed
    {
    public:
        TitleBar();

        void SetTitleBar();
    private:
        void OnLoaded(_In_ Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnUnloaded(_In_ Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void Initialize();
        void RegisterForLayoutChanged();
        void RegisterForVisibilityChanged();
        void SetTitleBarText(Platform::String^ text);
        void SetTitleBarHeight(double height);
        void SetTitleBarVisibility(bool isVisible);
        void SetTitleBarPadding();

        Platform::Agile<Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^> m_coreTitleBar;
        Windows::Foundation::EventRegistrationToken m_layoutChangedToken;
        Windows::Foundation::EventRegistrationToken m_visibilityChangedToken;
    };
}
