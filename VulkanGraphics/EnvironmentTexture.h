#pragma once
#include "Texture.h"
#include "CubeMapTexture.h";

class EnvironmentTexture : public Texture
{
private:
	void LoadTexture(std::string TextureLocation, VkFormat textureFormat);
	void CreateTextureView(VkFormat textureFormat);
	void CreateTextureSampler();
public:
	EnvironmentTexture();
	EnvironmentTexture(std::string TextureLocation, VkFormat format);
	~EnvironmentTexture();
};
