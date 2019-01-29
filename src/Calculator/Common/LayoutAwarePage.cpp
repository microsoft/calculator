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

    // Create an empty default view model
    DefaultViewModel = ref new Map<String^, Object^>(std::less<String^>());

    // When this page is part of the visual tree make two changes:
    // 1) Map application view state to visual state for the page
    // 2) Handle keyboard and mouse navigation requests
    Loaded += ref new RoutedEventHandler(this, &LayoutAwarePage::OnLoaded);

    // Undo the same changes when the page is no longer visible
    Unloaded += ref new RoutedEventHandler(this, &LayoutAwarePage::OnUnloaded);

    Language = LocalizationService::GetInstance()->GetLanguage();
}

static DependencyProperty^ _defaultViewModelProperty =
    DependencyProperty::Register("DefaultViewModel",
    TypeName(IObservableMap<String^, Object^>::typeid), TypeName(LayoutAwarePage::typeid), nullptr);

/// <summary>
/// Identifies the <see cref="DefaultViewModel"/> dependency property.
/// </summary>
DependencyProperty^ LayoutAwarePage::DefaultViewModelProperty::get()
{
    return _defaultViewModelProperty;
}

/// <summary>
/// Gets an implementation of <see cref="IObservableMap&lt;String, Object&gt;"/> designed to be
/// used as a trivial view model.
/// </summary>
IObservableMap<String^, Object^>^ LayoutAwarePage::DefaultViewModel::get()
{
    return safe_cast<IObservableMap<String^, Object^>^>(GetValue(DefaultViewModelProperty));
}

/// <summary>
/// Sets an implementation of <see cref="IObservableMap&lt;String, Object&gt;"/> designed to be
/// used as a trivial view model.
/// </summary>
void LayoutAwarePage::DefaultViewModel::set(IObservableMap<String^, Object^>^ value)
{
    SetValue(DefaultViewModelProperty, value);
}

/// <summary>
/// Invoked when the page is part of the visual tree
/// </summary>
/// <param name="sender">Instance that triggered the event.</param>
/// <param name="e">Event data describing the conditions that led to the event.</param>
void LayoutAwarePage::OnLoaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Keyboard and mouse navigation only apply when occupying the entire window
    if (this->ActualHeight == Window::Current->Bounds.Height &&
        this->ActualWidth == Window::Current->Bounds.Width)
    {
        // Listen to the window directly so focus isn't required
        _acceleratorKeyEventToken = Window::Current->CoreWindow->Dispatcher->AcceleratorKeyActivated +=
            ref new TypedEventHandler<CoreDispatcher^, AcceleratorKeyEventArgs^>(this,
            &LayoutAwarePage::CoreDispatcher_AcceleratorKeyActivated);
        _pointerPressedEventToken = Window::Current->CoreWindow->PointerPressed +=
            ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this,
            &LayoutAwarePage::CoreWindow_PointerPressed);
        _navigationShortcutsRegistered = true;
    }
}

/// <summary>
/// Invoked when the page is removed from visual tree
/// </summary>
/// <param name="sender">Instance that triggered the event.</param>
/// <param name="e">Event data describing the conditions that led to the event.</param>
void LayoutAwarePage::OnUnloaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_navigationShortcutsRegistered)
    {
        Window::Current->CoreWindow->Dispatcher->AcceleratorKeyActivated -= _acceleratorKeyEventToken;
        Window::Current->CoreWindow->PointerPressed -= _pointerPressedEventToken;
        _navigationShortcutsRegistered = false;
    }
}

#pragma region Navigation support

/// <summary>
/// Invoked as an event handler to navigate backward in the page's associated <see cref="Frame"/>
/// until it reaches the top of the navigation stack.
/// </summary>
/// <param name="sender">Instance that triggered the event.</param>
/// <param name="e">Event data describing the conditions that led to the event.</param>
void LayoutAwarePage::GoHome(Object^ sender, RoutedEventArgs^ e)
{
    (void) sender; // Unused parameter
    (void) e; // Unused parameter

    // Use the navigation frame to return to the topmost page
    if (Frame != nullptr)
    {
        while (Frame->CanGoBack)
        {
            Frame->GoBack();
        }
    }
}

