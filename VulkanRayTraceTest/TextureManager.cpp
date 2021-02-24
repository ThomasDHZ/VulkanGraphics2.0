#include "TextureManager.h"

TextureManager::TextureManager()
{
}

TextureManager::TextureManager(VulkanEngine& engine)
{
	LoadTexture(engine, "../texture/DefaultTexture.png", VK_FORMAT_R8G8B8A8_UNORM);
}

TextureManager::~TextureManager()
{
}

uint32_t TextureManager::LoadTexture(VulkanEngine& engine, const std::string TextureLocation, VkFormat format)
{
	//uint32_t TextureID = IsTextureLoaded(TextureLocation);
	//if (TextureID == -1)
	//{
		unsigned int TextureID = TextureList.size();
		TextureList.emplace_back(std::make_shared<Texture2D>(Texture2D(engine, TextureLocation, format, TextureID)));
	//}
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

uint32_t TextureManager::IsTextureLoaded(std::string name)
{
	uint32_t textureID = -1;
	for (auto texture : TextureList)
	{
		if (texture->FileName == name)
		{
			textureID = texture->TextureID;
			return textureID;
		}
	}

	return textureID;
}