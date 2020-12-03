#include "KTXTextureLoader.h"
#include "GLImageDefines.h"

KTXTextureLoader::KTXTextureLoader()
{

}

KTXTextureLoader::~KTXTextureLoader()
{
	
}

TextureInfo KTXTextureLoader::LoadKTXTexture(const std::string& TextureLocation)
{
	std::ifstream file(TextureLocation, std::ios::binary);
	file.read(reinterpret_cast<char*>(&Header), sizeof(Header));
	file.read(reinterpret_cast<char*>(&MetaData), sizeof(MetaData));

	TextureInfo info = {};
	info.Width = Header.pixelWidth;
	info.Height = Header.pixelHeight;
	info.Depth = Header.pixelDepth;
	info.ArrayLayers = Header.numberOfArrayElements;
	info.faces = Header.numberOfFaces;
	info.MipLevels = Header.numberOfMipmapLevels;
	info.BytesOfKeyValueData = Header.bytesOfKeyValueData;
	info.TextureSize = MetaData.TextureSize;
	info.TextureType = VK_IMAGE_TYPE_2D;

	if (Header.pixelDepth == 0)
	{
		info.Depth = 1;
	}
	if (Header.numberOfArrayElements == 0)
	{
		info.ArrayLayers = 1;
	}

	switch (Header.glFormat)
	{
	case GL_RED: info.ColorChannel = 1; break;
	case GL_GREEN: info.ColorChannel = 1; break;
	case GL_BLUE: info.ColorChannel = 1; break;
	case GL_ALPHA: info.ColorChannel = 1; break;
	case GL_RGB: info.ColorChannel = 3; break;
	case GL_RGBA: info.ColorChannel = 4; break;
	default: std::cout << "Format not supported?" << std::endl;
	};

	info.TextureData.resize(MetaData.TextureSize, 0xFF);
	file.read(reinterpret_cast<char*>(info.TextureData.data()), MetaData.TextureSize);
	file.close();
	return info;
}
