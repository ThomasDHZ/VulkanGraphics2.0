#include "Texture2D.h"
#include "ImGui/imgui_impl_vulkan.h"
#include <stdexcept>

Texture2D::Texture2D() : Texture()
{
}

Texture2D::Texture2D(std::shared_ptr<VulkanEngine> engine, TextureType textureType) : Texture(textureType, VK_IMAGE_LAYOUT_UNDEFINED)
{
}

Texture2D::Texture2D(const std::string TextureLocation, VkFormat format) : Texture(TextureLocation, format, TextureType::vkTexture2D, VK_IMAGE_LAYOUT_UNDEFINED)
{
	CreateTextureView(format);
	CreateTextureSampler();
}

Texture2D::Texture2D(unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format) : Texture (width, height, PixelList, format, TextureType::vkTexture2D, VK_IMAGE_LAYOUT_UNDEFINED)
{
	CreateTextureView(format);
	CreateTextureSampler();
}

Texture2D::Texture2D(unsigned int width, unsigned int height, std::vector<glm::vec4>& PixelList, VkFormat format) : Texture(width, height, PixelList, format, TextureType::vkTexture2D, VK_IMAGE_LAYOUT_UNDEFINED)
{
	CreateTextureView(format);
	CreateTextureSampler();
}


Texture2D::~Texture2D()
{
}

void Texture2D::CreateTextureView(VkFormat format)
{
	VkImageViewCreateInfo TextureImageViewInfo = {};
	TextureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	TextureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	TextureImageViewInfo.format = format;
	TextureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	TextureImageViewInfo.subresourceRange.baseMipLevel = 0;
	TextureImageViewInfo.subresourceRange.levelCount = MipMapLevels;
	TextureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	TextureImageViewInfo.subresourceRange.layerCount = 1;
	TextureImageViewInfo.image = Image;

	if (vkCreateImageView(VulkanPtr::GetDevice(), &TextureImageViewInfo, nullptr, &View)) {
		throw std::runtime_error("Failed to create Image View.");
	}
}

void Texture2D::CreateTextureSampler()
{
	VkSamplerCreateInfo TextureImageSamplerInfo = {};
	TextureImageSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	TextureImageSamplerInfo.magFilter = VK_FILTER_NEAREST;
	TextureImageSamplerInfo.minFilter = VK_FILTER_NEAREST;
	TextureImageSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	TextureImageSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	TextureImageSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
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

	if (vkCreateSampler(VulkanPtr::GetDevice(), &TextureImageSamplerInfo, nullptr, &Sampler))
	{
		throw std::runtime_error("Failed to create Sampler.");
	}
}