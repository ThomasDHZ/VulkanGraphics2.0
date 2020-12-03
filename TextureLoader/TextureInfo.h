#pragma once
#include "Pixel.h"
#include <vulkan/vulkan_core.h>
#include <vector>

struct TextureInfo
{
	VkImageType TextureType;
	unsigned int Width;
	unsigned int Height;
	unsigned int Depth;
	unsigned int MipLevels;
	unsigned int ArrayLayers;
	unsigned int faces;
	unsigned int ColorChannel;
	unsigned int BytesOfKeyValueData;
	unsigned int TextureSize;
	std::vector<byte> TextureData;
};