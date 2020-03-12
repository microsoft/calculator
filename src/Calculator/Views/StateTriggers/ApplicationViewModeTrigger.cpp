// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ApplicationViewModeTrigger.h"
using namespace CalculatorApp::Views::StateTriggers;
using namespace Windows::System::Profile;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;

DEPENDENCY_PROPERTY_INITIALIZATION(ApplicationViewModeTrigger, ViewMode);
DEPENDENCY_PROPERTY_INITIALIZATION(ApplicationViewModeTrigger, MinWindowHeight);
DEPENDENCY_PROPERTY_INITIALIZATION(ApplicationViewModeTrigger, MinWindowWidth);

ApplicationViewModeTrigger::ApplicationViewModeTrigger()
{
    m_windowSizeEventToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &ApplicationViewModeTrigger::WindowSizeChanged);
    UpdateTrigger();
}

void ApplicationViewModeTrigger::WindowSizeChanged(_In_ Platform::Object ^ /*sender*/, _In_ Windows::UI::Core::WindowSizeChangedEventArgs ^ /*e*/)
{
    // We don't use layout aware page's view states, we have our own
    UpdateTrigger();
}
void ApplicationViewModeTrigger::OnViewModePropertyChanged(_In_ AppViewMode /*oldValue*/, _In_ AppViewMode /*newValue*/)
{
    UpdateTrigger();
}
void ApplicationViewModeTrigger::OnMinWindowHeightPropertyChanged(double /*oldValue*/, double /*newValue*/)
{
    UpdateTrigger();
}

void ApplicationViewModeTrigger::OnMinWindowWidthPropertyChanged(double /*oldValue*/, double /*newValue*/)
{
    UpdateTrigger();
}

void ApplicationViewModeTrigger::UpdateTrigger()
{
    auto applicationView = ApplicationView::GetForCurrentView();
    auto viewMode = applicationView->ViewMode;
    if (applicationView->VisibleBounds.Width < this->MinWindowWidth || applicationView->VisibleBounds.Height < this->MinWindowHeight)
    {
        SetActive(false);
        return;
    }

    switch (static_cast<int>(viewMode))
    {
    case 0 /*ApplicationViewMode::Default*/:
        SetActive(this->ViewMode == AppViewMode::Normal);
        break;
    case 1 /*ApplicationViewMode::CompactOverlay*/:
        SetActive(this->ViewMode == AppViewMode::CompactOverlay);
        break;
    case 2 /*ApplicationViewMode::Spanning*/:

        // We will need to update this code to use new SpanningRects API instead of DisplayRegions when the app will use the SDK for windows 10 2004.
        auto displayRegions = applicationView->GetDisplayRegions();
        if (displayRegions->Size == 2)
        {
            auto display1 = displayRegions->GetAt(0);
            auto display2 = displayRegions->GetAt(1);
            if (display1->WorkAreaSize.Width > display1->WorkAreaSize.Height && display1->WorkAreaSize.Height == display2->WorkAreaSize.Height)
            {
                SetActive(this->ViewMode == AppViewMode::DoubleLandscape);
                return;
            }
            else if (display1->WorkAreaSize.Height > display1->WorkAreaSize.Width && display1->WorkAreaSize.Width == display2->WorkAreaSize.Width)
            {
                SetActive(this->ViewMode == AppViewMode::DoublePortrait);
                return;
            }
        }
        SetActive(this->ViewMode == AppViewMode::Normal);
        break;
    }
}
