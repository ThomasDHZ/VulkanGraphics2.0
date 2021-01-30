#pragma once
#include <vector>
#include <memory>
#include "Texuture2D.h"
#include "CubeMapTexture.h"
class TextureManager
{
private:
	std::vector<std::shared_ptr<Texture2D>> TextureList;
	CubeMapTexture CubeMap;
public:
	TextureManager();
	~TextureManager();

	unsigned int LoadTexture(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string TextureLocation, VkFormat format);
	void LoadCubeMap(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, CubeMapLayout CubeMapFiles);
	void LoadCubeMap(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, std::string CubeMapFiles[6]);

	std::shared_ptr<Texture2D> GetTexture(unsigned int TextureID) { return TextureList[TextureID]; };
	std::vector<std::shared_ptr<Texture2D>> GetTextureList() { return TextureList; }
	CubeMapTexture GetCubeMapTexture() { return CubeMap; }
};

