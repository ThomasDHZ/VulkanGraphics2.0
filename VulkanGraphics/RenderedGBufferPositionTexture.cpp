#include "RenderedGBufferPositionTexture.h"
#include "ImGui/imgui_impl_vulkan.h"

RenderedGBufferPositionTexture::RenderedGBufferPositionTexture() : Texture()
{
}

RenderedGBufferPositionTexture::RenderedGBufferPositionTexture(VulkanEngine& engine) : Texture(engine, TextureType::vkRenderedTexture)
{
    CreateTextureImage(engine);
    CreateTextureView(engine);
    CreateTextureSampler(engine);
    ImGui_ImplVulkan_AddTexture(ImGuiDescriptorSet, Sampler, View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

RenderedGBufferPositionTexture::~RenderedGBufferPositionTexture()
{
}

void RenderedGBufferPositionTexture::CreateTextureImage(VulkanEngine& engine)
{
    VkImageCreateInfo TextureInfo = {};
    TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    TextureInfo.imageType = VK_IMAGE_TYPE_2D;
    TextureInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    TextureInfo.extent.width = engine.SwapChain.GetSwapChainResolution().width;
    TextureInfo.extent.height = engine.SwapChain.GetSwapChainResolution().height;
    TextureInfo.extent.depth = 1;
    TextureInfo.mipLevels = 1;
    TextureInfo.arrayLayers = 1;
    TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    TextureInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    Texture::CreateTextureImage(engine, TextureInfo);
}

void RenderedGBufferPositionTexture::CreateTextureView(VulkanEngine& engine)
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

    View = engine.CreateTextureView(TextureImageViewInfo);
}

void RenderedGBufferPositionTexture::CreateTextureSampler(VulkanEngine& engine)
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

    Sampler = engine.CreateTextureSampler(TextureImageSamplerInfo);
}

void RenderedGBufferPositionTexture::RecreateRendererTexture(VulkanEngine& engine)
{
    Texture::Delete(engine);
    CreateTextureImage(engine);
    CreateTextureView(engine);
    CreateTextureSampler(engine);
    ImGui_ImplVulkan_AddTexture(ImGuiDescriptorSet, Sampler, View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}