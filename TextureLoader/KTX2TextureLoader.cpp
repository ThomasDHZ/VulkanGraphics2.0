#include "KTX2TextureLoader.h"

KTX2TextureLoader::KTX2TextureLoader()
{

}

KTX2TextureLoader::~KTX2TextureLoader()
{

}

TextureInfo KTX2TextureLoader::KTX2extureLoader(const std::string& TextureLocation)
{
	std::ifstream file(TextureLocation, std::ios::binary);

	file.read(reinterpret_cast<char*>(&Header), sizeof(Header));
	file.read(reinterpret_cast<char*>(&Index), sizeof(Index));

	LevelIndexList.resize(Header.levelCount);
	file.read(reinterpret_cast<char*>(LevelIndexList.data()), sizeof(KTX2_LevelIndex) * LevelIndexList.size());

	file.read(reinterpret_cast<char*>(&DataFormatDescriptor.dfdTotalSize), sizeof(DataFormatDescriptor.dfdTotalSize));

	DataFormatDescriptor.dfdBlock.resize(DataFormatDescriptor.dfdTotalSize);
	file.read(reinterpret_cast<char*>(DataFormatDescriptor.dfdBlock.data()), sizeof(byte) * DataFormatDescriptor.dfdBlock.size() -  4);

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
