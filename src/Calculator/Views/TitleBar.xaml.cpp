// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "TitleBar.xaml.h"
#include "CalcViewModel/Common/AppResourceProvider.h"

using namespace std;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation::Collections;

namespace CalculatorApp
{
    TitleBar::TitleBar() :
        m_coreTitleBar(CoreApplication::GetCurrentView()->TitleBar)
    {
        InitializeComponent();
        
        Loaded += ref new RoutedEventHandler(this, &TitleBar::OnLoaded);
        Unloaded += ref new RoutedEventHandler(this, &TitleBar::OnUnloaded);

        m_coreTitleBar->ExtendViewIntoTitleBar = true;

        this->Initialize();
    }

    void TitleBar::OnLoaded(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
    {
        this->RegisterForLayoutChanged();
        this->RegisterForVisibilityChanged();
    }

    void TitleBar::OnUnloaded(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
    {
        m_coreTitleBar->LayoutMetricsChanged -= m_layoutChangedToken;
        m_coreTitleBar->IsVisibleChanged -= m_visibilityChangedToken;
    }

    void TitleBar::Initialize()
    {
        SetTitleBarText(AppResourceProvider::GetInstance().GetResourceString(L"AppName"));
        SetTitleBarHeight(m_coreTitleBar->Height);
        SetTitleBarVisibility(m_coreTitleBar->IsVisible);
        SetTitleBarPadding();
        SetTitleBar();
    }

    void TitleBar::RegisterForLayoutChanged()
    {
        m_layoutChangedToken =
            m_coreTitleBar->LayoutMetricsChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar^, Object^>(
                [this](CoreApplicationViewTitleBar^ cTitleBar, Object^)
        {
            // Update title bar control size as needed to account for system size changes
            SetTitleBarHeight(cTitleBar->Height);
            SetTitleBarPadding();
        });
    }

    void TitleBar::RegisterForVisibilityChanged()
    {
        m_visibilityChangedToken =
            m_coreTitleBar->IsVisibleChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar^, Object^>(
                [this](CoreApplicationViewTitleBar^ cTitleBar, Object^)
        {
            // Update title bar visibility
            SetTitleBarVisibility(cTitleBar->IsVisible);
        });
    }

    void TitleBar::SetTitleBarText(String^ text)
    {
        this->AppName->Text = text;
    }

    void TitleBar::SetTitleBarHeight(double height)
    {
        this->Height = height;
    }

    void TitleBar::SetTitleBarVisibility(bool isVisible)
    {
        this->Visibility = isVisible ? ::Visibility::Visible : ::Visibility::Collapsed;
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

        auto padding = Thickness(leftAddition, 0, rightAddition, 0);

        this->LayoutRoot->Margin = padding;
    }

    void TitleBar::SetTitleBar()
    {
        Window::Current->SetTitleBar(this->LayoutRoot);
    }
}

