#pragma once
#include <vector>
#include <memory>
#include "Texture2D.h"
#include "Texture3D.h"
#include "FontTexture.h"
#include "CubeMapTexture.h"
#include "RenderedRayTracedColorTexture.h"
#include "RenderedColorTexture.h"
#include "RenderedDepthTexture.h"
#include "RenderedCubeMapTexture.h"
#include "RenderedCubeMapDepthTexture.h"


class TextureManager
{
private:

	std::vector<std::shared_ptr<Texture3D>> Texture3DList;
	std::vector<std::shared_ptr<RenderedColorTexture>> RenderedColorTextureList;
	std::vector<std::shared_ptr<RenderedDepthTexture>> RenderedDepthTextureList;
	std::vector<std::shared_ptr<RenderedCubeMapTexture>> RenderedCubeMapTextureList;
	std::vector<std::shared_ptr<RenderedCubeMapDepthTexture>> RenderedCubeMapDepthTextureList;
	uint32_t IsTextureLoaded(std::string name);

	VkSampler NullSampler;
public:
	std::vector<std::shared_ptr<Texture2D>> Texture2DList;
	std::vector<std::shared_ptr<Texture>> CubeMapList;

	TextureManager();
	TextureManager(std::shared_ptr<VulkanEngine> Engine);
	~TextureManager();

	std::shared_ptr<Texture2D> LoadTexture2D(std::shared_ptr<Texture2D> texture);
	std::shared_ptr<Texture2D> LoadTexture2D(const std::string TextureLocation, VkFormat format);
	std::shared_ptr<Texture2D> LoadTexture2D(glm::ivec2 TextureResolution, std::vector<Pixel>& PixelList, VkFormat format);
	std::shared_ptr<Texture3D> LoadTexture3D(const std::string TextureLocation, VkFormat format);
	std::shared_ptr<Texture3D> LoadTexture3D(glm::ivec3& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format);
	std::shared_ptr<CubeMapTexture> LoadCubeMap(CubeMapLayout CubeMapFiles, VkFormat textureFormat);
	std::shared_ptr<CubeMapTexture> LoadCubeMap(std::string CubeMapFiles[6], VkFormat textureFormat);
	std::shared_ptr<CubeMapTexture> LoadCubeMap(std::string CubeMapLocation, VkFormat textureFormat);
	std::shared_ptr<CubeMapTexture> LoadCubeMap(std::shared_ptr<CubeMapTexture> cubeMapTexture);
	std::shared_ptr<RenderedColorTexture> LoadRenderedColorTexture(std::shared_ptr<RenderedColorTexture> renderedColorTexture);
	std::shared_ptr<RenderedDepthTexture> LoadRenderedDepthTexture(std::shared_ptr<RenderedDepthTexture> renderedDepthTexture);
	std::shared_ptr<RenderedCubeMapTexture> LoadRenderedCubeMapTexture(std::shared_ptr<RenderedCubeMapTexture> renderedCubeMapTexture);
	std::shared_ptr<RenderedCubeMapDepthTexture> LoadRenderedCubeMapDepthTexture(std::shared_ptr<RenderedCubeMapDepthTexture> renderedCubeMapDepthTexture);
	//std::shared_ptr<FontTexture> LoadFontTexture(void* GlyphData, uint32_t width, uint32_t height);

	void DeleteTexture2DByID(uint32_t TextureID);
	void DeleteTexture3DByID(uint32_t TextureID);
	void DeleteCubeMapTextureByID(uint32_t TextureID);
	void DeleteRenderedColorTextureByID(uint32_t TextureID);
	void DeleteRenderedDepthTextureByID(uint32_t TextureID);
	void DeleteRenderedCubeMapTextureByID(uint32_t TextureID);
	void DeleteRenderedCubeMapDepthTextureByID(uint32_t TextureID);

	void DeleteTexture2DByBufferIndex(uint32_t TextureBufferIndex);
	void DeleteTexture3DByBufferIndex(uint32_t TextureBufferIndex);
	void DeleteCubeMapTextureByBufferIndex(uint32_t TextureBufferIndex);
	void DeleteRenderedColorTextureByBufferIndex(uint32_t TextureBufferIndex);
	void DeleteRenderedDepthTextureByBufferIndex(uint32_t TextureBufferIndex);
	void DeleteRenderedCubeMapTextureByBufferIndex(uint32_t TextureBufferIndex);
	void DeleteRenderedCubeMapDepthTextureByBufferIndex(uint32_t TextureBufferIndex);

