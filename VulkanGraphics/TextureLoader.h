#pragma once
#include "KTXTextureLoader.h"
#include "KTX2TextureLoader.h"
#include "DDSTextureLoader.h"
#include "STBILoader.h"
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION

enum TextureFormatEnum : unsigned int
{
	File_KTX = 1144346192,
	File_KTX2 = 2161254544,
	File_DDS = 2096260224,
};

static class TextureLoader
{
private:
public:
	static TextureInfo LoadTexture(const std::string TexturePath)
	{
		std::string FileFormat = TexturePath.substr(TexturePath.find_last_of(".") + 1);
		std::transform(FileFormat.begin(), FileFormat.end(), FileFormat.begin(), ::toupper);
		unsigned int FormatInt = *reinterpret_cast<unsigned int*>(&FileFormat);

		switch (FormatInt)
		{
		case File_KTX: return KTXTextureLoader::LoadKTXTexture(TexturePath); break;
		case File_KTX2: return KTX2TextureLoader::KTX2extureLoader(TexturePath); break;
		case File_DDS: return DDSTextureLoader::DDSSTextureLoader(TexturePath); break;
		}

		return TextureInfo();
	}

	static STBIContainer LoadSTBITexture(const std::string TexturePath)
	{
		return STBILoader::LoadSTBILoader(TexturePath);
	}
};

