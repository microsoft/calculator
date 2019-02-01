// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "LayoutAwarePage.h"
#include "SuspensionManager.h"
#include "CalcViewModel\Common\LocalizationService.h"
#include "App.xaml.h"

using namespace CalculatorApp::Common;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes a new instance of the <see cref="LayoutAwarePage"/> class.
/// </summary>
LayoutAwarePage::LayoutAwarePage()
{
    if (Windows::ApplicationModel::DesignMode::DesignModeEnabled)
    {
        return;
    }

    Language = LocalizationService::GetInstance()->GetLanguage();
}

#pragma region Process lifetime management

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property provides the group to be displayed.</param>
void LayoutAwarePage::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Returning to a cached page through navigation shouldn't trigger state loading
    if (_pageKey != nullptr) return;

    auto frameState = SuspensionManager::SessionStateForFrame(Frame);
    _pageKey = "Page-" + Frame->BackStackDepth;

    if (e->NavigationMode == NavigationMode::New)
    {
        // Clear existing state for forward navigation when adding a new page to the
        // navigation stack
        auto nextPageKey = _pageKey;
        int nextPageIndex = Frame->BackStackDepth;
        while (frameState->HasKey(nextPageKey))
        {
            frameState->Remove(nextPageKey);
            nextPageIndex++;
            nextPageKey = "Page-" + nextPageIndex;
        }

        // Pass the navigation parameter to the new page
        LoadState(e->Parameter, nullptr);
    }
    else
    {
        // Pass the navigation parameter and preserved page state to the page, using
        // the same strategy for loading suspended state and recreating pages discarded
        // from cache
        LoadState(e->Parameter, safe_cast<IMap<String^, Object^>^>(frameState->Lookup(_pageKey)));
    }
}

/// <summary>
/// Invoked when this page will no longer be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property provides the group to be displayed.</param>
void LayoutAwarePage::OnNavigatedFrom(NavigationEventArgs^ e)
{
    auto frameState = SuspensionManager::SessionStateForFrame(Frame);
    auto pageState = ref new Map<String^, Object^>();
    SaveState(pageState);
    frameState->Insert(_pageKey, pageState);
}

/// <summary>
/// Populates the page with content passed during navigation.  Any saved state is also
/// provided when recreating a page from a prior session.
/// </summary>
/// <param name="navigationParameter">The parameter value passed to
/// <see cref="Frame.Navigate(Type, Object)"/> when this page was initially requested.
/// </param>
/// <param name="pageState">A map of state preserved by this page during an earlier
/// session.  This will be null the first time a page is visited.</param>
void LayoutAwarePage::LoadState(Object^ navigationParameter, IMap<String^, Object^>^ pageState)
{
}

/// <summary>
/// Preserves state associated with this page in case the application is suspended or the
/// page is discarded from the navigation cache.  Values must conform to the serialization
/// requirements of <see cref="SuspensionManager.SessionState"/>.
/// </summary>
/// <param name="pageState">An empty map to be populated with serializable state.</param>
void LayoutAwarePage::SaveState(IMap<String^, Object^>^ pageState)
{
}

#pragma endregion
