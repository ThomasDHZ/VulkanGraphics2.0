#pragma once
#include <vector>
#include <memory>
#include "Texture2D.h"
#include "Texture3D.h"
#include "CubeMapTexture.h"
#include "RenderedRayTracedColorTexture.h"

class TextureManager
{
private:
	CubeMapTexture CubeMap;

	void UpdateTexture2DBufferIndex(VulkanEngine& engine);
	uint32_t IsTextureLoaded(std::string name);

public:
	std::vector<std::shared_ptr<Texture2D>> TextureList;
	std::vector<std::shared_ptr<Texture3D>> Texture3DList;

	TextureManager();
	TextureManager(VulkanEngine& engine);
	~TextureManager();

	std::shared_ptr<Texture2D> LoadTexture2D(VulkanEngine& engine, const std::string TextureLocation, VkFormat format);
	uint32_t LoadTexture2D(std::shared_ptr<RenderedRayTracedColorTexture> RenderedTexture);
	uint32_t LoadTexture2D(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format);
	uint32_t Load3DTexture(VulkanEngine& engine, const std::string TextureLocation, VkFormat format);
	uint32_t LoadTexture3D(VulkanEngine& engine, int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format);
	void DeleteTexture(VulkanEngine& engine, uint32_t TextureBufferIndex);
	void LoadCubeMap(VulkanEngine& engine, CubeMapLayout CubeMapFiles);
	void LoadCubeMap(VulkanEngine& engine, std::string CubeMapFiles[6]);
	void UnloadAllTextures(VulkanEngine& engine);
	void UnloadCubeMap(VulkanEngine& engine);
	void Destory(VulkanEngine& engine);

	std::shared_ptr<Texture2D> GetTextureByName(const std::string TextureName);
	std::shared_ptr<Texture3D> Get3DTextureByName(const std::string TextureName);

	std::shared_ptr<Texture2D> GetTextureByBufferIndex(unsigned int TextureBufferIndex) { return TextureList[TextureBufferIndex]; };
	std::shared_ptr<Texture3D> Get3DTexture(unsigned int TextureBufferIndex) { return Texture3DList[TextureBufferIndex]; };
	std::vector<std::shared_ptr<Texture2D>> GetTextureList() { return TextureList; }
	std::vector<std::shared_ptr<Texture3D>> Get3DTextureList() { return Texture3DList; }
	CubeMapTexture GetCubeMapTexture() { return CubeMap; }

	std::vector<VkDescriptorImageInfo>  GetTextureBufferListDescriptor();
	std::vector<VkDescriptorImageInfo>  Get3DTextureBufferListDescriptor();
	VkDescriptorImageInfo  GetSkyBoxTextureBufferListDescriptor();

	uint32_t GetTextureBufferDescriptorCount() { return TextureList.size(); }
	uint32_t Get3DTextureBufferDescriptorCount() { return TextureList.size(); }
};
