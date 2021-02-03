#include "TextureManager.h"

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

unsigned int TextureManager::LoadTexture(VulkanEngine& engine, const std::string TextureLocation, VkFormat format)
{
	unsigned int TextureID = TextureList.size();
	TextureList.emplace_back(std::make_shared<Texture2D>(Texture2D(engine, TextureLocation, format, TextureID)));
	return TextureID;
}

void TextureManager::LoadCubeMap(VulkanEngine& engine, CubeMapLayout CubeMapFiles)
{
	CubeMap = CubeMapTexture(engine, CubeMapFiles, 0);
}

void TextureManager::LoadCubeMap(VulkanEngine& engine, std::string CubeMapFiles[6])
{
	CubeMap = CubeMapTexture(engine, CubeMapFiles, 0);
}

void TextureManager::UnloadAllTextures(VulkanEngine& engine)
{
	for (auto& texture : TextureList)
	{
		texture->Delete(engine);
	}
}

void TextureManager::UnloadCubeMap(VulkanEngine& engine)
{
	CubeMap.Delete(engine);
}

void TextureManager::Destory(VulkanEngine& engine)
{
	UnloadAllTextures(engine);
	UnloadCubeMap(engine);
}