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

Texture::Texture(VulkanEngine& engine, TextureType textureType, VkImageLayout imageLayout)
{
	Width = 0;
	Height = 0;
	Depth = 1;
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(VulkanEngine& engine, glm::vec2& TextureResolution, TextureType textureType, VkImageLayout imageLayout)
{
	Width = TextureResolution.x;
	Height = TextureResolution.y;
	Depth = 1;
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(VulkanEngine& engine, unsigned int width, unsigned int height, TextureType textureType, VkImageLayout imageLayout)
{
	Width = width;
	Height = height;
	Depth = 1;
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(VulkanEngine& engine, glm::vec3& TextureResolution, TextureType textureType, VkImageLayout imageLayout)
{
	Width = TextureResolution.x;
	Height = TextureResolution.y;
	Depth = TextureResolution.z;
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(VulkanEngine& engine, unsigned int width, unsigned int height, unsigned int depth, TextureType textureType, VkImageLayout imageLayout)
{
	Width = width;
	Height = height;
	Depth = depth;
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;
}

Texture::Texture(VulkanEngine& engine, glm::vec2& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	Width = TextureResolution.x;
	Height = TextureResolution.y;
	Depth = 1;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture(engine, PixelList, format);
}

Texture::Texture(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	Width = width;
	Height = height;
	Depth = 1;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture(engine, PixelList, format);
}

Texture::Texture(VulkanEngine& engine, glm::vec3& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	Width = TextureResolution.x;
	Height = TextureResolution.y;
	Depth = TextureResolution.z;
	TextureFormat = format;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture3D(engine, PixelList, format);
}

Texture::Texture(VulkanEngine& engine, int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	TextureID = engine.GenerateID();
	TextureBufferIndex = 0;
	Width = width;
	Height = height;
	Depth = depth;
	TextureFormat = format;
	ImageLayout = imageLayout;
	TypeOfTexture = textureType;

	CreateTexture3D(engine, PixelList, format);
}

Texture::Texture(VulkanEngine& engine, std::string TextureLocation, VkFormat format, TextureType textureType, VkImageLayout imageLayout)
{
	Width = 0;
	Height = 0;
	Depth = 1;
	TextureID = engine.GenerateID();
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

void Texture::TransitionImageLayout(VulkanEngine& engine, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = engine.beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = Image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
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
	engine.endSingleTimeCommands(commandBuffer);
}

void Texture::CopyBufferToImage(VulkanEngine& engine, VkBuffer buffer)
{
	VkCommandBuffer commandBuffer = engine.beginSingleTimeCommands();

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
	engine.endSingleTimeCommands(commandBuffer);
}

void Texture::LoadKTXTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format)
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
	vkMapMemory(engine.Device, stagingBufferMemory, 0, TextureData.TextureSize, 0, &data);
	memcpy(data, TextureData.TextureData.data(), TextureData.TextureSize);
	vkUnmapMemory(engine.Device, stagingBufferMemory);

	Texture::CreateTextureImage(engine, TextureInfo);

	KTXTransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	KTXCopyBufferToImage(engine, stagingBuffer);
	KTXTransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(engine.Device, stagingBuffer, nullptr);
	vkFreeMemory(engine.Device, stagingBufferMemory, nullptr);*/
}

void Texture::LoadDDSTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format)
{
	DDSTextureLoader ddsLoader = DDSTextureLoader();
	TextureInfo textureInfo = ddsLoader.DDSSTextureLoader(TextureLocation);
	
	Width = textureInfo.Width;
	Height = textureInfo.Height;
	Depth = textureInfo.Depth;

	VkDeviceSize imageSize = Width * Height * Depth * textureInfo.BytesOfKeyValueData;

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &textureInfo.TextureData[0]);

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
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);
	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.DestoryBuffer(engine.Device);
}

void Texture::LoadTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format)
{
	int ColorChannels;
	stbi_uc* pixels = stbi_load(TextureLocation.c_str(), &Width, &Height, &ColorChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = Width * Height * 4;

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

	CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);
	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.DestoryBuffer(engine.Device);
	stbi_image_free(pixels);
}

