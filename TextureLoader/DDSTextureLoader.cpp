#include "DDSTextureLoader.h"

DDSTextureLoader::DDSTextureLoader()
{
}

DDSTextureLoader::~DDSTextureLoader()
{
}

TextureInfo DDSTextureLoader::DDSSTextureLoader(const std::string& TextureLocation)
{

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
	if (DX10Header.miscFlag == 4)
	{
		//Somehow miscFlag means cubemap.
		info.faces = 6;
	}
	else
	{
		info.faces = 1;
	}
	info.MipLevels = Header.mipMapCount;
	info.BytesOfKeyValueData = Header.ddspf.RGBBitCount / 8;
	info.TextureSize = length - sizeof(Header);
	info.TextureType = VK_IMAGE_TYPE_2D;

	info.TextureData.resize(info.TextureSize);
	file.read(reinterpret_cast<char*>(info.TextureData.data()), info.TextureData.size());
	file.close();

	return info;
}