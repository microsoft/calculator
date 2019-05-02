// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "TitleBar.xaml.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "CalcViewModel/Common/Utils.h"

using namespace std;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Foundation::Collections;

namespace CalculatorApp
{
    TitleBar::TitleBar()
        : m_coreTitleBar(CoreApplication::GetCurrentView()->TitleBar)
    {
        m_uiSettings = ref new UISettings();
        m_accessibilitySettings = ref new AccessibilitySettings();
        InitializeComponent();

        Loaded += ref new RoutedEventHandler(this, &TitleBar::OnLoaded);
        Unloaded += ref new RoutedEventHandler(this, &TitleBar::OnUnloaded);

        AppName->Text = AppResourceProvider::GetInstance().GetResourceString(L"AppName");
    }

    void TitleBar::OnLoaded(_In_ Object ^ /*sender*/, _In_ RoutedEventArgs ^ /*e*/)
    {
        // Register events
        m_visibilityChangedToken = m_coreTitleBar->IsVisibleChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar ^, Object ^>(
            [this](CoreApplicationViewTitleBar ^ cTitleBar, Object ^) { this->SetTitleBarVisibility(); });
        m_layoutChangedToken = m_coreTitleBar->LayoutMetricsChanged +=
            ref new TypedEventHandler<CoreApplicationViewTitleBar ^, Object ^>([this](CoreApplicationViewTitleBar ^ cTitleBar, Object ^) {
                this->LayoutRoot->Height = cTitleBar->Height;
                this->SetTitleBarPadding();
            });

        m_colorValuesChangedToken = m_uiSettings->ColorValuesChanged += ref new TypedEventHandler<UISettings ^, Object ^>(this, &TitleBar::ColorValuesChanged);
        m_accessibilitySettingsToken = m_accessibilitySettings->HighContrastChanged +=
            ref new Windows::Foundation::TypedEventHandler<AccessibilitySettings ^, Object ^>(this, &CalculatorApp::TitleBar::OnHighContrastChanged);
        m_windowActivatedToken = Window::Current->Activated +=
            ref new Windows::UI::Xaml::WindowActivatedEventHandler(this, &CalculatorApp::TitleBar::OnWindowActivated);
        // Set properties
        LayoutRoot->Height = m_coreTitleBar->Height;
        SetTitleBarControlColors();
        SetTitleBarExtendView();
        SetTitleBarVisibility();
        SetTitleBarPadding();
    }

    void TitleBar::OnUnloaded(_In_ Object ^ /*sender*/, _In_ RoutedEventArgs ^ /*e*/)
    {
        // Unregister events
        m_coreTitleBar->LayoutMetricsChanged -= m_layoutChangedToken;
        m_layoutChangedToken.Value = 0;
        m_coreTitleBar->IsVisibleChanged -= m_visibilityChangedToken;
        m_visibilityChangedToken.Value = 0;
        m_uiSettings->ColorValuesChanged -= m_colorValuesChangedToken;
        m_colorValuesChangedToken.Value = 0;
        m_accessibilitySettings->HighContrastChanged -= m_accessibilitySettingsToken;
        m_accessibilitySettingsToken.Value = 0;
        Window::Current->Activated -= m_windowActivatedToken;
        m_windowActivatedToken.Value = 0;
    }

    void TitleBar::SetTitleBarExtendView()
    {
        m_coreTitleBar->ExtendViewIntoTitleBar = !m_accessibilitySettings->HighContrast;
    }

    void TitleBar::SetTitleBarVisibility()
    {
        this->LayoutRoot->Visibility = m_coreTitleBar->IsVisible && !m_accessibilitySettings->HighContrast ? ::Visibility::Visible : ::Visibility::Collapsed;
    }

    void TitleBar::SetTitleBarPadding()
    {
        double leftAddition = 0;
        double rightAddition = 0;

        if (this->FlowDirection == ::FlowDirection::LeftToRight)
        {
            leftAddition = m_coreTitleBar->SystemOverlayLeftInset;
            rightAddition = m_coreTitleBar->SystemOverlayRightInset;
        }
        else
        {
            leftAddition = m_coreTitleBar->SystemOverlayRightInset;
            rightAddition = m_coreTitleBar->SystemOverlayLeftInset;
        }

        this->LayoutRoot->Padding = Thickness(leftAddition, 0, rightAddition, 0);
    }

    void TitleBar::ColorValuesChanged(_In_ UISettings ^ /*sender*/, _In_ Object ^ /*e*/)
    {
        Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]() { SetTitleBarControlColors(); }));
    }

    void TitleBar::SetTitleBarControlColors()
    {
        auto applicationView = ApplicationView::GetForCurrentView();
        if (applicationView == nullptr)
        {
            return;
        }

        auto applicationTitleBar = applicationView->TitleBar;
        if (applicationTitleBar == nullptr)
        {
            return;
        }

        if (m_accessibilitySettings->HighContrast)
        {
            // Reset to use default colors.
            applicationTitleBar->ButtonBackgroundColor = nullptr;
            applicationTitleBar->ButtonForegroundColor = nullptr;
            applicationTitleBar->ButtonInactiveBackgroundColor = nullptr;
            applicationTitleBar->ButtonInactiveForegroundColor = nullptr;
            applicationTitleBar->ButtonHoverBackgroundColor = nullptr;
            applicationTitleBar->ButtonHoverForegroundColor = nullptr;
            applicationTitleBar->ButtonPressedBackgroundColor = nullptr;
            applicationTitleBar->ButtonPressedForegroundColor = nullptr;
        }
        else
        {
            Color bgColor = Colors::Transparent;
            Color fgColor = safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup("SystemControlPageTextBaseHighBrush"))->Color;
            Color inactivefgColor =
                safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup("SystemControlForegroundChromeDisabledLowBrush"))->Color;
            Color hoverbgColor = safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup("SystemControlBackgroundListLowBrush"))->Color;
            Color hoverfgColor = safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup("SystemControlForegroundBaseHighBrush"))->Color;
            Color pressedbgColor = safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup("SystemControlBackgroundListMediumBrush"))->Color;
            Color pressedfgCoolor = safe_cast<SolidColorBrush ^>(Application::Current->Resources->Lookup("SystemControlForegroundBaseHighBrush"))->Color;
            applicationTitleBar->ButtonBackgroundColor = bgColor;
            applicationTitleBar->ButtonForegroundColor = fgColor;
            applicationTitleBar->ButtonInactiveBackgroundColor = bgColor;
            applicationTitleBar->ButtonInactiveForegroundColor = inactivefgColor;
            applicationTitleBar->ButtonHoverBackgroundColor = hoverbgColor;
            applicationTitleBar->ButtonHoverForegroundColor = hoverfgColor;
            applicationTitleBar->ButtonPressedBackgroundColor = pressedbgColor;
            applicationTitleBar->ButtonPressedForegroundColor = pressedfgCoolor;
        }
    }

    void TitleBar::OnHighContrastChanged(_In_ AccessibilitySettings ^ /*sender*/, _In_ Object ^ /*args*/)
    {
        Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]() {
                                 SetTitleBarControlColors();
                                 SetTitleBarExtendView();
                                 SetTitleBarVisibility();
                             }));
    }

    void TitleBar::OnWindowActivated(_In_ Object ^ /*sender*/, _In_ WindowActivatedEventArgs ^ e)
    {
        VisualStateManager::GoToState(
            this, e->WindowActivationState == CoreWindowActivationState::Deactivated ? WindowNotFocused->Name : WindowFocused->Name, false);
    }
}