void Texture::CreateTexture(VulkanEngine& engine, std::vector<Pixel>& Pixels, VkFormat format)
{
	VkDeviceSize imageSize = Width * Height * sizeof(Pixel);

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Pixels[0]);

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
}

void Texture::CreateTexture3D(VulkanEngine& engine, std::vector<Pixel>& Pixels, VkFormat format)
{
	VkDeviceSize imageSize = Width * Height * Depth * sizeof(Pixel);

	VulkanBuffer StagingBuffer;
	StagingBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Pixels[0]);

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
	TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	TextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	Texture::CreateTextureImage(engine, TextureInfo);

	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(engine, StagingBuffer.Buffer);
	TransitionImageLayout(engine, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	StagingBuffer.DestoryBuffer(engine.Device);
}

void Texture::CreateTextureImage(VulkanEngine& engine, VkImageCreateInfo TextureInfo)
{
	if (vkCreateImage(engine.Device, &TextureInfo, nullptr, &Image)) {
		throw std::runtime_error("Failed to create Image.");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(engine.Device, Image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = engine.FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(engine.Device, &allocInfo, nullptr, &Memory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate image Memory.");
	}

	vkBindImageMemory(engine.Device, Image, Memory, 0);
}

void Texture::UpdateImageLayout(VulkanEngine& engine, VkCommandBuffer buffer, VkImageLayout newImageLayout)
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

void Texture::Update(VulkanEngine& engine, uint32_t NewTextureBufferIndex)
{
	TextureBufferIndex = NewTextureBufferIndex;
}

void Texture::Delete(VulkanEngine& engine)
{
	vkDestroyImageView(engine.Device, View, nullptr);
	vkDestroyImage(engine.Device, Image, nullptr);
	vkFreeMemory(engine.Device, Memory, nullptr);
	vkDestroySampler(engine.Device, Sampler, nullptr);

	View = VK_NULL_HANDLE;
	Image = VK_NULL_HANDLE;
	Memory = VK_NULL_HANDLE;
	Sampler = VK_NULL_HANDLE;
}

void Texture::CopyTexture(VulkanEngine& engine, VkCommandBuffer CommandBuffer, std::shared_ptr<Texture> SrcTexture, std::shared_ptr<Texture> DstTexture)
{
	VkImageSubresourceRange subresourceRange{};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;

	VkImageMemoryBarrier MemoryBarrior{};
	MemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	MemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	MemoryBarrior.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	MemoryBarrior.image = SrcTexture->Image;
	MemoryBarrior.subresourceRange = subresourceRange;
	MemoryBarrior.srcAccessMask = 0;
	MemoryBarrior.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &MemoryBarrior);

	VkImageMemoryBarrier barrier2 = {};
	barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier2.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier2.image = DstTexture->Image;
	barrier2.subresourceRange = subresourceRange;
	barrier2.srcAccessMask = 0;
	barrier2.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier2);

	VkImageCopy copyRegion{};
	copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	copyRegion.srcOffset = { 0, 0, 0 };
	copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	copyRegion.dstOffset = { 0, 0, 0 };
	copyRegion.extent = { (uint32_t)SrcTexture->Width, (uint32_t)SrcTexture->Height, 1 };
	vkCmdCopyImage(CommandBuffer, SrcTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, DstTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

	VkImageMemoryBarrier barrier3 = {};
	barrier3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier3.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier3.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier3.image = DstTexture->Image;
	barrier3.subresourceRange = subresourceRange;
	barrier3.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier3.dstAccessMask = 0;
	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier3);

	VkImageMemoryBarrier ReturnMemoryBarrior{};
	ReturnMemoryBarrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ReturnMemoryBarrior.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	ReturnMemoryBarrior.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	ReturnMemoryBarrior.image = SrcTexture->Image;
	ReturnMemoryBarrior.subresourceRange = subresourceRange;
	ReturnMemoryBarrior.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	ReturnMemoryBarrior.dstAccessMask = 0;
	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &ReturnMemoryBarrior);

}
