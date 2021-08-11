#include "RenderedGBufferPositionTexture.h"
#include "ImGui/imgui_impl_vulkan.h"

RenderedGBufferPositionTexture::RenderedGBufferPositionTexture() : Texture()
{
}

RenderedGBufferPositionTexture::RenderedGBufferPositionTexture(std::shared_ptr<VulkanEngine> engine, VkImageLayout imageLayout) : Texture(TextureType::vkRenderedTexture, imageLayout)
{
    Width = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width;
    Height = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height;

    CreateTextureImage();
    CreateTextureView();
    CreateTextureSampler();
    ImGui_ImplVulkan_AddTexture(ImGuiDescriptorSet, Sampler, View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

RenderedGBufferPositionTexture::RenderedGBufferPositionTexture(glm::vec2& TextureResolution, VkImageLayout imageLayout) : Texture(TextureResolution, TextureType::vkRenderedTexture, imageLayout)
{
    CreateTextureImage();
    CreateTextureView();
    CreateTextureSampler();
    ImGui_ImplVulkan_AddTexture(ImGuiDescriptorSet, Sampler, View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

RenderedGBufferPositionTexture::RenderedGBufferPositionTexture(int width, int height, VkImageLayout imageLayout) : Texture(width, height, TextureType::vkRenderedTexture, imageLayout)
{
    CreateTextureImage();
    CreateTextureView();
    CreateTextureSampler();
    ImGui_ImplVulkan_AddTexture(ImGuiDescriptorSet, Sampler, View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

RenderedGBufferPositionTexture::~RenderedGBufferPositionTexture()
{
}

void RenderedGBufferPositionTexture::CreateTextureImage()
{
    VkImageCreateInfo TextureInfo = {};
    TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    TextureInfo.imageType = VK_IMAGE_TYPE_2D;
    TextureInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    TextureInfo.extent.width = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width;
    TextureInfo.extent.height = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height;
    TextureInfo.extent.depth = 1;
    TextureInfo.mipLevels = 1;
    TextureInfo.arrayLayers = 1;
    TextureInfo.initialLayout = ImageLayout;
    TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    TextureInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    Texture::CreateTextureImage(TextureInfo);
}

void RenderedGBufferPositionTexture::CreateTextureView()
{
    VkImageViewCreateInfo TextureImageViewInfo = {};
    TextureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    TextureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    TextureImageViewInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    TextureImageViewInfo.subresourceRange = {};
    TextureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    TextureImageViewInfo.subresourceRange.baseMipLevel = 0;
    TextureImageViewInfo.subresourceRange.levelCount = 1;
    TextureImageViewInfo.subresourceRange.baseArrayLayer = 0;
    TextureImageViewInfo.subresourceRange.layerCount = 1;
    TextureImageViewInfo.image = Image;

    if (vkCreateImageView(VulkanPtr::GetDevice(), &TextureImageViewInfo, nullptr, &View)) {
        throw std::runtime_error("Failed to create Image View.");
    }
}

void RenderedGBufferPositionTexture::CreateTextureSampler()
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

    if (vkCreateSampler(VulkanPtr::GetDevice(), &TextureImageSamplerInfo, nullptr, &Sampler))
    {
        throw std::runtime_error("Failed to create Sampler.");
    }
}

void RenderedGBufferPositionTexture::RecreateRendererTexture()
{
    Width = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width;
    Height = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height;

    Texture::Delete();
    CreateTextureImage();
    CreateTextureView();
    CreateTextureSampler();
    ImGui_ImplVulkan_AddTexture(ImGuiDescriptorSet, Sampler, View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}