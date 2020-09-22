// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
#include "App.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "CalcViewModel/Common/Automation/NarratorNotifier.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "CalcViewModel/Common/LocalizationSettings.h"
#include "Views/MainPage.xaml.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::Automation;

using namespace Concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::Activation;

namespace CalculatorApp
{
    namespace ApplicationResourceKeys
    {
        StringReference AppMinWindowHeight(L"AppMinWindowHeight");
        StringReference AppMinWindowWidth(L"AppMinWindowWidth");
    }
}

/// <summary>
/// Initializes the singleton application object. This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

    m_preLaunched = false;

    RegisterDependencyProperties();

    // TODO: MSFT 14645325: Set this directly from XAML.
    // Currently this is bugged so the property is only respected from code-behind.
    this->HighContrastAdjustment = ApplicationHighContrastAdjustment::None;

    this->Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);

#if _DEBUG
    this->DebugSettings->IsBindingTracingEnabled = true;
    this->DebugSettings->BindingFailed += ref new BindingFailedEventHandler([](_In_ Object ^ /*sender*/, _In_ BindingFailedEventArgs ^ e) {
        if (IsDebuggerPresent())
        {
            ::Platform::String ^ errorMessage = e->Message;
            __debugbreak();
        }
    });
#endif
}

void App::AddWindowToMap(_In_ WindowFrameService ^ frameService)
{
    reader_writer_lock::scoped_lock lock(m_windowsMapLock);
    m_secondaryWindows[frameService->GetViewId()] = frameService;
    TraceLogger::GetInstance()->UpdateWindowCount(m_secondaryWindows.size());
}

WindowFrameService ^ App::GetWindowFromMap(int viewId)
{
    reader_writer_lock::scoped_lock_read lock(m_windowsMapLock);
    auto windowMapEntry = m_secondaryWindows.find(viewId);
    if (windowMapEntry != m_secondaryWindows.end())
    {
        return windowMapEntry->second;
    }
    return nullptr;
}

void App::RemoveWindowFromMap(int viewId)
{
    reader_writer_lock::scoped_lock lock(m_windowsMapLock);
    auto iter = m_secondaryWindows.find(viewId);
    assert(iter != m_secondaryWindows.end() && "Window does not exist in the list");
    m_secondaryWindows.erase(viewId);
}

void App::RemoveWindow(_In_ WindowFrameService ^ frameService)
{
    // Shell does not allow killing the main window.
    if (m_mainViewId != frameService->GetViewId())
    {
        HandleViewReleaseAndRemoveWindowFromMap(frameService);
    }
}

task<void> App::HandleViewReleaseAndRemoveWindowFromMap(_In_ WindowFrameService ^ frameService)
{
    WeakReference weak(this);

    // Unregister the event handler of the Main Page
    auto frame = safe_cast<Frame ^>(Window::Current->Content);
    auto mainPage = safe_cast<MainPage ^>(frame->Content);
    mainPage->UnregisterEventHandlers();

    return frameService->HandleViewRelease().then(
        [weak, frameService]() {
            auto that = weak.Resolve<App>();
            that->RemoveWindowFromMap(frameService->GetViewId());
        },
        task_continuation_context::use_arbitrary());
}

#pragma optimize("", off) // Turn off optimizations to work around coroutine optimization bug
task<void> App::SetupJumpList()
{
    try
    {
        auto calculatorOptions = NavCategoryGroup::CreateCalculatorCategory();

        auto jumpList = co_await JumpList::LoadCurrentAsync();
        jumpList->SystemGroupKind = JumpListSystemGroupKind::None;
        jumpList->Items->Clear();

        for (NavCategory ^ option : calculatorOptions->Categories)
        {
            if (!option->IsEnabled)
            {
                continue;
            }
            ViewMode mode = option->Mode;
            auto item = JumpListItem::CreateWithArguments(((int)mode).ToString(), L"ms-resource:///Resources/" + NavCategory::GetNameResourceKey(mode));
            item->Description = L"ms-resource:///Resources/" + NavCategory::GetNameResourceKey(mode);
            item->Logo = ref new Uri("ms-appx:///Assets/" + mode.ToString() + ".png");

            jumpList->Items->Append(item);
        }

        co_await jumpList->SaveAsync();
    }
    catch (...)
    {
    }
};
#pragma optimize("", on)

void App::RemoveSecondaryWindow(_In_ WindowFrameService ^ frameService)
{
    // Shell does not allow killing the main window.
    if (m_mainViewId != frameService->GetViewId())
    {
        RemoveWindowFromMap(frameService->GetViewId());
    }
}

