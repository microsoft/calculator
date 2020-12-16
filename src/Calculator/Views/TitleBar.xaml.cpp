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
using namespace Windows::System::Profile;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Foundation::Collections;
using namespace Concurrency;

namespace CalculatorApp
{
    DEPENDENCY_PROPERTY_INITIALIZATION(TitleBar, IsAlwaysOnTopMode);

    TitleBar::TitleBar()
        : m_coreTitleBar(CoreApplication::GetCurrentView()->TitleBar)
    {
        m_uiSettings = ref new UISettings();
        m_accessibilitySettings = ref new AccessibilitySettings();
        InitializeComponent();

        m_coreTitleBar->ExtendViewIntoTitleBar = true;
        Window::Current->SetTitleBar(BackgroundElement);

        Loaded += ref new RoutedEventHandler(this, &TitleBar::OnLoaded);
        Unloaded += ref new RoutedEventHandler(this, &TitleBar::OnUnloaded);
#ifdef IS_STORE_BUILD
        AppName->Text = AppResourceProvider::GetInstance()->GetResourceString(L"AppName");
#else
        AppName->Text = AppResourceProvider::GetInstance()->GetResourceString(L"DevAppName");
#endif // IS_STORE_BUILD
    }

    void TitleBar::OnLoaded(_In_ Object ^ /*sender*/, _In_ RoutedEventArgs ^ /*e*/)
    {
        auto that(this);
        // Register events
        m_visibilityChangedToken = m_coreTitleBar->IsVisibleChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar ^, Object ^>(
            [that](CoreApplicationViewTitleBar ^ cTitleBar, Object ^) { that->SetTitleBarVisibility(false); });
        m_layoutChangedToken = m_coreTitleBar->LayoutMetricsChanged +=
            ref new TypedEventHandler<CoreApplicationViewTitleBar ^, Object ^>([that](CoreApplicationViewTitleBar ^ cTitleBar, Object ^) {
                that->SetTitleBarHeightAndPadding();
            });

        m_colorValuesChangedToken = m_uiSettings->ColorValuesChanged += ref new TypedEventHandler<UISettings ^, Object ^>(this, &TitleBar::ColorValuesChanged);
        m_accessibilitySettingsToken = m_accessibilitySettings->HighContrastChanged +=
            ref new Windows::Foundation::TypedEventHandler<AccessibilitySettings ^, Object ^>(this, &CalculatorApp::TitleBar::OnHighContrastChanged);
        m_windowActivatedToken = Window::Current->Activated +=
            ref new Windows::UI::Xaml::WindowActivatedEventHandler(this, &CalculatorApp::TitleBar::OnWindowActivated);
        // Set properties
        SetTitleBarControlColors();
        SetTitleBarHeightAndPadding();

        // As of Windows 10 1903: when an app runs on a PC (without Tablet mode activated)
        // properties of CoreApplicationViewTitleBar aren't initialized during the first seconds after launch.
        auto forceDisplay = AnalyticsInfo::VersionInfo->DeviceFamily == L"Windows.Desktop"
                            && UIViewSettings::GetForCurrentView()->UserInteractionMode == UserInteractionMode::Mouse;

        SetTitleBarVisibility(forceDisplay);
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

    void TitleBar::SetTitleBarVisibility(bool forceDisplay)
    {
        this->LayoutRoot->Visibility =
            forceDisplay || m_coreTitleBar->IsVisible || IsAlwaysOnTopMode ? ::Visibility::Visible : ::Visibility::Collapsed;
    }

    void TitleBar::SetTitleBarHeightAndPadding()
    {
        if (m_coreTitleBar->Height == 0)
        {
            // The titlebar isn't init
            return;
        }

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
        this->LayoutRoot->Height = m_coreTitleBar->Height;
    }

    void TitleBar::ColorValuesChanged(_In_ UISettings ^ /*sender*/, _In_ Object ^ /*e*/)
    {
        auto that(this);
        Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([that]() { that->SetTitleBarControlColors(); }));
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
        auto that(this);
        Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([that]() {
                                 that->SetTitleBarControlColors();
                                 that->SetTitleBarVisibility(false);
                             }));
    }

    void TitleBar::OnWindowActivated(_In_ Object ^ /*sender*/, _In_ WindowActivatedEventArgs ^ e)
    {
        VisualStateManager::GoToState(
            this, e->WindowActivationState == CoreWindowActivationState::Deactivated ? WindowNotFocused->Name : WindowFocused->Name, false);
    }

    void TitleBar::OnIsAlwaysOnTopModePropertyChanged(bool /*oldValue*/, bool newValue)
    {
        SetTitleBarVisibility(false);
        VisualStateManager::GoToState(this, newValue ? "AOTMiniState" : "AOTNormalState", false);
    }

    void TitleBar::AlwaysOnTopButton_Click(_In_ Object ^ /*sender*/, _In_ RoutedEventArgs ^ e)
    {
        AlwaysOnTopClick(this, e);
    }
}
