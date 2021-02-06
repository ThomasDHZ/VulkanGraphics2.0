#pragma once
#include "Texture.h"

struct CubeMapLayout
{
	std::string Front;
	std::string Back;
	std::string Top;
	std::string Bottom;
	std::string Right;
	std::string Left;
};

class CubeMapTexture : public Texture
{
private:
	void LoadTexture(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, CubeMapLayout CubeMapFiles);
	void CreateTextureView(VkDevice& device);
	void CreateTextureSampler(VkDevice& device);
public:
	CubeMapTexture();
	CubeMapTexture(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, CubeMapLayout CubeMapFiles, unsigned int TextureID);
	CubeMapTexture(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, std::string CubeMapFiles[6], unsigned int TextureID);
	~CubeMapTexture();
};