#include "HeightMapTexture.h"
#include <stb_image.h>

HeightMapTexture::HeightMapTexture() : Texture2D()
{
}

HeightMapTexture::HeightMapTexture(VulkanEngine& engine, const std::string TextureLocation) : Texture2D(engine, TextureLocation, VK_FORMAT_R8G8B8A8_UNORM)
{
	Width = 0;
	Height = 0;
	Depth = 1;
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	TypeOfTexture = vkHeightMap;
	FileName = TextureLocation;
	TextureFormat = VK_FORMAT_R8G8B8A8_UNORM;

	LoadTexture(engine, TextureLocation, VK_FORMAT_R8G8B8A8_UNORM);
	CreateTextureView(engine, VK_FORMAT_R8G8B8A8_UNORM);
	CreateTextureSampler(engine);
}

HeightMapTexture::HeightMapTexture(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList) : Texture2D(engine, width, height, PixelList, VK_FORMAT_R8G8B8A8_UNORM)
{
}

HeightMapTexture::~HeightMapTexture()
{
}

void HeightMapTexture::LoadTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format)
{
	int ColorChannels;
	stbi_uc* pixels = stbi_load(TextureLocation.c_str(), &Width, &Height, &ColorChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = Width * Height * 4;

	for (int x = 0; x < imageSize; x += 4)
	{
		HeightMap.emplace_back(Pixel(pixels[x], pixels[x+1], pixels[x+2]));
	}

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pixels);

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_2D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = Depth;
	TextureInfo.mipLevels = 1;
	TextureInfo.arrayLayers = 1;
	TextureInfo.format = format;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Texture::CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);
	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.DestoryBuffer(engine.Device);
	stbi_image_free(pixels);
}

Pixel HeightMapTexture::GetPixel(uint32_t x, uint32_t y)
{
	return HeightMap[x + (Width * y)];
}