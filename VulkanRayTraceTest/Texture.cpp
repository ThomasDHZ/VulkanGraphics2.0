#include "Texture.h"
#include <stdexcept>
#include "ImGui/imgui_impl_vulkan.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <stb_image.h>
#include "VulkanBuffer.h"

Texture::Texture()
{
	FileName = "";
	Width = 0;
	Height = 0;
	Depth = 0;
	TextureBufferIndex = 0;
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, TextureType textureType, VkImageLayout imageLayout)
{
	Width = 0;
	Height = 0;
	Depth = 1;
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, glm::vec2& TextureResolution, TextureType textureType, VkImageLayout imageLayout)
{
	Width = TextureResolution.x;
	Height = TextureResolution.y;
	Depth = 1;
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, unsigned int width, unsigned int height, TextureType textureType, VkImageLayout imageLayout)
{
	Width = width;
	Height = height;
	Depth = 1;
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, glm::vec3& TextureResolution, TextureType textureType, VkImageLayout imageLayout)
{
	Width = TextureResolution.x;
	Height = TextureResolution.y;
	Depth = TextureResolution.z;
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, unsigned int width, unsigned int height, unsigned int depth, TextureType textureType, VkImageLayout imageLayout)
{
	Width = width;
	Height = height;
	Depth = depth;
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, glm::vec2& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	Width = TextureResolution.x;
	Height = TextureResolution.y;
	Depth = 1;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture(engine, PixelList, format);
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, glm::vec2& TextureResolution, std::vector<glm::vec4>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	Width = TextureResolution.x;
	Height = TextureResolution.y;
	Depth = 1;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture(engine, PixelList, format);
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	Width = width;
	Height = height;
	Depth = 1;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture(engine, PixelList, format);
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, unsigned int width, unsigned int height, std::vector<glm::vec4>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	Width = width;
	Height = height;
	Depth = 1;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture(engine, PixelList, format);
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, glm::vec3& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	Width = TextureResolution.x;
	Height = TextureResolution.y;
	Depth = TextureResolution.z;
	TextureFormat = format;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture3D(engine, PixelList, format);
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	Width = width;
	Height = height;
	Depth = depth;
	TextureFormat = format;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture3D(engine, PixelList, format);
}

Texture::Texture(std::shared_ptr<VulkanEngine> engine, std::string TextureLocation, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	Width = 0;
	Height = 0;
	Depth = 1;
	TextureID = engine->GenerateID();
	TextureBufferIndex = 0;
	FileName = TextureLocation;
	TextureFormat = format;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	LoadTexture(engine, TextureLocation, format);
}

Texture::~Texture()
{
}

void Texture::TransitionImageLayout(std::shared_ptr<VulkanEngine> engine, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = engine->beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = Image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = MipMapLevels;
	barrier.subresourceRange.baseArrayLayer = 0;

	if (TypeOfTexture == TextureType::vkTexture2D ||
		TypeOfTexture == TextureType::vkTexture3D ||
		TypeOfTexture == TextureType::vkHeightMap)
	{
		barrier.subresourceRange.layerCount = 1;
	}
	else if (TypeOfTexture == TextureType::vkTextureCube)
	{
		barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	}

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	engine->endSingleTimeCommands(commandBuffer);
}

void Texture::CopyBufferToImage(std::shared_ptr<VulkanEngine> engine, VkBuffer buffer)
{
	VkCommandBuffer commandBuffer = engine->beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageOffset.x = 0;
	region.imageOffset.y = 0;
	region.imageOffset.z = 0;
	region.imageExtent.width = Width;
	region.imageExtent.height = Height;
	region.imageExtent.depth = Depth;

	if (TypeOfTexture == TextureType::vkTexture2D ||
		TypeOfTexture == TextureType::vkTexture3D ||
		TypeOfTexture == TextureType::vkHeightMap)
	{
		region.imageSubresource.layerCount = 1;
	}
	else if (TypeOfTexture == TextureType::vkTextureCube)
	{
		region.imageSubresource.layerCount = 6;
	}

	vkCmdCopyBufferToImage(commandBuffer, buffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	engine->endSingleTimeCommands(commandBuffer);
}

void Texture::LoadKTXTexture(std::shared_ptr<VulkanEngine> engine, std::string TextureLocation, VkFormat format)
{
	/*KTX2TextureLoader KTXLoader = KTX2TextureLoader();
	TextureData = KTXLoader.KTX2extureLoader("C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/back_DDS_BC7_1.ktx2");

	Width = TextureData.Width;
	Height = TextureData.Height;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanBufferManager::CreateBuffer(engine, TextureData.TextureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = TextureData.TextureType;
	TextureInfo.extent.width = TextureData.Width;
	TextureInfo.extent.height = TextureData.Height;
	TextureInfo.extent.depth = TextureData.Depth;
	TextureInfo.mipLevels = TextureData.MipLevels;
	TextureInfo.arrayLayers = TextureData.ArrayLayers;
	TextureInfo.format = VK_FORMAT_BC7_UNORM_BLOCK;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	void* data;
	vkMapMemory(engine->Device, stagingBufferMemory, 0, TextureData.TextureSize, 0, &data);
	memcpy(data, TextureData.TextureData.data(), TextureData.TextureSize);
	vkUnmapMemory(engine->Device, stagingBufferMemory);

	Texture::CreateTextureImage(engine, TextureInfo);

	KTXTransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	KTXCopyBufferToImage(engine, stagingBuffer);
	KTXTransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(engine->Device, stagingBuffer, nullptr);
	vkFreeMemory(engine->Device, stagingBufferMemory, nullptr);*/
}

void Texture::LoadDDSTexture(std::shared_ptr<VulkanEngine> engine, std::string TextureLocation, VkFormat format)
{
	DDSTextureLoader ddsLoader = DDSTextureLoader();
	TextureInfo textureInfo = ddsLoader.DDSSTextureLoader(TextureLocation);
	
	Width = textureInfo.Width;
	Height = textureInfo.Height;
	Depth = textureInfo.Depth;

	VkDeviceSize imageSize = Width * Height * Depth * textureInfo.BytesOfKeyValueData;

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine->Device, engine->PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &textureInfo.TextureData[0]);

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_3D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = Depth;
	TextureInfo.mipLevels = 1;
	TextureInfo.arrayLayers = 1;
	TextureInfo.format = format;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);
	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.DestoryBuffer(engine->Device);
}

