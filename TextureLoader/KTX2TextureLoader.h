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

class KTX2TextureLoader
{
    //KTX2 Format Guide: https://github.khronos.org/KTX-Specification/
private:
    KTX2_Header Header;
    KTX2_Index Index;
    std::vector<KTX2_LevelIndex> LevelIndexList;
    KTX_DataFormatDescriptor DataFormatDescriptor;
    KTX2_KeyAndValueData KeyAndValueData;
    std::vector<byte> TextureData;
public:
    KTX2TextureLoader();
    ~KTX2TextureLoader();
    TextureInfo KTX2extureLoader(const std::string& TextureLocation);
};