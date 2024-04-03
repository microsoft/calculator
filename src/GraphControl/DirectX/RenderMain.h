// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

// Taken from the default template for Xaml and Direct3D 11 apps.
#include "DeviceResources.h"
#include "NearestPointRenderer.h"
#include "IGraph.h"

namespace
{
    struct RenderMainHelper;
}

// Renders Direct2D and 3D content on the screen.
namespace GraphControl::DX
{
    class RenderMain
    {
        friend struct RenderMainHelper;

    public:
        explicit RenderMain(Windows::UI::Xaml::Controls::SwapChainPanel ^ panel, Graphing::IGraph* graph);
        ~RenderMain();

        void FireRenderPass() const;
        concurrency::task<bool> RunRenderPassAsync() const;

        bool ActiveTracing() const noexcept
        {
            return m_renderContext.ActiveTracing;
        }
        Windows::Foundation::Point ActiveTraceCursorPosition() const noexcept
        {
            return m_renderContext.ActiveTraceCursorPosition;
        }
        Windows::Foundation::Point TraceLocation() const noexcept
        {
            std::scoped_lock lck{ m_renderContext.Mtx };
            return m_renderContext.TraceLocation;
        }

        void ActiveTracing(bool value) noexcept;
        void ActiveTraceCursorPosition(const Windows::Foundation::Point& value) noexcept;
        void BackgroundColor(const Windows::UI::Color& value) noexcept;
        void PointerLocation(const Windows::Foundation::Point& value) noexcept;
        void PointRadius(float radius) noexcept;

    private:
        struct RenderContext
        {
            std::unique_ptr<DeviceResources> DxRes;
            Graphing::IGraph* Graph;
            mutable std::mutex Mtx;
            mutable std::condition_variable Cv;
            mutable std::queue<std::function<void(RenderContext& context)>> Tasks;
            Windows::Foundation::Point ActiveTraceCursorPosition = {};
            Windows::Foundation::Point TraceLocation = {};
            float BkgColor[4] = {};
            bool ActiveTracing = false;
            bool Quit = false;
        };

    private:
        RenderContext m_renderContext;
        std::thread m_renderWorker;
    };
}
