#pragma once
#include "Pixel.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <vulkan/vulkan_core.h>

struct DDS_PIXELFORMAT
{
	unsigned int  size;
	unsigned int  flags;
	unsigned int  fourCC;
	unsigned int  RGBBitCount;
	unsigned int  RBitMask;
	unsigned int  GBitMask;
	unsigned int  BBitMask;
	unsigned int  ABitMask;
};

struct DDS_HEADER
{
	unsigned int    dwMagic;
	unsigned int    size;
	unsigned int    flags;
	unsigned int    height;
	unsigned int    width;
	unsigned int    pitchOrLinearSize;
	unsigned int    depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
	unsigned int    mipMapCount;
	unsigned int    reserved1[11];
	DDS_PIXELFORMAT ddspf;
	unsigned int    caps;
	unsigned int    caps2;
	unsigned int    caps3;
	unsigned int    caps4;
	unsigned int    reserved2;
};

struct DDS_HEADER_DXT10
{
	unsigned int dxgiFormat;
	unsigned int      resourceDimension;
	unsigned int      miscFlag; // See D3D11_RESOURCE_MISC_FLAG
	unsigned int      arraySize;
	unsigned int      reserved;
};

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

static class DDSTextureLoader
{
	//DDS Format Guide: https://docs.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide
private:
public:
	static TextureInfo DDSSTextureLoader(const std::string& TextureLocation)
	{
		DDS_HEADER Header;
		DDS_HEADER_DXT10 DX10Header;

		std::ifstream file(TextureLocation, std::ios::ate | std::ios::binary);

		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);

		file.read(reinterpret_cast<char*>(&Header), sizeof(Header));

		int HeaderSize = sizeof(Header);
		if (std::string(reinterpret_cast<char*>(&Header.ddspf.fourCC)) == "DX10")
		{
			file.read(reinterpret_cast<char*>(&DX10Header), sizeof(DX10Header));
			HeaderSize += sizeof(DX10Header);
		}

		TextureInfo info = {};
		info.Width = Header.width;
		info.Height = Header.height;
		info.Depth = Header.depth;
		info.ArrayLayers = 1;
		info.faces = 1;
		info.MipLevels = Header.mipMapCount;
		info.BytesOfKeyValueData = Header.ddspf.RGBBitCount / 8;
		info.TextureSize = length - sizeof(Header);
		info.TextureType = VK_IMAGE_TYPE_3D;

		info.TextureData.resize(info.TextureSize);
		file.read(reinterpret_cast<char*>(info.TextureData.data()), info.TextureData.size());
		file.close();

		return info;
	}
};