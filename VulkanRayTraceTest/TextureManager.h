#pragma once
#include <vector>
#include <memory>
#include "Texture2D.h"
#include "Texture3D.h"
#include "FontTexture.h"
#include "CubeMapTexture.h"
#include "RenderedRayTracedColorTexture.h"

class TextureManager
{
private:
	void UpdateTexture2DBufferIndex(VulkanEngine& engine);
	uint32_t IsTextureLoaded(std::string name);

public:
	std::vector<std::shared_ptr<Texture>> TextureList;
	std::vector<std::shared_ptr<Texture3D>> Texture3DList;
	std::shared_ptr<Texture> CubeMap;
	TextureManager();
	TextureManager(VulkanEngine& engine);
	~TextureManager();

	std::shared_ptr<Texture2D> LoadTexture2D(VulkanEngine& engine, const std::string TextureLocation, VkFormat format);
	std::shared_ptr<Texture2D> LoadTexture2D(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format);
	uint32_t LoadTexture2D(std::shared_ptr<Texture> RenderedTexture);
	std::shared_ptr<FontTexture> LoadTextTexture(VulkanEngine& engine, void* GlyphData, uint32_t width, uint32_t height);
	uint32_t Load3DTexture(VulkanEngine& engine, const std::string TextureLocation, VkFormat format);
	uint32_t LoadTexture3D(VulkanEngine& engine, int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format);
	void DeleteTexture(VulkanEngine& engine, uint32_t TextureBufferIndex);
	void LoadCubeMap(VulkanEngine& engine, CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	void LoadCubeMap(VulkanEngine& engine, std::string CubeMapFiles[6], VkFormat textureFormat);
	void LoadCubeMap(VulkanEngine& engine, std::string CubeMapLocation, VkFormat textureFormat);
	void LoadCubeMap(VulkanEngine& engine, std::shared_ptr<Texture> cubeMapTexture);
	void UnloadAllTextures(VulkanEngine& engine);
	void UnloadCubeMap(VulkanEngine& engine);
	void Update(VulkanEngine& engine);
	void Destory(VulkanEngine& engine);



	std::shared_ptr<Texture> GetTextureByName(const std::string TextureName);
	std::shared_ptr<Texture3D> Get3DTextureByName(const std::string TextureName);

	std::shared_ptr<Texture> GetTextureByBufferIndex(unsigned int TextureBufferIndex) { return TextureList[TextureBufferIndex]; };
	std::shared_ptr<Texture> Get3DTexture(unsigned int TextureBufferIndex) { return Texture3DList[TextureBufferIndex]; };
	std::vector<std::shared_ptr<Texture>> GetTextureList() { return TextureList; }
	std::vector<std::shared_ptr<Texture3D>> Get3DTextureList() { return Texture3DList; }
    std::shared_ptr<Texture> GetCubeMapTexture() { return CubeMap; }

	std::vector<VkDescriptorImageInfo>  GetTextureBufferListDescriptor();
	std::vector<VkDescriptorImageInfo>  Get3DTextureBufferListDescriptor();
	VkDescriptorImageInfo  GetSkyBoxTextureBufferListDescriptor();

	uint32_t GetTextureBufferDescriptorCount();
	uint32_t Get3DTextureBufferDescriptorCount();
};
