// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "TitleBarHelper.h"
#include "Converters/BooleanToVisibilityConverter.h"
#include "CalcViewModel/ViewState.h"

using namespace CalculatorApp::Common;
using namespace CalculatorApp::Converters;
using namespace Platform;
using namespace std;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

unique_ptr<TitleBarHelper> TitleBarHelper::CreateTitleBarHelperIfNotDocked(FrameworkElement^ customTitleBar)
{
    return (App::GetAppViewState() == ViewState::DockedView)
        ? nullptr
        : CalculatorApp::Common::TitleBarHelper::CreateTitleBarHelper(customTitleBar);
}

unique_ptr<TitleBarHelper> TitleBarHelper::CreateTitleBarHelper(_In_ FrameworkElement^ customTitleBar)
{
    assert(customTitleBar != nullptr);
    if (customTitleBar != nullptr)
    {
        CoreApplicationViewTitleBar^ coreTitleBar = CoreApplication::GetCurrentView()->TitleBar;
        assert(coreTitleBar != nullptr);
        if (coreTitleBar != nullptr)
        {
            return make_unique<TitleBarHelper>(coreTitleBar, customTitleBar);
        }
    }

    return nullptr;
}

TitleBarHelper::TitleBarHelper(_In_ CoreApplicationViewTitleBar^ coreTitleBar, _In_ FrameworkElement^ customTitleBar) :
    m_coreTitleBar(coreTitleBar),
    m_customTitleBar(customTitleBar)
{
    RegisterForLayoutChanged();
    RegisterForVisibilityChanged();
    SetCustomTitleBar();
}

TitleBarHelper::~TitleBarHelper()
{
    m_coreTitleBar->LayoutMetricsChanged -= m_layoutChangedToken;
    m_coreTitleBar->IsVisibleChanged -= m_visibilityChangedToken;
}

void TitleBarHelper::SetTitleBarHeight(double height)
{
    m_customTitleBar->Height = height;
}

void TitleBarHelper::SetTitleBarVisibility(bool isVisible)
{
    m_customTitleBar->Visibility = BooleanToVisibilityConverter::Convert(isVisible);
}

void TitleBarHelper::RegisterForLayoutChanged()
{
    m_layoutChangedToken =
        m_coreTitleBar->LayoutMetricsChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar^, Object^>(
        [this](CoreApplicationViewTitleBar^ cTitleBar, Object^)
    {
        // Update title bar control size as needed to account for system size changes
        SetTitleBarHeight(cTitleBar->Height);
    });
}

void TitleBarHelper::RegisterForVisibilityChanged()
{
    m_visibilityChangedToken =
        m_coreTitleBar->IsVisibleChanged += ref new TypedEventHandler<CoreApplicationViewTitleBar^, Object^>(
        [this](CoreApplicationViewTitleBar^ cTitleBar, Object^)
    {
        // Update title bar visibility
        SetTitleBarVisibility(cTitleBar->IsVisible);
    });
}

void TitleBarHelper::SetCustomTitleBar()
{
    // Set custom XAML Title Bar
    m_coreTitleBar->ExtendViewIntoTitleBar = true;
    SetTitleBarHeight(m_coreTitleBar->Height);
    SetTitleBarVisibility(m_coreTitleBar->IsVisible);
    Window::Current->SetTitleBar(m_customTitleBar);
}
