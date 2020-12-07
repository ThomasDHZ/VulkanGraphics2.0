#pragma once
#include "TextureInfo.h"
#include <string>
#include <stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

struct STBIContainer
{
	int Width;
	int Height;
	int ColorChannels;
	int TextureSize;
	stbi_uc* TextureData;
};

static class STBILoader
{
private:
public:
	static STBIContainer LoadSTBILoader(const std::string& TextureLocation)
	{
		STBIContainer TextureContainer;
		TextureContainer.TextureData = stbi_load(TextureLocation.c_str(), &TextureContainer.Width, &TextureContainer.Height, &TextureContainer.ColorChannels, STBI_rgb_alpha);
		TextureContainer.TextureSize = TextureContainer.Width * TextureContainer.Height * 4;
		return TextureContainer;
	}
};