void Texture::LoadTexture(std::shared_ptr<VulkanEngine> engine, std::string TextureLocation, VkFormat format)
{
	int ColorChannels;
	stbi_uc* pixels = stbi_load(TextureLocation.c_str(), &Width, &Height, &ColorChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = Width * Height * 4;
	MipMapLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(Width, Height)))) + 1;

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine->Device, engine->PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pixels);

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_2D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = Depth;
	TextureInfo.mipLevels = MipMapLevels;
	TextureInfo.arrayLayers = 1;
	TextureInfo.format = format;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);

	StagingBuffer.DestoryBuffer(engine->Device);
	stbi_image_free(pixels);

	GenerateMipmaps(engine, format);
}

void Texture::CreateTexture(std::shared_ptr<VulkanEngine> engine, std::vector<Pixel>& Pixels, VkFormat format)
{
	VkDeviceSize imageSize = Width * Height * sizeof(Pixel);

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine->Device, engine->PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Pixels[0]);
	
	MipMapLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(Width, Height)))) + 1;
	
	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_2D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = Depth;
	TextureInfo.mipLevels = MipMapLevels;
	TextureInfo.arrayLayers = 1;
	TextureInfo.format = format;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Texture::CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);

	StagingBuffer.DestoryBuffer(engine->Device);

	GenerateMipmaps(engine, format);
}

void Texture::CreateTexture(std::shared_ptr<VulkanEngine> engine, std::vector<glm::vec4>& Pixels, VkFormat format)
{
	VkDeviceSize imageSize = Width * Height * sizeof(glm::vec4);

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine->Device, engine->PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Pixels[0]);

	MipMapLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(Width, Height)))) + 1;

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_2D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = Depth;
	TextureInfo.mipLevels = MipMapLevels;
	TextureInfo.arrayLayers = 1;
	TextureInfo.format = format;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Texture::CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);

	StagingBuffer.DestoryBuffer(engine->Device);

	GenerateMipmaps(engine, format);
}

void Texture::CreateTexture3D(std::shared_ptr<VulkanEngine> engine, std::vector<Pixel>& Pixels, VkFormat format)
{
	VkDeviceSize imageSize = Width * Height * Depth * sizeof(Pixel);

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine->Device, engine->PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Pixels[0]);

	VkImageCreateInfo TextureInfo = {};
	TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	TextureInfo.imageType = VK_IMAGE_TYPE_3D;
	TextureInfo.extent.width = Width;
	TextureInfo.extent.height = Height;
	TextureInfo.extent.depth = Depth;
	TextureInfo.mipLevels = MipMapLevels;
	TextureInfo.arrayLayers = 1;
	TextureInfo.format = format;
	TextureInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Texture::CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);
	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.DestoryBuffer(engine->Device);
}

