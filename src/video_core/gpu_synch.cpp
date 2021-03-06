// Copyright 2019 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "video_core/gpu_synch.h"
#include "video_core/renderer_base.h"

namespace VideoCommon {

GPUSynch::GPUSynch(Core::System& system, std::unique_ptr<VideoCore::RendererBase>&& renderer,
                   std::unique_ptr<Core::Frontend::GraphicsContext>&& context)
    : GPU(system, std::move(renderer), false), context{std::move(context)} {}

GPUSynch::~GPUSynch() = default;

void GPUSynch::Start() {
    context->MakeCurrent();
}

void GPUSynch::PushGPUEntries(Tegra::CommandList&& entries) {
    dma_pusher->Push(std::move(entries));
    dma_pusher->DispatchCalls();
}

void GPUSynch::SwapBuffers(const Tegra::FramebufferConfig* framebuffer) {
    renderer->SwapBuffers(framebuffer);
}

void GPUSynch::FlushRegion(CacheAddr addr, u64 size) {
    renderer->Rasterizer().FlushRegion(addr, size);
}

void GPUSynch::InvalidateRegion(CacheAddr addr, u64 size) {
    renderer->Rasterizer().InvalidateRegion(addr, size);
}

void GPUSynch::FlushAndInvalidateRegion(CacheAddr addr, u64 size) {
    renderer->Rasterizer().FlushAndInvalidateRegion(addr, size);
}

} // namespace VideoCommon
