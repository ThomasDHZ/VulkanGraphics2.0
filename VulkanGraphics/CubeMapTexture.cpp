#include "CubeMapTexture.h"
#include <stb_image.h>
#include "VulkanBuffer.h"

CubeMapTexture::CubeMapTexture() : Texture()
{
}

CubeMapTexture::CubeMapTexture(CubeMapLayout CubeMapFiles, VkFormat textureFormat) : Texture(TextureType::vkTextureCube)
{
	LoadTexture(CubeMapFiles, textureFormat);
	CreateTextureView(textureFormat);
	CreateTextureSampler();
}

CubeMapTexture::CubeMapTexture(std::string CubeMapFiles[6], VkFormat textureFormat) : Texture(TextureType::vkTextureCube)
{
	CubeMapLayout cubeMapfiles;
	cubeMapfiles.Left = CubeMapFiles[0];
	cubeMapfiles.Right = CubeMapFiles[1];
	cubeMapfiles.Top = CubeMapFiles[2];
	cubeMapfiles.Bottom = CubeMapFiles[3];
	cubeMapfiles.Front = CubeMapFiles[4];
	cubeMapfiles.Back = CubeMapFiles[5];

	LoadTexture(cubeMapfiles, textureFormat);
	CreateTextureView(textureFormat);
	CreateTextureSampler();
}

CubeMapTexture::CubeMapTexture(std::string CubeMapLocation, VkFormat textureFormat)
{
	LoadTexture(CubeMapLocation, textureFormat);
	CreateTextureView(textureFormat);
	CreateTextureSampler();
}

CubeMapTexture::~CubeMapTexture()
{
}

void CubeMapTexture::LoadTexture(CubeMapLayout CubeMapFiles, VkFormat textureFormat)
{
	std::vector<unsigned char*> textureData;
	int texChannels;

	textureData.emplace_back(stbi_load(CubeMapFiles.Left.c_str(), &Width, &Height, &texChannels, STBI_rgb_alpha));
	textureData.emplace_back(stbi_load(CubeMapFiles.Right.c_str(), &Width, &Height, &texChannels, STBI_rgb_alpha));
	textureData.emplace_back(stbi_load(CubeMapFiles.Top.c_str(), &Width, &Height, &texChannels, STBI_rgb_alpha));
	textureData.emplace_back(stbi_load(CubeMapFiles.Bottom.c_str(), &Width, &Height, &texChannels, STBI_rgb_alpha));
	textureData.emplace_back(stbi_load(CubeMapFiles.Back.c_str(), &Width, &Height, &texChannels, STBI_rgb_alpha));
	textureData.emplace_back(stbi_load(CubeMapFiles.Front.c_str(), &Width, &Height, &texChannels, STBI_rgb_alpha));

	const VkDeviceSize imageSize = Width * Height * 4 * 6;
	const VkDeviceSize layerSize = imageSize / 6;

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(VulkanPtr::GetDevice(), StagingBuffer.BufferMemory, 0, imageSize, 0, &data);
	for (int x = 0; x < 6; ++x)
	{
		memcpy(static_cast<char*>(data) + (x * layerSize), textureData[x], static_cast<size_t>(layerSize));
	}
	vkUnmapMemory(VulkanPtr::GetDevice(), StagingBuffer.BufferMemory);

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_2D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = 1;
	TextureInfo.mipLevels = 1;
	TextureInfo.arrayLayers = 6;
	TextureInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	TextureInfo.format = textureFormat;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	CreateTextureImage(TextureInfo);

	TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(StagingBuffer.Buffer);
	TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.DestoryBuffer();
	for (auto texturedata : textureData)
	{
		stbi_image_free(texturedata);
	}
}

void CubeMapTexture::LoadTexture(std::string CubeMapLocation, VkFormat textureFormat)
{
	std::vector<unsigned char*> textureData;
	int texChannels;

	auto data = stbi_load(CubeMapLocation.c_str(), &Width, &Height, &texChannels, STBI_rgb_alpha);

	const VkDeviceSize imageSize = Width * Height * 4 * 6;
	const VkDeviceSize layerSize = imageSize / 6;

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(layerSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &data[0]);

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_2D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = 1;
	TextureInfo.mipLevels = 1;
	TextureInfo.arrayLayers = 6;
	TextureInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	TextureInfo.format = textureFormat;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	CreateTextureImage(TextureInfo);

	TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(StagingBuffer.Buffer);
	TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.DestoryBuffer();
	for (auto texturedata : textureData)
	{
		stbi_image_free(texturedata);
	}
}

void CubeMapTexture::CreateTextureView(VkFormat textureFormat)
{
	VkImageViewCreateInfo TextureImageViewInfo = {};
	TextureImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	TextureImageViewInfo.image = Image;
	TextureImageViewInfo.format = textureFormat;
	TextureImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	TextureImageViewInfo.subresourceRange.baseMipLevel = 0;
	TextureImageViewInfo.subresourceRange.levelCount = 1;
	TextureImageViewInfo.subresourceRange.baseArrayLayer = 0;
	TextureImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	TextureImageViewInfo.subresourceRange.layerCount = 6;

	if (vkCreateImageView(VulkanPtr::GetDevice(), &TextureImageViewInfo, nullptr, &View)) {
		throw std::runtime_error("Failed to create Image View.");
	}
}

void CubeMapTexture::CreateTextureSampler()
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

	if (vkCreateSampler(VulkanPtr::GetDevice(), &TextureImageSamplerInfo, nullptr, &Sampler))
	{
		throw std::runtime_error("Failed to create Sampler.");
	}
}