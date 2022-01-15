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
	void LoadTexture(CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	void LoadTexture(std::string CubeMapLocation, VkFormat textureFormat);
	void CreateTextureView(VkFormat textureFormat);
	void CreateTextureSampler();
public:
	CubeMapTexture();
	CubeMapTexture(TextureType textureFormat);
	CubeMapTexture(glm::ivec2 TextureResolution, TextureType textureFormat);
	CubeMapTexture(CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	CubeMapTexture(std::string CubeMapFiles[6], VkFormat textureFormat);
	CubeMapTexture(std::string CubeMapLocation, VkFormat textureFormat);
	~CubeMapTexture();
};