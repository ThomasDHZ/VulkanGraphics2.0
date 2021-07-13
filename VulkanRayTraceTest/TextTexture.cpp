#include "TextTexture.h"

TextTexture::TextTexture() : Texture()
{
}

TextTexture::TextTexture(VulkanEngine& engine, void* GlyphData, uint32_t width, uint32_t height) : Texture()
{
	CreateTextTexture(engine, GlyphData, width, height);
	CreateTextureView(engine);
	CreateTextureSampler(engine);
}

TextTexture::~TextTexture()
{
}

void TextTexture::CreateTextureView(VulkanEngine& engine)
{
	VkImageViewCreateInfo TextureImageViewInfo = {};
	TextureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	TextureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	TextureImageViewInfo.format = VK_FORMAT_R8_UINT;
	TextureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	TextureImageViewInfo.subresourceRange.baseMipLevel = 0;
	TextureImageViewInfo.subresourceRange.levelCount = MipMapLevels;
	TextureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	TextureImageViewInfo.subresourceRange.layerCount = 1;
	TextureImageViewInfo.image = Image;

	if (vkCreateImageView(engine.Device, &TextureImageViewInfo, nullptr, &View)) {
		throw std::runtime_error("Failed to create Image View.");
	}
}

void TextTexture::CreateTextureSampler(VulkanEngine& engine)
{
	VkSamplerCreateInfo TextureImageSamplerInfo = {};
	TextureImageSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	TextureImageSamplerInfo.magFilter = VK_FILTER_LINEAR;
	TextureImageSamplerInfo.minFilter = VK_FILTER_LINEAR;
	TextureImageSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	TextureImageSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	TextureImageSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	TextureImageSamplerInfo.anisotropyEnable = VK_TRUE;
	TextureImageSamplerInfo.maxAnisotropy = 16.0f;
	TextureImageSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	TextureImageSamplerInfo.unnormalizedCoordinates = VK_FALSE;
	TextureImageSamplerInfo.compareEnable = VK_FALSE;
	TextureImageSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	TextureImageSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	TextureImageSamplerInfo.minLod = 0;
	TextureImageSamplerInfo.maxLod = static_cast<float>(MipMapLevels);
	TextureImageSamplerInfo.mipLodBias = 0;

	if (vkCreateSampler(engine.Device, &TextureImageSamplerInfo, nullptr, &Sampler))
	{
		throw std::runtime_error("Failed to create Sampler.");
	}
}