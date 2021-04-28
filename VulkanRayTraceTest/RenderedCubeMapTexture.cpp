#include "RenderedCubeMapTexture.h"

RenderedCubeMapTexture::RenderedCubeMapTexture() : Texture()
{
}

RenderedCubeMapTexture::RenderedCubeMapTexture(VulkanEngine& engine) : Texture(engine, TextureType::vkRenderedCubeMap, VK_IMAGE_LAYOUT_UNDEFINED)
{
    Width = engine.SwapChain.GetSwapChainResolution().width;
    Height = engine.SwapChain.GetSwapChainResolution().height;

    CreateTextureImage(engine);
    CreateTextureView(engine);
    CreateTextureSampler(engine);
}

RenderedCubeMapTexture::RenderedCubeMapTexture(VulkanEngine& engine, glm::vec2 TextureResolution) : Texture(engine, TextureResolution, TextureType::vkRenderedCubeMap, VK_IMAGE_LAYOUT_UNDEFINED)
{
    CreateTextureImage(engine);
    CreateTextureView(engine);
    CreateTextureSampler(engine);
}

RenderedCubeMapTexture::RenderedCubeMapTexture(VulkanEngine& engine, int width, int height) : Texture(engine, width, height, TextureType::vkRenderedCubeMap, VK_IMAGE_LAYOUT_UNDEFINED)
{
    CreateTextureImage(engine);
    CreateTextureView(engine);
    CreateTextureSampler(engine);
}

RenderedCubeMapTexture::~RenderedCubeMapTexture()
{
}

void RenderedCubeMapTexture::CreateTextureImage(VulkanEngine& engine)
{
    VkImageCreateInfo TextureInfo = {};
    TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    TextureInfo.imageType = VK_IMAGE_TYPE_2D;
    TextureInfo.extent.width = 2048;
    TextureInfo.extent.height = 2048;
    TextureInfo.extent.depth = 1;
    TextureInfo.mipLevels = 1;
    TextureInfo.arrayLayers = 6;
    TextureInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    TextureInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    TextureInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    Texture::CreateTextureImage(engine, TextureInfo);
}

void RenderedCubeMapTexture::CreateTextureView(VulkanEngine& engine)
{
    VkImageViewCreateInfo TextureImageViewInfo = {};
    TextureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    TextureImageViewInfo.image = Image;
    TextureImageViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    TextureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    TextureImageViewInfo.subresourceRange.baseMipLevel = 0;
    TextureImageViewInfo.subresourceRange.levelCount = 1;
    TextureImageViewInfo.subresourceRange.baseArrayLayer = 0;
    TextureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    TextureImageViewInfo.subresourceRange.layerCount = 6;

    if (vkCreateImageView(engine.Device, &TextureImageViewInfo, nullptr, &View)) {
        throw std::runtime_error("Failed to create Image View.");
    }
}

void RenderedCubeMapTexture::CreateTextureSampler(VulkanEngine& engine)
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
    TextureImageSamplerInfo.maxAnisotropy = 16.0f;
    TextureImageSamplerInfo.minLod = 0.0f;
    TextureImageSamplerInfo.maxLod = 1.0f;
    TextureImageSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
    TextureImageSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    TextureImageSamplerInfo.anisotropyEnable = VK_TRUE;

    if (vkCreateSampler(engine.Device, &TextureImageSamplerInfo, nullptr, &Sampler))
    {
        throw std::runtime_error("Failed to create Sampler.");
    }
}

void RenderedCubeMapTexture::RecreateRendererTexture(VulkanEngine& engine)
{
    Texture::Delete(engine);
    CreateTextureImage(engine);
    CreateTextureView(engine);
    CreateTextureSampler(engine);
}