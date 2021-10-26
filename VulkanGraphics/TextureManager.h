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
	std::vector<std::shared_ptr<Texture2D>> Texture2DList;
	std::vector<std::shared_ptr<Texture3D>> Texture3DList;
	std::vector<std::shared_ptr<Texture>> CubeMapList;

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
	//std::shared_ptr<FontTexture> LoadFontTexture(void* GlyphData, uint32_t width, uint32_t height);
	std::shared_ptr<Texture> LoadCubeMap(CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	std::shared_ptr<Texture> LoadCubeMap(std::string CubeMapFiles[6], VkFormat textureFormat);
	std::shared_ptr<Texture> LoadCubeMap(std::string CubeMapLocation, VkFormat textureFormat);
	std::shared_ptr<Texture> LoadCubeMap(std::shared_ptr<Texture> cubeMapTexture);
	void DeleteTexture2DByID(uint32_t Texture2DID);
	void DeleteTexture3DByID(uint32_t Texture3DID);
	void DeleteTexture2DByBufferIndex(uint32_t Texture2DBufferIndex);
	void DeleteTexture3DByBufferIndex(uint32_t Texture3DBufferIndex);

	std::shared_ptr<Texture2D> GetTexture2DByName(const std::string TextureName);
	std::shared_ptr<Texture3D> Get3DTextureByName(const std::string TextureName);
	std::shared_ptr<Texture2D> GetTextureByID(uint32_t TextureID);
	std::shared_ptr<Texture3D> GetTexture3DByID(uint32_t TextureID);
	std::shared_ptr<Texture2D> GetTextureByBufferIndex(unsigned int TextureBufferIndex) { return Texture2DList[TextureBufferIndex]; };
	std::shared_ptr<Texture> GetTexture3DByBufferIndex(unsigned int Texture3DBufferIndex) { return Texture3DList[Texture3DBufferIndex]; };
	std::shared_ptr<Texture> Get3DTexture(unsigned int TextureBufferIndex) { return Texture3DList[TextureBufferIndex]; };
	std::shared_ptr<Texture> GetCubeMapTexture(uint32_t TextureBufferIndex) { return CubeMapList[TextureBufferIndex]; }
	std::vector<std::shared_ptr<Texture>> GetAllCubeMapTextures() { return CubeMapList; }

	void UnloadAllTextures();
	void UnloadCubeMap();
	void UpdateBufferIndex();
	void Destory();

	std::vector<VkDescriptorImageInfo>  GetTextureBufferListDescriptor();
	std::vector<VkDescriptorImageInfo>  Get3DTextureBufferListDescriptor();
	std::vector<VkDescriptorImageInfo>  GetSkyBoxTextureBufferListDescriptor();

	uint32_t GetTextureBufferDescriptorCount();
	uint32_t Get3DTextureBufferDescriptorCount();
	uint32_t GetCubeMapBufferDescriptorCount();
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