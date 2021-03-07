#pragma once
#include <vector>
#include <memory>
#include "Texture2D.h"
#include "Texture3D.h"
#include "CubeMapTexture.h"

class TextureManager
{
private:
	std::vector<std::shared_ptr<Texture2D>> TextureList;
	std::vector<std::shared_ptr<Texture3D>> Texture3DList;
	CubeMapTexture CubeMap;

	uint32_t IsTextureLoaded(std::string name);

public:
	TextureManager();
	TextureManager(VulkanEngine& engine);
	~TextureManager();

	uint32_t LoadTexture(VulkanEngine& engine, const std::string TextureLocation, VkFormat format);
	uint32_t Load3DTexture(VulkanEngine& engine, const std::string TextureLocation, VkFormat format);
	void LoadCubeMap(VulkanEngine& engine, CubeMapLayout CubeMapFiles);
	void LoadCubeMap(VulkanEngine& engine, std::string CubeMapFiles[6]);
	void UnloadAllTextures(VulkanEngine& engine);
	void UnloadCubeMap(VulkanEngine& engine);
	void Destory(VulkanEngine& engine);

	std::shared_ptr<Texture2D> GetTexture(unsigned int TextureID) { return TextureList[TextureID]; };
	std::shared_ptr<Texture3D> Get3DTexture(unsigned int TextureID) { return Texture3DList[TextureID]; };
	std::vector<std::shared_ptr<Texture2D>> GetTextureList() { return TextureList; }
	std::vector<std::shared_ptr<Texture3D>> Get3DTextureList() { return Texture3DList; }
	CubeMapTexture GetCubeMapTexture() { return CubeMap; }
};
