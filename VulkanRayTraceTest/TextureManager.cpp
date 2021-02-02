#include "TextureManager.h"

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

unsigned int TextureManager::LoadTexture(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string TextureLocation, VkFormat format)
{
	unsigned int TextureID = TextureList.size();
	TextureList.emplace_back(std::make_shared<Texture2D>(Texture2D(device, physcialDevice, commandPool, graphicsQueue, TextureLocation, format, TextureID)));
	return TextureID;
}

void TextureManager::LoadCubeMap(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, CubeMapLayout CubeMapFiles)
{
	CubeMap = CubeMapTexture(device, physcialDevice, commandPool, graphicsQueue, CubeMapFiles, 0);
}

void TextureManager::LoadCubeMap(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, std::string CubeMapFiles[6])
{
	CubeMap = CubeMapTexture(device, physcialDevice, commandPool, graphicsQueue, CubeMapFiles, 0);
}

void TextureManager::UnloadAllTextures(VkDevice& device)
{
	for (auto& texture : TextureList)
	{
		texture->Delete(device);
	}
}

void TextureManager::UnloadCubeMap(VkDevice& device)
{
	CubeMap.Delete(device);
}

void TextureManager::Destory(VkDevice& device)
{
	UnloadAllTextures(device);
	UnloadCubeMap(device);
}