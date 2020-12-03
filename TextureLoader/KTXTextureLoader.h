#pragma once
#include "Pixel.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "TextureInfo.h"

struct KTX_Header
{
    byte identifier[12];
    unsigned int endianness;
    unsigned int glType;
    unsigned int glTypeSize;
    unsigned int glFormat;
    unsigned int glInternalFormat;
    unsigned int glBaseInternalFormat;
    unsigned int pixelWidth;
    unsigned int pixelHeight;
    unsigned int pixelDepth;
    unsigned int numberOfArrayElements;
    unsigned int numberOfFaces;
    unsigned int numberOfMipmapLevels;
    unsigned int bytesOfKeyValueData;
    unsigned int keyAndValueByteSize;
};

struct KTX_MetaData
{
    byte identifier3[27];
    unsigned int TextureSize;
};

class KTXTextureLoader
{
    //KTX Formate Guide: https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
private:
    KTX_Header Header;
    KTX_MetaData MetaData;
public:
    KTXTextureLoader();
    ~KTXTextureLoader();
    TextureInfo LoadKTXTexture(const std::string& TextureLocation);
    
};
