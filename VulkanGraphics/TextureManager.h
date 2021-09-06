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
	std::vector<std::shared_ptr<Texture>> TextureList;
	std::vector<std::shared_ptr<Texture3D>> Texture3DList;
	std::shared_ptr<Texture> CubeMap;

	uint32_t IsTextureLoaded(std::string name);

	VkSampler NullSampler;
public:

	TextureManager();
	TextureManager(std::shared_ptr<VulkanEngine> Engine);
	~TextureManager();

	std::shared_ptr<Texture2D> LoadTexture2D(std::shared_ptr<Texture2D> texture);
	std::shared_ptr<Texture2D> LoadTexture2D(const std::string TextureLocation, VkFormat format);
	std::shared_ptr<Texture2D> LoadTexture2D(glm::ivec2 TextureResolution, std::vector<Pixel>& PixelList, VkFormat format);
	std::shared_ptr<Texture3D> LoadTexture3D(const std::string TextureLocation, VkFormat format);
	std::shared_ptr<Texture3D> LoadTexture3D(glm::ivec3& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format);
	std::shared_ptr<FontTexture> LoadFontTexture(void* GlyphData, uint32_t width, uint32_t height);
	void LoadCubeMap(CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	void LoadCubeMap(std::string CubeMapFiles[6], VkFormat textureFormat);
	void LoadCubeMap(std::string CubeMapLocation, VkFormat textureFormat);
	void LoadCubeMap(std::shared_ptr<Texture> cubeMapTexture);
	void DeleteTexture2DByID(uint32_t Texture2DID);
	void DeleteTexture3DByID(uint32_t Texture3DID);
	void DeleteTexture2DByBufferIndex(uint32_t Texture2DBufferIndex);
	void DeleteTexture3DByBufferIndex(uint32_t Texture3DBufferIndex);

	std::shared_ptr<Texture> GetTextureByName(const std::string TextureName);
	std::shared_ptr<Texture3D> Get3DTextureByName(const std::string TextureName);
	std::shared_ptr<Texture> GetTextureByID(uint32_t TextureID);
	std::shared_ptr<Texture3D> GetTexture3DByID(uint32_t TextureID);
	std::shared_ptr<Texture> GetTextureByBufferIndex(unsigned int TextureBufferIndex) { return TextureList[TextureBufferIndex]; };
	std::shared_ptr<Texture> GetTexture3DByBufferIndex(unsigned int Texture3DBufferIndex) { return Texture3DList[Texture3DBufferIndex]; };
	std::shared_ptr<Texture> Get3DTexture(unsigned int TextureBufferIndex) { return Texture3DList[TextureBufferIndex]; };
	std::shared_ptr<Texture> GetCubeMapTexture() { return CubeMap; }

	void UnloadAllTextures();
	void UnloadCubeMap();
	void UpdateBufferIndex();
	void Destory();

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
		if (textureManager == nullptr)
		{
			textureManager = std::make_shared<TextureManager>(TextureManager(engine));
		}
		else
		{
			std::cout << "Texture Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<TextureManager> GetTextureManagerPtr()
	{
		return textureManager;
	}
};