	std::shared_ptr<Texture2D> GetTexture2DByName(const std::string TextureName);
	std::shared_ptr<Texture3D> Get3DTextureByName(const std::string TextureName);

	std::shared_ptr<Texture2D> GetTextureByID(uint32_t TextureID);
	std::shared_ptr<Texture3D> GetTexture3DByID(uint32_t TextureID);
	std::shared_ptr<Texture> GetCubeMapTextureByID(uint32_t TextureID);
	std::shared_ptr<RenderedColorTexture> GetRenderedColorTextureByID(uint32_t TextureID);
	std::shared_ptr<RenderedDepthTexture> GetRenderedDepthTextureByID(uint32_t TextureID);
	std::shared_ptr<Texture> GetRenderedCubeMapTextureByID(uint32_t TextureID);
	std::shared_ptr<RenderedCubeMapDepthTexture> GetRenderedCubeMapDepthTextureByID(uint32_t TextureID);

	std::shared_ptr<Texture2D> GetTextureByBufferIndex(uint32_t TextureBufferIndex) { return Texture2DList[TextureBufferIndex]; };
	std::shared_ptr<Texture3D> GetTexture3DByBufferIndex(uint32_t TextureBufferIndex) { return Texture3DList[TextureBufferIndex]; };
	std::shared_ptr<Texture> GetCubeMapTextureByBufferIndex(uint32_t TextureBufferIndex) { return CubeMapList[TextureBufferIndex]; };
	std::shared_ptr<RenderedColorTexture> GetRenderedColorTextureByBufferIndex(uint32_t TextureBufferIndex) { return RenderedColorTextureList[TextureBufferIndex]; };
	std::shared_ptr<RenderedDepthTexture> GetRenderedDepthTextureByBufferIndex(uint32_t TextureBufferIndex) { return RenderedDepthTextureList[TextureBufferIndex]; };
	std::shared_ptr<Texture> GetRenderedCubeMapTextureByBufferIndex(uint32_t TextureBufferIndex) { return RenderedCubeMapTextureList[TextureBufferIndex]; };
	std::shared_ptr<RenderedCubeMapDepthTexture> GetRenderedCubeMapDepthTextureByBufferIndex(uint32_t TextureBufferIndex) { return RenderedCubeMapDepthTextureList[TextureBufferIndex]; };

	std::vector<std::shared_ptr<Texture2D>> GetAllTexture2D() { return Texture2DList; }
	std::vector<std::shared_ptr<Texture3D>> GetAllTexture3DList() { return Texture3DList; }
	std::vector<std::shared_ptr<Texture>> GetAllCubeMapTextures() { return CubeMapList; }

	void UpdateBufferIndex();
	void ClearTextures();
	void Destory();

	std::vector<VkDescriptorImageInfo>  GetTexture2DBufferDescriptorList();
	std::vector<VkDescriptorImageInfo>  GetTexture3DBufferDescriptorList();
	std::vector<VkDescriptorImageInfo>  GetCubeMapTextureBufferDescriptorList();
	std::vector<VkDescriptorImageInfo>  GetRenderedColorTextureBufferDescriptorList();
	std::vector<VkDescriptorImageInfo>  GetRenderedDepthTextureBufferDescriptorList();
	std::vector<VkDescriptorImageInfo>  GetRenderedCubeMapTextureBufferDescriptorList();
	std::vector<VkDescriptorImageInfo>  GetRenderedCubeMapDepthTextureBufferDescriptorList();

	uint32_t GetTextureBufferDescriptorCount();
	uint32_t Get3DTextureBufferDescriptorCount();
	uint32_t GetCubeMapBufferDescriptorCount();
	uint32_t GetRenderedColorTextureBufferCount();
	uint32_t GetRenderedDepthTextureBufferCount();
	uint32_t GetRenderedCubeMapTextureBufferCount();
	uint32_t GetRenderedCubeMapDepthTextureBufferCount();
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