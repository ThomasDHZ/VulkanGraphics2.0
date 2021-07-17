#include "TextTexture.h"

TextTexture::TextTexture() : Texture()
{
}

TextTexture::TextTexture(VulkanEngine& engine, void* GlyphData, uint32_t width, uint32_t height) : Texture()
{
	TextureID = engine.GenerateID();
	CreateTextTexture(engine, GlyphData, width, height);
	CreateTextureView(engine);
	CreateTextureSampler(engine);
}

TextTexture::~TextTexture()
{
}

void TextTexture::CreateTextTexture(VulkanEngine& engine, void* GlyphData, uint32_t width, uint32_t height)
{
	Width = width;
	Height = height;

	VkDeviceSize imageSize = Width * Height;


	byte* buff = static_cast<byte*>(GlyphData);

	std::vector<byte> PixelList;
	for (int x = 0; x < imageSize; x++)
	{
		PixelList.emplace_back(0xff);
	}
	//imageSize *= 4;
	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &PixelList[0]);

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.imageType = VK_IMAGE_TYPE_2D;
	TextureInfo.format = VK_FORMAT_R8_UNORM;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = 1;
	TextureInfo.mipLevels = 1;
	TextureInfo.arrayLayers = 1;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	Texture::CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);

	StagingBuffer.DestoryBuffer(engine.Device);
}

void TextTexture::CreateTextureView(VulkanEngine& engine)
{
	VkImageViewCreateInfo TextureImageViewInfo = {};
	TextureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	TextureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	TextureImageViewInfo.format = VK_FORMAT_R8_UNORM;
	TextureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	TextureImageViewInfo.subresourceRange.baseMipLevel = 0;
	TextureImageViewInfo.subresourceRange.levelCount = 1;
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
	TextureImageSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	TextureImageSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	TextureImageSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	TextureImageSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	TextureImageSamplerInfo.mipLodBias = 0.0f;
	TextureImageSamplerInfo.compareOp = VK_COMPARE_OP_NEVER;
	TextureImageSamplerInfo.minLod = 0.0f;
	TextureImageSamplerInfo.maxLod = 1.0f;
	TextureImageSamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

	if (vkCreateSampler(engine.Device, &TextureImageSamplerInfo, nullptr, &Sampler))
	{
		throw std::runtime_error("Failed to create Sampler.");
	}
}