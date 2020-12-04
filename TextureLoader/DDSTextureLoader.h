#pragma once
#include "Pixel.h"
#include "TextureInfo.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

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

class DDSTextureLoader
{
    //DDS Format Guide: https://docs.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide
private:
    DDS_HEADER Header;
    DDS_HEADER_DXT10 DX10Header;
public:
    DDSTextureLoader();
    ~DDSTextureLoader();
    TextureInfo DDSSTextureLoader(const std::string& TextureLocation);
};