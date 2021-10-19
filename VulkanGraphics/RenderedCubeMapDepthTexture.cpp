#include "RenderedCubeMapDepthTexture.h"

RenderedCubeMapDepthTexture::RenderedCubeMapDepthTexture() : Texture()
{
}

RenderedCubeMapDepthTexture::RenderedCubeMapDepthTexture(std::shared_ptr<VulkanEngine> engine, uint32_t mipLevels) : Texture(TextureType::vkRenderedCubeMap)
{
    Width = engine->SwapChain.GetSwapChainResolution().width;
    Height = engine->SwapChain.GetSwapChainResolution().height;
    MipMapLevels = mipLevels;

    CreateTextureImage();
    CreateTextureView();
    CreateTextureSampler();
}

RenderedCubeMapDepthTexture::RenderedCubeMapDepthTexture(glm::ivec2 TextureResolution, uint32_t mipLevels) : Texture(TextureResolution, TextureType::vkRenderedCubeMap)
{
    MipMapLevels = mipLevels;

    CreateTextureImage();
    CreateTextureView();
    CreateTextureSampler();
}

RenderedCubeMapDepthTexture::~RenderedCubeMapDepthTexture()
{
}

void RenderedCubeMapDepthTexture::CreateTextureImage()
{
    VkImageCreateInfo TextureInfo = {};
    TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    TextureInfo.imageType = VK_IMAGE_TYPE_2D;
    TextureInfo.extent.width = Width;
    TextureInfo.extent.height = Height;
    TextureInfo.extent.depth = 1;
    TextureInfo.mipLevels = MipMapLevels;
    TextureInfo.arrayLayers = 6;
    TextureInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    TextureInfo.format = VK_FORMAT_D32_SFLOAT;
    TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    TextureInfo.usage = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    Texture::CreateTextureImage(TextureInfo);
}

void RenderedCubeMapDepthTexture::CreateTextureView()
{
    VkImageViewCreateInfo TextureImageViewInfo = {};
    TextureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    TextureImageViewInfo.image = Image;
    TextureImageViewInfo.format = VK_FORMAT_D32_SFLOAT;
    TextureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    TextureImageViewInfo.subresourceRange.baseMipLevel = 0;
    TextureImageViewInfo.subresourceRange.levelCount = MipMapLevels;
    TextureImageViewInfo.subresourceRange.baseArrayLayer = 0;
    TextureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    TextureImageViewInfo.subresourceRange.layerCount = 6;

    if (vkCreateImageView(VulkanPtr::GetDevice(), &TextureImageViewInfo, nullptr, &View)) {
        throw std::runtime_error("Failed to create Image View.");
    }
}

void RenderedCubeMapDepthTexture::CreateTextureSampler()
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

    if (vkCreateSampler(VulkanPtr::GetDevice(), &TextureImageSamplerInfo, nullptr, &Sampler))
    {
        throw std::runtime_error("Failed to create Sampler.");
    }
}

void RenderedCubeMapDepthTexture::RecreateRendererTexture(glm::ivec2 TextureResolution)
{
    Width = TextureResolution.x;
    Height = TextureResolution.y;

    Texture::Delete();
    CreateTextureImage();
    CreateTextureView();
    CreateTextureSampler();
}