void Texture::CreateTextureImage(std::shared_ptr<VulkanEngine> engine, VkImageCreateInfo TextureInfo)
{
	if (vkCreateImage(engine->Device, &TextureInfo, nullptr, &Image)) {
		throw std::runtime_error("Failed to create Image.");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(engine->Device, Image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = engine->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(engine->Device, &allocInfo, nullptr, &Memory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate image Memory.");
	}

	vkBindImageMemory(engine->Device, Image, Memory, 0);
}

void Texture::UpdateImageLayout(std::shared_ptr<VulkanEngine> engine, VkCommandBuffer buffer, VkImageLayout newImageLayout)
{
	const VkImageLayout OldImageLayout = ImageLayout;
	ImageLayout = newImageLayout;

	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.oldLayout = OldImageLayout;
	imageMemoryBarrier.newLayout = ImageLayout;
	imageMemoryBarrier.image = Image;
	imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	if(newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = 0;
	}
	else
	{
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void Texture::UpdateImageLayout(std::shared_ptr<VulkanEngine> engine, VkCommandBuffer commandBuffer, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
{
	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldImageLayout;
	barrier.newLayout = newImageLayout;
	barrier.image = Image;
	barrier.subresourceRange = subresourceRange;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Texture::UpdateImageLayout(std::shared_ptr<VulkanEngine> engine, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
{
	VkImageSubresourceRange ImageSubresourceRange{};
	ImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageSubresourceRange.baseMipLevel = 0;
	ImageSubresourceRange.levelCount = 1;
	ImageSubresourceRange.layerCount = 1;

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldImageLayout;
	barrier.newLayout = newImageLayout;
	barrier.image = Image;
	barrier.subresourceRange = ImageSubresourceRange;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	auto SingleCommand = engine->beginSingleTimeCommands();
	vkCmdPipelineBarrier(SingleCommand, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	engine->endSingleTimeCommands(SingleCommand);
}

void Texture::UpdateCubeImageLayout(std::shared_ptr<VulkanEngine> engine, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
{
	VkImageSubresourceRange ImageSubresourceRange{};
	ImageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageSubresourceRange.baseMipLevel = 0;
	ImageSubresourceRange.levelCount = 1;
	ImageSubresourceRange.layerCount = 6;

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldImageLayout;
	barrier.newLayout = newImageLayout;
	barrier.image = Image;
	barrier.subresourceRange = ImageSubresourceRange;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	auto SingleCommand = engine->beginSingleTimeCommands();
	vkCmdPipelineBarrier(SingleCommand, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	engine->endSingleTimeCommands(SingleCommand);
}

void Texture::UpdateCubeImageLayout(std::shared_ptr<VulkanEngine> engine, VkCommandBuffer commandBuffer, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
{
	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldImageLayout;
	barrier.newLayout = newImageLayout;
	barrier.image = Image;
	barrier.subresourceRange = subresourceRange;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}


void Texture::CopyTexture(std::shared_ptr<VulkanEngine> engine, VkCommandBuffer& commandBuffer, std::shared_ptr<Texture> CopyToTexture)
{
	VkImageCopy copyRegion = {};
	copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.srcSubresource.baseArrayLayer = 0;
	copyRegion.srcSubresource.mipLevel = 0;
	copyRegion.srcSubresource.layerCount = 1;
	copyRegion.srcOffset = { 0, 0, 0 };

	copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.dstSubresource.baseArrayLayer = 0;
	copyRegion.dstSubresource.mipLevel = 0;
	copyRegion.dstSubresource.layerCount = 1;
	copyRegion.dstOffset = { 0, 0, 0 };

	copyRegion.extent.width = (uint32_t)Width;
	copyRegion.extent.height = (uint32_t)Height;
	copyRegion.extent.depth = 1;
	vkCmdCopyImage(commandBuffer, Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, CopyToTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
}

void Texture::CopyTexture(std::shared_ptr<VulkanEngine> engine, VkCommandBuffer& commandBuffer, std::shared_ptr<Texture> CopyToTexture, int FaceCopy)
{
	VkImageCopy copyRegion = {};
	copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.srcSubresource.baseArrayLayer = 0;
	copyRegion.srcSubresource.mipLevel = 0;
	copyRegion.srcSubresource.layerCount = 1;
	copyRegion.srcOffset = { 0, 0, 0 };

	copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.dstSubresource.baseArrayLayer = FaceCopy;
	copyRegion.dstSubresource.mipLevel = 0;
	copyRegion.dstSubresource.layerCount = 1;
	copyRegion.dstOffset = { 0, 0, 0 };

	copyRegion.extent.width = (uint32_t)Width;
	copyRegion.extent.height = (uint32_t)Height;
	copyRegion.extent.depth = 1;
	vkCmdCopyImage(commandBuffer, Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, CopyToTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
}

void Texture::Update(std::shared_ptr<VulkanEngine> engine, uint32_t NewTextureBufferIndex)
{
	TextureBufferIndex = NewTextureBufferIndex;
}

void Texture::Delete(std::shared_ptr<VulkanEngine> engine)
{
	vkDestroyImageView(engine->Device, View, nullptr);
	vkDestroyImage(engine->Device, Image, nullptr);
	vkFreeMemory(engine->Device, Memory, nullptr);
	vkDestroySampler(engine->Device, Sampler, nullptr);

	View = VK_NULL_HANDLE;
	Image = VK_NULL_HANDLE;
	Memory = VK_NULL_HANDLE;
	Sampler = VK_NULL_HANDLE;
}

void Texture::GenerateMipmaps(std::shared_ptr<VulkanEngine> engine, VkFormat imageFormat)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(engine->PhysicalDevice, imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("texture image format does not support linear blitting!");
	}

	VkCommandBuffer commandBuffer = engine->beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = Image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = Width;
	int32_t mipHeight = Height;

	for (uint32_t i = 1; i < MipMapLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer,
			Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = MipMapLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	engine->endSingleTimeCommands(commandBuffer);
}