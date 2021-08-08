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
	std::shared_ptr<VulkanEngine> engine;

	void UpdateTexture2DBufferIndex(std::shared_ptr<VulkanEngine> engine);
	uint32_t IsTextureLoaded(std::string name);

	VkSampler NullSampler;
public:
	std::vector<std::shared_ptr<Texture>> TextureList;
	std::vector<std::shared_ptr<Texture3D>> Texture3DList;
	std::shared_ptr<Texture> CubeMap;
	TextureManager();
	TextureManager(std::shared_ptr<VulkanEngine> Engine);
	~TextureManager();

	std::shared_ptr<Texture2D> LoadTexture2D(const std::string TextureLocation, VkFormat format);
	std::shared_ptr<Texture2D> LoadTexture2D(unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format);
	uint32_t LoadTexture2D(std::shared_ptr<Texture> RenderedTexture);
	std::shared_ptr<FontTexture> LoadTextTexture(void* GlyphData, uint32_t width, uint32_t height);
	uint32_t Load3DTexture(const std::string TextureLocation, VkFormat format);
	uint32_t LoadTexture3D(int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format);
	void DeleteTexture(uint32_t TextureBufferIndex);
	void LoadCubeMap(CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	void LoadCubeMap(std::string CubeMapFiles[6], VkFormat textureFormat);
	void LoadCubeMap(std::string CubeMapLocation, VkFormat textureFormat);
	void LoadCubeMap(std::shared_ptr<Texture> cubeMapTexture);
	void UnloadAllTextures();
	void UnloadCubeMap();
	void Update();
	void Destory();


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

class TextureManagerPtr
{
private:
	static std::shared_ptr<TextureManager> textureManager;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine)
	{
		textureManager = std::make_shared<TextureManager>(TextureManager(engine));
	}

	static std::shared_ptr<TextureManager> GetTextureManagerPtr()
	{
		return textureManager;
	}
};