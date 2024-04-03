// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "RenderMain.h"
#include "DirectXHelper.h"

namespace ctrls = Windows::UI::Xaml::Controls;

namespace
{
    constexpr unsigned int s_RedChannelIndex = 0;
    constexpr unsigned int s_GreenChannelIndex = 1;
    constexpr unsigned int s_BlueChannelIndex = 2;
    constexpr unsigned int s_AlphaChannelIndex = 3;
    constexpr float s_MaxChannelValue = 255.0f;
    constexpr float nearestPointRadius = 3;

    struct RenderMainHelper
    {
        static std::thread CreateRenderWorker(GraphControl::DX::RenderMain::RenderContext& context, ctrls::SwapChainPanel ^ panel, Graphing::IGraph* graph)
        {
            context.Graph = graph;
            return std::thread{ [&]
                                {
                                    struct DeviceNotify : GraphControl::DX::IDeviceNotify
                                    {
                                        explicit DeviceNotify(GraphControl::DX::RenderMain::RenderContext& context)
                                            : Context(context)
                                        {
                                        }
                                        void OnDeviceLost() override
                                        {
                                        }
                                        void OnDeviceRestored() override
                                        {
                                        }

                                        GraphControl::DX::RenderMain::RenderContext& Context;
                                    };
                                    DeviceNotify notify{ context };
                                    context.DxRes = std::make_unique<GraphControl::DX::DeviceResources>(panel, &notify);
                                    std::unique_lock lck{ context.Mtx };
                                    while (!context.Quit)
                                    {
                                        if (!context.Tasks.empty())
                                        {
                                            auto task = std::move(context.Tasks.front());
                                            context.Tasks.pop();
                                            lck.unlock();
                                            task(context);
                                            lck.lock();
                                        }
                                        else
                                        {
                                            context.Cv.wait(lck);
                                        }
                                    }
                                } };
        }

        static void QuitWorker(GraphControl::DX::RenderMain& self)
        {
            {
                std::scoped_lock lck{ self.m_renderContext.Mtx };
                self.m_renderContext.Quit = true;
            }
            self.m_renderContext.Cv.notify_one();
        }

        template <class F>
        static void SubmitTask(const GraphControl::DX::RenderMain& self, F&& task)
        {
            {
                std::scoped_lock lck{ self.m_renderContext.Mtx };
                self.m_renderContext.Tasks.push(std::forward<F>(task));
            }
            self.m_renderContext.Cv.notify_one();
        }
    };
    using Helper = RenderMainHelper;
}

namespace GraphControl::DX
{
    RenderMain::RenderMain(ctrls::SwapChainPanel ^ panel, Graphing::IGraph* graph)
        : m_renderWorker(Helper::CreateRenderWorker(m_renderContext, panel, graph))
    {
    }

    void RenderMain::FireRenderPass() const
    {
        Helper::SubmitTask(
            *this,
            [](RenderContext& renderContext)
            {
                auto dx = renderContext.DxRes.get();
                auto d3dctx = dx->GetD3DDeviceContext();
                d3dctx->ClearRenderTargetView(dx->GetBackBufferRenderTargetView(), renderContext.BkgColor);
            });
    }

    concurrency::task<bool> RenderMain::RunRenderPassAsync() const
    {
        co_return true;
    }

    RenderMain::~RenderMain()
    {
        Helper::QuitWorker(*this);
        m_renderWorker.join();
    }
}
