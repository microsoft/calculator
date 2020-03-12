// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "WindowFrameService.h"
#include "Common/KeyboardShortcutManager.h"
#include "CalcViewModel/Common/TraceLogger.h"

using namespace concurrency;
using namespace Platform;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace std;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;

namespace CalculatorApp
{
    WindowFrameService ^ WindowFrameService::CreateNewWindowFrameService(_In_ Frame ^ viewFrame, bool createdByUs, Platform::WeakReference parent)
    {
        assert(CoreWindow::GetForCurrentThread() != nullptr);
        auto frameService = ref new WindowFrameService(viewFrame, parent);
        frameService->InitializeFrameService(createdByUs);
        return frameService;
    }

    WindowFrameService::WindowFrameService(_In_ Frame ^ frame, WeakReference parent)
        : m_currentWindow(CoreWindow::GetForCurrentThread())
        , m_coreDispatcher(m_currentWindow->Dispatcher)
        , m_frame(frame)
        , m_parent(parent)
        , m_viewId(ApplicationView::GetApplicationViewIdForWindow(m_currentWindow.Get()))
    {
    }

    void WindowFrameService::InitializeFrameService(bool createdByUs)
    {
        assert(createdByUs == (!CoreApplication::GetCurrentView()->IsHosted && !CoreApplication::GetCurrentView()->IsMain));
        if (createdByUs)
        {
            ApplicationView::GetForCurrentView()->Consolidated +=
                ref new TypedEventHandler<ApplicationView ^, ApplicationViewConsolidatedEventArgs ^>(this, &WindowFrameService::OnConsolidated);
        }
        else
        {
            CoreWindow::GetForCurrentThread()->Closed += ref new TypedEventHandler<CoreWindow ^, CoreWindowEventArgs ^>(this, &WindowFrameService::OnClosed);
        }
    }

    CoreDispatcher ^ WindowFrameService::GetCoreDispatcher()
    {
        return m_coreDispatcher.Get();
    }

    int WindowFrameService::GetViewId()
    {
        return m_viewId;
    }

    Frame ^ WindowFrameService::GetFrame()
    {
        return m_frame;
    }

    Page ^ WindowFrameService::GetCurrentPage()
    {
        return dynamic_cast<Page ^>(m_frame->Content);
    }

    void WindowFrameService::SetNewFrame(_In_ Windows::UI::Xaml::Controls::Frame ^ frame)
    {
        assert(frame->BackStackDepth == 0);
        m_frame = frame;
    }

    void WindowFrameService::RegisterOnWindowClosingHandler(function<void()> onWindowClosingHandler)
    {
        m_onWindowClosingHandlers.push_back(onWindowClosingHandler);
    }

    void WindowFrameService::InvokeWindowClosingHandlers()
    {
        // Should be called only once just before we kill the window.
        for (auto& handler : m_onWindowClosingHandlers)
        {
            handler();
        }
        m_onWindowClosingHandlers.clear();
    }

    task<void> WindowFrameService::HandleViewRelease()
    {
        auto that(this);
        task_completion_event<void> closingHandlersCompletedEvent;
        m_coreDispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler([that, closingHandlersCompletedEvent]() {
                                       KeyboardShortcutManager::OnWindowClosed(that->m_viewId);
                                       Window::Current->Content = nullptr;
                                       that->InvokeWindowClosingHandlers();
                                       // This is to ensure InvokeWindowClosingHandlers is be done before RemoveWindowFromMap
                                       // If InvokeWindowClosingHandlers throws any exception we want it to crash the application
                                       // so we are OK not setting closingHandlersCompletedEvent in that case
                                       closingHandlersCompletedEvent.set();
                                       that->m_coreDispatcher->StopProcessEvents();
                                       Window::Current->Close();
                                   }));
        return create_task(closingHandlersCompletedEvent);
    }

    void WindowFrameService::OnConsolidated(_In_ ApplicationView ^ sender, _In_ ApplicationViewConsolidatedEventArgs ^ e)
    {
        TraceLogger::GetInstance()->DecreaseWindowCount();
        auto parent = m_parent.Resolve<App>();
        if (parent != nullptr)
        {
            parent->RemoveWindow(this);
        }
    }

    void WindowFrameService::OnClosed(_In_ CoreWindow ^ sender, _In_ CoreWindowEventArgs ^ args)
    {
        auto parent = m_parent.Resolve<App>();
        if (parent != nullptr)
        {
            parent->RemoveSecondaryWindow(this);
        }
    }

    void WindowFrameService::RegisterRuntimeWindowService(TypeName serviceId, _In_opt_ Object ^ service)
    {
        if (TryResolveRuntimeWindowService(serviceId))
        {
            throw ref new InvalidArgumentException(serviceId + L" already registered");
        }

        m_runtimeServicesMap[serviceId.Name] = service;
    }

    bool WindowFrameService::RemoveRuntimeWindowService(TypeName serviceId)
    {
        return m_runtimeServicesMap.erase(serviceId.Name) > 0;
    }

    Object ^ WindowFrameService::ResolveRuntimeWindowService(TypeName serviceId)
    {
        auto service = TryResolveRuntimeWindowService(serviceId);

        if (service)
        {
            return service;
        }
        else
        {
            throw ref new InvalidArgumentException(serviceId.Name + L" not found");
        }
    }

    _Ret_maybenull_ Object ^ WindowFrameService::TryResolveRuntimeWindowService(TypeName serviceId)
    {
        auto entry = m_runtimeServicesMap.find(serviceId.Name);

        if (entry != m_runtimeServicesMap.end())
        {
            return entry->second;
        }

        return nullptr;
    }
}
