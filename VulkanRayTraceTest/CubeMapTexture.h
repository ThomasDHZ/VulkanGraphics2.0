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
	void LoadTexture(std::shared_ptr<VulkanEngine> engine, CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	void LoadTexture(std::shared_ptr<VulkanEngine> engine, std::string CubeMapLocation, VkFormat textureFormat);
	void CreateTextureView(std::shared_ptr<VulkanEngine> engine, VkFormat textureFormat);
	void CreateTextureSampler(std::shared_ptr<VulkanEngine> engine);
public:
	CubeMapTexture();
	CubeMapTexture(std::shared_ptr<VulkanEngine> engine, CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	CubeMapTexture(std::shared_ptr<VulkanEngine> engine, std::string CubeMapFiles[6], VkFormat textureFormat);
	CubeMapTexture(std::shared_ptr<VulkanEngine> engine, std::string CubeMapLocation, VkFormat textureFormat);
	~CubeMapTexture();
};