#include "RenderedRayTracedColorTexture.h"
#include "ImGui/imgui_impl_vulkan.h"

RenderedRayTracedColorTexture::RenderedRayTracedColorTexture() : Texture2D()
{
}

RenderedRayTracedColorTexture::RenderedRayTracedColorTexture(std::shared_ptr<VulkanEngine> engine) : Texture2D(engine, TextureType::vkRenderedTexture)
{
    CreateTextureImage(engine);
    CreateTextureView(engine);
    CreateTextureSampler(engine);
    SendTextureToGPU(engine);
    ImGui_ImplVulkan_AddTexture(ImGuiDescriptorSet, Sampler, View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

RenderedRayTracedColorTexture::~RenderedRayTracedColorTexture()
{
}

void RenderedRayTracedColorTexture::CreateTextureImage(std::shared_ptr<VulkanEngine> engine)
{
    VkImageCreateInfo TextureInfo = {};
    TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    TextureInfo.imageType = VK_IMAGE_TYPE_2D;
    TextureInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
    TextureInfo.extent.width = engine->SwapChain.GetSwapChainResolution().width;
    TextureInfo.extent.height = engine->SwapChain.GetSwapChainResolution().height;
    TextureInfo.extent.depth = 1;
    TextureInfo.mipLevels = 1;
    TextureInfo.arrayLayers = 1;
    TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    Texture::CreateTextureImage(engine, TextureInfo);
}

void RenderedRayTracedColorTexture::CreateTextureView(std::shared_ptr<VulkanEngine> engine)
{
    VkImageViewCreateInfo TextureImageViewInfo = {};
    TextureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    TextureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    TextureImageViewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
    TextureImageViewInfo.subresourceRange = {};
    TextureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    TextureImageViewInfo.subresourceRange.baseMipLevel = 0;
    TextureImageViewInfo.subresourceRange.levelCount = 1;
    TextureImageViewInfo.subresourceRange.baseArrayLayer = 0;
    TextureImageViewInfo.subresourceRange.layerCount = 1;
    TextureImageViewInfo.image = Image;

    if (vkCreateImageView(engine->Device, &TextureImageViewInfo, nullptr, &View)) {
        throw std::runtime_error("Failed to create Image View.");
    }
}

void RenderedRayTracedColorTexture::CreateTextureSampler(std::shared_ptr<VulkanEngine> engine)
{
    VkSamplerCreateInfo TextureImageSamplerInfo = {};
    TextureImageSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    TextureImageSamplerInfo.magFilter = VK_FILTER_LINEAR;
    TextureImageSamplerInfo.minFilter = VK_FILTER_LINEAR;
    TextureImageSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    TextureImageSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    TextureImageSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    TextureImageSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    TextureImageSamplerInfo.mipLodBias = 0.0f;
    TextureImageSamplerInfo.maxAnisotropy = 1.0f;
    TextureImageSamplerInfo.minLod = 0.0f;
    TextureImageSamplerInfo.maxLod = 1.0f;
    TextureImageSamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    if (vkCreateSampler(engine->Device, &TextureImageSamplerInfo, nullptr, &Sampler))
    {
        throw std::runtime_error("Failed to create Sampler.");
    }
}

void RenderedRayTracedColorTexture::SendTextureToGPU(std::shared_ptr<VulkanEngine> engine)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = engine->CommandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;
    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(engine->Device, &commandBufferAllocateInfo, &cmdBuffer);

    VkCommandBufferBeginInfo cmdBufferBeginInfo{};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
    UpdateImageLayout(engine, cmdBuffer, VK_IMAGE_LAYOUT_GENERAL);
    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence;
    vkCreateFence(engine->Device, &fenceCreateInfo, nullptr, &fence);

    vkQueueSubmit(engine->GraphicsQueue, 1, &submitInfo, fence);

    vkWaitForFences(engine->Device, 1, &fence, VK_TRUE, INT64_MAX);
    vkDestroyFence(engine->Device, fence, nullptr);

    vkFreeCommandBuffers(engine->Device, engine->CommandPool, 1, &cmdBuffer);

    UpdateImageLayout(engine, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void RenderedRayTracedColorTexture::RecreateRendererTexture(std::shared_ptr<VulkanEngine> engine)
{
    Texture::Delete(engine);
    CreateTextureImage(engine);
    CreateTextureView(engine);
    CreateTextureSampler(engine);
    UpdateImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    ImGui_ImplVulkan_AddTexture(ImGuiDescriptorSet, Sampler, View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}