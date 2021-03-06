// Copyright 2018 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <array>
#include <memory>
#include <tuple>

#include "video_core/renderer_vulkan/declarations.h"
#include "video_core/renderer_vulkan/vk_memory_manager.h"
#include "video_core/renderer_vulkan/vk_resource_manager.h"

namespace Core {
class System;
}

namespace Core::Frontend {
class EmuWindow;
}

namespace Tegra {
struct FramebufferConfig;
}

namespace VideoCore {
class RasterizerInterface;
}

namespace Vulkan {

struct ScreenInfo;
class RasterizerVulkan;
class VKDevice;
class VKFence;
class VKImage;
class VKScheduler;
class VKSwapchain;

class VKBlitScreen final {
public:
    explicit VKBlitScreen(Core::System& system, Core::Frontend::EmuWindow& render_window,
                          VideoCore::RasterizerInterface& rasterizer, const VKDevice& device,
                          VKResourceManager& resource_manager, VKMemoryManager& memory_manager,
                          VKSwapchain& swapchain, VKScheduler& scheduler,
                          const VKScreenInfo& screen_info);
    ~VKBlitScreen();

    void Recreate();

    std::tuple<VKFence&, vk::Semaphore> Draw(const Tegra::FramebufferConfig& framebuffer,
                                             bool use_accelerated);

private:
    struct BufferData;

    void CreateStaticResources();
    void CreateShaders();
    void CreateSemaphores();
    void CreateDescriptorPool();
    void CreateRenderPass();
    void CreateDescriptorSetLayout();
    void CreateDescriptorSets();
    void CreatePipelineLayout();
    void CreateGraphicsPipeline();
    void CreateSampler();

    void CreateDynamicResources();
    void CreateFramebuffers();

    void RefreshResources(const Tegra::FramebufferConfig& framebuffer);
    void ReleaseRawImages();
    void CreateStagingBuffer(const Tegra::FramebufferConfig& framebuffer);
    void CreateRawImages(const Tegra::FramebufferConfig& framebuffer);

    void UpdateDescriptorSet(std::size_t image_index, vk::ImageView image_view) const;
    void SetUniformData(BufferData& data, const Tegra::FramebufferConfig& framebuffer) const;
    void SetVertexData(BufferData& data, const Tegra::FramebufferConfig& framebuffer) const;

    u64 CalculateBufferSize(const Tegra::FramebufferConfig& framebuffer) const;
    u64 GetRawImageOffset(const Tegra::FramebufferConfig& framebuffer,
                          std::size_t image_index) const;

    Core::System& system;
    Core::Frontend::EmuWindow& render_window;
    VideoCore::RasterizerInterface& rasterizer;
    const VKDevice& device;
    VKResourceManager& resource_manager;
    VKMemoryManager& memory_manager;
    VKSwapchain& swapchain;
    VKScheduler& scheduler;
    const std::size_t image_count;
    const VKScreenInfo& screen_info;

    UniqueShaderModule vertex_shader;
    UniqueShaderModule fragment_shader;
    UniqueDescriptorPool descriptor_pool;
    UniqueDescriptorSetLayout descriptor_set_layout;
    UniquePipelineLayout pipeline_layout;
    UniquePipeline pipeline;
    UniqueRenderPass renderpass;
    std::vector<UniqueFramebuffer> framebuffers;
    std::vector<vk::DescriptorSet> descriptor_sets;
    UniqueSampler sampler;

    UniqueBuffer buffer;
    VKMemoryCommit buffer_commit;

    std::vector<std::unique_ptr<VKFenceWatch>> watches;

    std::vector<UniqueSemaphore> semaphores;
    std::vector<std::unique_ptr<VKImage>> raw_images;
    std::vector<VKMemoryCommit> raw_buffer_commits;
    u32 raw_width = 0;
    u32 raw_height = 0;
};

} // namespace Vulkan
