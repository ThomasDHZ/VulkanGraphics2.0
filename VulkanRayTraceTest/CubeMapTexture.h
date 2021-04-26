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
	void LoadTexture(VulkanEngine& engine, CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	void CreateTextureView(VulkanEngine& engine, VkFormat textureFormat);
	void CreateTextureSampler(VulkanEngine& engine);
public:
	CubeMapTexture();
	CubeMapTexture(VulkanEngine& engine, CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	CubeMapTexture(VulkanEngine& engine, std::string CubeMapFiles[6], VkFormat textureFormat);
	~CubeMapTexture();
};