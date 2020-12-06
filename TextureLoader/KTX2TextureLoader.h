#pragma once
#include "Pixel.h"
#include "TextureInfo.h"
#include <vulkan/vulkan_core.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

struct KTX2_Header
{
    byte identifier[12];
    VkFormat vkFormat;
    unsigned int typeSize;
    unsigned int pixelWidth;
    unsigned int pixelHeight;
    unsigned int pixelDepth;
    unsigned int layerCount;
    unsigned int faceCount;
    unsigned int levelCount;
    unsigned int supercompressionScheme;
};
struct KTX2_Index
{
    unsigned int dfdByteOffset;
    unsigned int dfdByteLength;
    unsigned int kvdByteOffset;
    unsigned int kvdByteLength;
    uint64_t sgdByteOffset;
    uint64_t sgdByteLength;
};
struct KTX2_LevelIndex
{
    uint64_t byteOffset;
    uint64_t byteLength;
    uint64_t uncompressedByteLength;
};
struct KTX_DataFormatDescriptor
{
    unsigned int dfdTotalSize;
    std::vector<byte> dfdBlock;
};
struct KTX2_KeyAndValueData
{
    unsigned int        keyAndValueByteLength;
    std::vector<byte>   keyAndValue;
    unsigned int        Padding;
    unsigned int        Padding2;
};

static class KTX2TextureLoader
{
    //KTX2 Format Guide: https://github.khronos.org/KTX-Specification/
private:
public:
	static TextureInfo KTX2extureLoader(const std::string& TextureLocation)
	{
		 KTX2_Header Header;
		 KTX2_Index Index;
		 std::vector<KTX2_LevelIndex> LevelIndexList;
		 KTX_DataFormatDescriptor DataFormatDescriptor;
		 KTX2_KeyAndValueData KeyAndValueData;
		 std::vector<byte> TextureData;

		std::ifstream file(TextureLocation, std::ios::binary);

		file.read(reinterpret_cast<char*>(&Header), sizeof(Header));
		file.read(reinterpret_cast<char*>(&Index), sizeof(Index));

		LevelIndexList.resize(Header.levelCount);
		file.read(reinterpret_cast<char*>(LevelIndexList.data()), sizeof(KTX2_LevelIndex) * LevelIndexList.size());

		file.read(reinterpret_cast<char*>(&DataFormatDescriptor.dfdTotalSize), sizeof(DataFormatDescriptor.dfdTotalSize));

		DataFormatDescriptor.dfdBlock.resize(DataFormatDescriptor.dfdTotalSize);
		file.read(reinterpret_cast<char*>(DataFormatDescriptor.dfdBlock.data()), sizeof(byte) * DataFormatDescriptor.dfdBlock.size() - 4);

		file.read(reinterpret_cast<char*>(&KeyAndValueData.keyAndValueByteLength), sizeof(byte) * sizeof(KeyAndValueData.keyAndValueByteLength));
		KeyAndValueData.keyAndValue.resize(KeyAndValueData.keyAndValueByteLength);
		file.read(reinterpret_cast<char*>(KeyAndValueData.keyAndValue.data()), sizeof(byte) * KeyAndValueData.keyAndValue.size());
		//file.read(reinterpret_cast<char*>(&KeyAndValueData.Padding), sizeof(KeyAndValueData.Padding));
		//file.read(reinterpret_cast<char*>(&KeyAndValueData.Padding2), sizeof(KeyAndValueData.Padding2));

		TextureInfo info = {};
		info.Width = Header.pixelWidth;
		info.Height = Header.pixelHeight;
		info.Depth = Header.pixelDepth;
		info.ArrayLayers = Header.layerCount;
		info.faces = Header.faceCount;
		info.MipLevels = Header.levelCount;
		info.BytesOfKeyValueData = 4;
		info.TextureSize = LevelIndexList[0].byteLength;
		info.TextureType = VK_IMAGE_TYPE_2D;

		if (Header.pixelDepth == 0)
		{
			info.Depth = 1;
		}
		if (Header.layerCount == 0)
		{
			info.ArrayLayers = 1;
		}

		info.TextureData.resize(LevelIndexList[0].byteLength);
		file.read(reinterpret_cast<char*>(info.TextureData.data()), info.TextureData.size());
		file.close();

		return info;
	}
};