/// <summary>
/// Invoked as an event handler to navigate backward in the navigation stack
/// associated with this page's <see cref="Frame"/>.
/// </summary>
/// <param name="sender">Instance that triggered the event.</param>
/// <param name="e">Event data describing the conditions that led to the event.</param>
void LayoutAwarePage::GoBack(Object^ sender, RoutedEventArgs^ e)
{
    (void) sender; // Unused parameter
    (void) e; // Unused parameter

    // Use the navigation frame to return to the previous page
    if (Frame != nullptr && Frame->CanGoBack)
    {
        Frame->GoBack();
    }
}

/// <summary>
/// Invoked as an event handler to navigate forward in the navigation stack
/// associated with this page's <see cref="Frame"/>.
/// </summary>
/// <param name="sender">Instance that triggered the event.</param>
/// <param name="e">Event data describing the conditions that led to the event.</param>
void LayoutAwarePage::GoForward(Object^ sender, RoutedEventArgs^ e)
{
    (void) sender; // Unused parameter
    (void) e; // Unused parameter

    // Use the navigation frame to advance to the next page
    if (Frame != nullptr && Frame->CanGoForward)
    {
        Frame->GoForward();
    }
}

/// <summary>
/// Invoked on every keystroke, including system keys such as Alt key combinations, when
/// this page is active and occupies the entire window.  Used to detect keyboard navigation
/// between pages even when the page itself doesn't have focus.
/// </summary>
/// <param name="sender">Instance that triggered the event.</param>
/// <param name="args">Event data describing the conditions that led to the event.</param>
void LayoutAwarePage::CoreDispatcher_AcceleratorKeyActivated(CoreDispatcher^ sender,
                                                             AcceleratorKeyEventArgs^ args)
{
    auto virtualKey = args->VirtualKey;

    // Only investigate further when Left, Right, or the dedicated Previous or Next keys
    // are pressed
    if ((args->EventType == CoreAcceleratorKeyEventType::SystemKeyDown ||
        args->EventType == CoreAcceleratorKeyEventType::KeyDown) &&
        (virtualKey == VirtualKey::Left || virtualKey == VirtualKey::Right ||
        (int)virtualKey == 166 || (int)virtualKey == 167))
    {
        auto coreWindow = Window::Current->CoreWindow;
        auto downState = Windows::UI::Core::CoreVirtualKeyStates::Down;
        bool menuKey = (coreWindow->GetKeyState(VirtualKey::Menu) & downState) == downState;
        bool controlKey = (coreWindow->GetKeyState(VirtualKey::Control) & downState) == downState;
        bool shiftKey = (coreWindow->GetKeyState(VirtualKey::Shift) & downState) == downState;
        bool noModifiers = !menuKey && !controlKey && !shiftKey;
        bool onlyAlt = menuKey && !controlKey && !shiftKey;

        if (((int)virtualKey == 166 && noModifiers) ||
            (virtualKey == VirtualKey::Left && onlyAlt))
        {
            // When the previous key or Alt+Left are pressed navigate back
            args->Handled = true;
            GoBack(this, ref new RoutedEventArgs());
        }
        else if (((int)virtualKey == 167 && noModifiers) ||
            (virtualKey == VirtualKey::Right && onlyAlt))
        {
            // When the next key or Alt+Right are pressed navigate forward
            args->Handled = true;
            GoForward(this, ref new RoutedEventArgs());
        }
    }
}

/// <summary>
/// Invoked on every mouse click, touch screen tap, or equivalent interaction when this
/// page is active and occupies the entire window.  Used to detect browser-style next and
/// previous mouse button clicks to navigate between pages.
/// </summary>
/// <param name="sender">Instance that triggered the event.</param>
/// <param name="args">Event data describing the conditions that led to the event.</param>
void LayoutAwarePage::CoreWindow_PointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
    auto properties = args->CurrentPoint->Properties;

    // Ignore button chords with the left, right, and middle buttons
    if (properties->IsLeftButtonPressed || properties->IsRightButtonPressed ||
        properties->IsMiddleButtonPressed) return;

    // If back or foward are pressed (but not both) navigate appropriately
    bool backPressed = properties->IsXButton1Pressed;
    bool forwardPressed = properties->IsXButton2Pressed;
    if (backPressed ^ forwardPressed)
    {
        args->Handled = true;
        if (backPressed) GoBack(this, ref new RoutedEventArgs());
        if (forwardPressed) GoForward(this, ref new RoutedEventArgs());
    }
}

#pragma endregion

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
