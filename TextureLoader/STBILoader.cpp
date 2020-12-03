#include "STBILoader.h"
#include <stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

STBILoader::STBILoader()
{
}

STBILoader::~STBILoader()
{
}

TextureInfo STBILoader::LoadSTBILoader(const std::string& TextureLocation)
{
	int Width;
	int Height;
	int ColorChannels;
	stbi_uc* pixels = stbi_load(TextureLocation.c_str(), &Width, &Height, &ColorChannels, STBI_rgb_alpha);

	VkDeviceSize imageSize = Width * Height * 4;

	TextureInfo info = {};
	info.Width = Width;
	info.Height = Height;
	info.Depth = 1;
	info.ArrayLayers = 1;
	info.faces = 1;
	info.MipLevels = 1;
	info.ColorChannel = 4;
	info.BytesOfKeyValueData = 0;
	info.TextureSize = imageSize;
	info.TextureType = VK_IMAGE_TYPE_2D;

	return info;
}