Frame ^ App::CreateFrame()
{
    auto frame = ref new Frame();
    frame->FlowDirection = LocalizationService::GetInstance()->GetFlowDirection();

    return frame;
}

/// <summary>
/// Invoked when the application is launched normally by the end user. Other entry points
/// will be used when the application is launched to open a specific file, to display
/// search results, and so forth.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs ^ args)
{
    if (args->PrelaunchActivated)
    {
        // If the app got pre-launch activated, then save that state in a flag
        m_preLaunched = true;
    }
    OnAppLaunch(args, args->Arguments);
}

void App::OnAppLaunch(IActivatedEventArgs ^ args, String ^ argument)
{

    // Uncomment the following lines to display frame-rate and per-frame CPU usage info.
    //#if _DEBUG
    //    if (IsDebuggerPresent())
    //    {
    //        DebugSettings->EnableFrameRateCounter = true;
    //    }
    //#endif

    args->SplashScreen->Dismissed += ref new TypedEventHandler<SplashScreen ^, Object ^>(this, &App::DismissedEventHandler);

    auto rootFrame = dynamic_cast<Frame ^>(Window::Current->Content);
    WeakReference weak(this);

    float minWindowWidth = static_cast<float>(static_cast<double>(this->Resources->Lookup(ApplicationResourceKeys::AppMinWindowWidth)));
    float minWindowHeight = static_cast<float>(static_cast<double>(this->Resources->Lookup(ApplicationResourceKeys::AppMinWindowHeight)));
    Size minWindowSize = SizeHelper::FromDimensions(minWindowWidth, minWindowHeight);

    ApplicationView ^ appView = ApplicationView::GetForCurrentView();
    ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
    // For very first launch, set the size of the calc as size of the default standard mode
    if (!localSettings->Values->HasKey(L"VeryFirstLaunch"))
    {
        localSettings->Values->Insert(ref new String(L"VeryFirstLaunch"), false);
        appView->SetPreferredMinSize(minWindowSize);
        appView->TryResizeView(minWindowSize);
    }
    else
    {
        appView->PreferredLaunchWindowingMode = ApplicationViewWindowingMode::Auto;
    }

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (rootFrame == nullptr)
    {
        if (!Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons")) // PC Family
        {
            // Disable the system view activation policy during the first launch of the app
            // only for PC family devices and not for phone family devices
            try
            {
                ApplicationViewSwitcher::DisableSystemViewActivationPolicy();
            }
            catch (Exception ^ e)
            {
                // Log that DisableSystemViewActionPolicy didn't work
            }
        }

        // Create a Frame to act as the navigation context
        rootFrame = App::CreateFrame();

        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a navigation
        // parameter
        if (!rootFrame->Navigate(MainPage::typeid, argument))
        {
            // We couldn't navigate to the main page, kill the app so we have a good
            // stack to debug
            throw std::bad_exception();
        }

        SetMinWindowSizeAndActivate(rootFrame, minWindowSize);
        m_mainViewId = ApplicationView::GetForCurrentView()->Id;
        AddWindowToMap(WindowFrameService::CreateNewWindowFrameService(rootFrame, false, weak));
    }
    else
    {
        // For first launch, LaunchStart is logged in constructor, this is for subsequent launches.

        // !Phone check is required because even in continuum mode user interaction mode is Mouse not Touch
        if ((UIViewSettings::GetForCurrentView()->UserInteractionMode == UserInteractionMode::Mouse)
            && (!Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons")))
        {
            // If the pre-launch hasn't happened then allow for the new window/view creation
            if (!m_preLaunched)
            {
                auto newCoreAppView = CoreApplication::CreateNewView();
                newCoreAppView->Dispatcher->RunAsync(
                    CoreDispatcherPriority::Normal, ref new DispatchedHandler([args, argument, minWindowSize, weak]() {
                        auto that = weak.Resolve<App>();
                        if (that != nullptr)
                        {
                            auto rootFrame = App::CreateFrame();
                            SetMinWindowSizeAndActivate(rootFrame, minWindowSize);

                            if (!rootFrame->Navigate(MainPage::typeid, argument))
                            {
                                // We couldn't navigate to the main page, kill the app so we have a good
                                // stack to debug
                                throw std::bad_exception();
                            }

                            auto frameService = WindowFrameService::CreateNewWindowFrameService(rootFrame, true, weak);
                            that->AddWindowToMap(frameService);

                            auto dispatcher = CoreWindow::GetForCurrentThread()->Dispatcher;
                            auto safeFrameServiceCreation = std::make_shared<SafeFrameWindowCreation>(frameService, that);
                            int newWindowId = ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread());

                            ActivationViewSwitcher ^ activationViewSwitcher;
                            auto activateEventArgs = dynamic_cast<IViewSwitcherProvider ^>(args);
                            if (activateEventArgs != nullptr)
                            {
                                activationViewSwitcher = activateEventArgs->ViewSwitcher;
                            }

                            if (activationViewSwitcher != nullptr)
                            {
                                activationViewSwitcher->ShowAsStandaloneAsync(newWindowId, ViewSizePreference::Default);
                                safeFrameServiceCreation->SetOperationSuccess(true);
                            }
                            else
                            {
                                auto activatedEventArgs = dynamic_cast<IApplicationViewActivatedEventArgs ^>(args);
                                if ((activatedEventArgs != nullptr) && (activatedEventArgs->CurrentlyShownApplicationViewId != 0))
                                {
                                    create_task(ApplicationViewSwitcher::TryShowAsStandaloneAsync(
                                                    frameService->GetViewId(),
                                                    ViewSizePreference::Default,
                                                    activatedEventArgs->CurrentlyShownApplicationViewId,
                                                    ViewSizePreference::Default))
                                        .then(
                                            [safeFrameServiceCreation](bool viewShown) {
                                                // SafeFrameServiceCreation is used to automatically remove the frame
                                                // from the list of frames if something goes bad.
                                                safeFrameServiceCreation->SetOperationSuccess(viewShown);
                                            },
                                            task_continuation_context::use_current());
                                }
                            }
                        }
                    }));
            }
            else
            {
                ActivationViewSwitcher ^ activationViewSwitcher;
                auto activateEventArgs = dynamic_cast<IViewSwitcherProvider ^>(args);
                if (activateEventArgs != nullptr)
                {
                    activationViewSwitcher = activateEventArgs->ViewSwitcher;
                }

                if (activationViewSwitcher != nullptr)
                {
                    activationViewSwitcher->ShowAsStandaloneAsync(
                        ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()), ViewSizePreference::Default);
                }
                else
                {
                    TraceLogger::GetInstance()->LogError(ViewMode::None, L"App::OnAppLaunch", L"Null_ActivationViewSwitcher");
                }
            }
            // Set the preLaunched flag to false
            m_preLaunched = false;
        }
        else // for touch devices
        {
            if (rootFrame->Content == nullptr)
            {
                // When the navigation stack isn't restored navigate to the first page,
                // configuring the new page by passing required information as a navigation
                // parameter
                if (!rootFrame->Navigate(MainPage::typeid, argument))
                {
                    // We couldn't navigate to the main page,
                    // kill the app so we have a good stack to debug
                    throw std::bad_exception();
                }
            }
            if (ApplicationView::GetForCurrentView()->ViewMode != ApplicationViewMode::CompactOverlay)
            {
                if (!Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
                {
                    // for tablet mode: since system view activation policy is disabled so do ShowAsStandaloneAsync if activationViewSwitcher exists in
                    // activationArgs
                    ActivationViewSwitcher ^ activationViewSwitcher;
                    auto activateEventArgs = dynamic_cast<IViewSwitcherProvider ^>(args);
                    if (activateEventArgs != nullptr)
                    {
                        activationViewSwitcher = activateEventArgs->ViewSwitcher;
                    }
                    if (activationViewSwitcher != nullptr)
                    {
                        auto viewId = safe_cast<IApplicationViewActivatedEventArgs ^>(args)->CurrentlyShownApplicationViewId;
                        if (viewId != 0)
                        {
                            activationViewSwitcher->ShowAsStandaloneAsync(viewId);
                        }
                    }
                }
                // Ensure the current window is active
                Window::Current->Activate();
            }
        }
    }
}

void App::SetMinWindowSizeAndActivate(Frame ^ rootFrame, Size minWindowSize)
{
    // SetPreferredMinSize should always be called before Window::Activate
    ApplicationView ^ appView = ApplicationView::GetForCurrentView();
    appView->SetPreferredMinSize(minWindowSize);

    // Place the frame in the current Window
    Window::Current->Content = rootFrame;
    Window::Current->Activate();
}

void App::RegisterDependencyProperties()
{
    NarratorNotifier::RegisterDependencyProperties();
}

void App::OnActivated(IActivatedEventArgs ^ args)
{
    if (args->Kind == ActivationKind::Protocol)
    {
        // We currently don't pass the uri as an argument,
        // and handle any protocol launch as a normal app launch.
        OnAppLaunch(args, nullptr);
    }
}

void CalculatorApp::App::OnSuspending(Object ^ sender, SuspendingEventArgs ^ args)
{
    TraceLogger::GetInstance()->LogButtonUsage();
}

void App::DismissedEventHandler(SplashScreen ^ sender, Object ^ e)
{
    SetupJumpList();
}



