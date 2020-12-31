#include "CubeMapTexture.h"

CubeMapTexture::CubeMapTexture() : Texture()
{
}

CubeMapTexture::CubeMapTexture(VulkanEngine& engine, CubeMapLayout CubeMapFiles) : Texture(engine, 0, TextureType::vkTextureCube)
{
	LoadTexture(engine, CubeMapFiles);
	CreateTextureView(engine);
	CreateTextureSampler(engine);
}

CubeMapTexture::CubeMapTexture(VulkanEngine& engine, std::string CubeMapFiles[6]) : Texture(engine, 0, TextureType::vkTextureCube)
{
	CubeMapLayout cubeMapfiles;
	cubeMapfiles.Left = CubeMapFiles[0];
	cubeMapfiles.Right = CubeMapFiles[1];
	cubeMapfiles.Top = CubeMapFiles[2];
	cubeMapfiles.Bottom = CubeMapFiles[3];
	cubeMapfiles.Front = CubeMapFiles[4];
	cubeMapfiles.Back = CubeMapFiles[5];

	LoadTexture(engine, cubeMapfiles);
	CreateTextureView(engine);
	CreateTextureSampler(engine);
}

CubeMapTexture::~CubeMapTexture()
{
}

void CubeMapTexture::LoadTexture(VulkanEngine& engine, CubeMapLayout CubeMapFiles)
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

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanBufferManager::CreateBuffer(engine, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_2D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = 1;
	TextureInfo.mipLevels = 1;
	TextureInfo.arrayLayers = 6;
	TextureInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	TextureInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	void* data;
	vkMapMemory(engine.Device, stagingBufferMemory, 0, imageSize, 0, &data);
	for (int x = 0; x < 6; ++x)
	{
		memcpy(static_cast<char*>(data) + (x * layerSize), textureData[x], static_cast<size_t>(layerSize));
	}
	vkUnmapMemory(engine.Device, stagingBufferMemory);

	Texture::CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, stagingBuffer);
	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(engine.Device, stagingBuffer, nullptr);
	vkFreeMemory(engine.Device, stagingBufferMemory, nullptr);
	stbi_image_free(textureData[0]);
	stbi_image_free(textureData[1]);
	stbi_image_free(textureData[2]);
	stbi_image_free(textureData[3]);
	stbi_image_free(textureData[4]);
	stbi_image_free(textureData[5]);

//	KTX2TextureLoader KTXLoader = KTX2TextureLoader();
//TextureData = KTXLoader.KTX2extureLoader("C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/CubeMap_KTX2_BC7.KTX2");
//
//Width = TextureData.Width;
//Height = TextureData.Height;
//
//VkBuffer stagingBuffer;
//VkDeviceMemory stagingBufferMemory;
//VulkanBufferManager::CreateBuffer(engine, TextureData.TextureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//VkImageCreateInfo TextureInfo = {};
//TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//TextureInfo.imageType = VK_IMAGE_TYPE_2D;
//TextureInfo.extent.width = Width;
//TextureInfo.extent.height = Height;
//TextureInfo.extent.depth = 1;
//TextureInfo.mipLevels = 1;
//TextureInfo.arrayLayers = 6;
//TextureInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
//TextureInfo.format = VK_FORMAT_BC7_UNORM_BLOCK;
//TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
//TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
//TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//void* data;
//vkMapMemory(engine.Device, stagingBufferMemory, 0, TextureData.TextureSize, 0, &data);
//memcpy(data, TextureData.TextureData.data(), TextureData.TextureSize);
//vkUnmapMemory(engine.Device, stagingBufferMemory);
//
//Texture::CreateTextureImage(engine, TextureInfo);
//
//KTXTransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//KTXCopyBufferToImage(engine, stagingBuffer);
//KTXTransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
//vkDestroyBuffer(engine.Device, stagingBuffer, nullptr);
//vkFreeMemory(engine.Device, stagingBufferMemory, nullptr);
}

void CubeMapTexture::CreateTextureView(VulkanEngine& engine)
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

	View = engine.CreateTextureView(TextureImageViewInfo);
}

void CubeMapTexture::CreateTextureSampler(VulkanEngine& engine)
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

	Sampler = engine.CreateTextureSampler(TextureImageSamplerInfo);
}