#include "FontTexture.h"

FontTexture::FontTexture() : Texture()
{
}

FontTexture::FontTexture(std::shared_ptr<VulkanEngine> engine, void* GlyphData, uint32_t width, uint32_t height) : Texture()
{
	TextureID = engine->GenerateID();
	CreateTextTexture(engine, GlyphData, width, height);
	CreateTextureView(engine);
	CreateTextureSampler(engine);
}

FontTexture::~FontTexture()
{
}

void FontTexture::CreateTextTexture(std::shared_ptr<VulkanEngine> engine, void* GlyphData, uint32_t width, uint32_t height)
{
	Width = width;
	Height = height;

	VkDeviceSize glypfSize = Width * Height;
	byte* buff = static_cast<byte*>(GlyphData);

	std::vector<Pixel> PixelList;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			const uint32_t index = x + (width * y);
			PixelList.emplace_back(Pixel(buff[index], buff[index], buff[index], 0xff));
		}
	}
	glypfSize *= 4;

	VkDeviceSize imageSize = Width * Height * sizeof(Pixel);

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &PixelList[0]);

	MipMapLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(Width, Height)))) + 1;

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_2D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = Depth;
	TextureInfo.mipLevels = MipMapLevels;
	TextureInfo.arrayLayers = 1;
	TextureInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Texture::CreateTextureImage(TextureInfo);

	TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(StagingBuffer.Buffer);

	StagingBuffer.DestoryBuffer();

	GenerateMipmaps(VK_FORMAT_R8G8B8A8_UNORM);
}

void FontTexture::CreateTextureView(std::shared_ptr<VulkanEngine> engine)
{
	VkImageViewCreateInfo TextureImageViewInfo = {};
	TextureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	TextureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	TextureImageViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	TextureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	TextureImageViewInfo.subresourceRange.baseMipLevel = 0;
	TextureImageViewInfo.subresourceRange.levelCount = MipMapLevels;
	TextureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	TextureImageViewInfo.subresourceRange.layerCount = 1;
	TextureImageViewInfo.image = Image;

	if (vkCreateImageView(engine->Device, &TextureImageViewInfo, nullptr, &View)) {
		throw std::runtime_error("Failed to create Image View.");
	}
}

void FontTexture::CreateTextureSampler(std::shared_ptr<VulkanEngine> engine)
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

	if (vkCreateSampler(engine->Device, &TextureImageSamplerInfo, nullptr, &Sampler))
	{
		throw std::runtime_error("Failed to create Sampler.");
	}
}