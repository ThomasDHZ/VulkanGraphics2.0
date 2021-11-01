#pragma once
#include <vulkan/vulkan.h>
#include "Vulkanengine.h"
#include "DDSTextureLoader.h"
#include "Pixel.h"

enum TextureType
{
    vkTexture2D,
    vkTexture2DArray,
    vkTexture3D,
    vkTextureCube,
    vkHeightMap,
    vkRenderedTexture,
    vkRenderedCubeMap
};

class Texture
{
private:

protected:
    void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VkBuffer buffer);
    void GenerateMipmaps(VkFormat imageFormat);
    void UpdateImageLayout(VkCommandBuffer buffer, VkImageLayout newImageLayout);

    virtual void LoadKTXTexture(std::string TextureLocation, VkFormat format);
    virtual void LoadDDSTexture(std::string TextureLocation, VkFormat format);
    virtual void LoadTexture(std::string TextureLocation, VkFormat format);
    virtual void CreateTexture(std::vector<Pixel>& Pixels, VkFormat format);
    virtual void CreateTexture(std::vector<glm::vec4>& Pixels, VkFormat format);
    virtual void CreateTexture3D(std::vector<Pixel>& Pixels, VkFormat format);
    virtual void CreateTextureImage(VkImageCreateInfo TextureInfo);
   
public:
    VkImage Image = VK_NULL_HANDLE;
    VkImageView View = VK_NULL_HANDLE;
    VkSampler Sampler = VK_NULL_HANDLE;
    VkDeviceMemory Memory = VK_NULL_HANDLE;
    VkFormat TextureFormat;
    VkDescriptorSet ImGuiDescriptorSet = VK_NULL_HANDLE;

    uint32_t TextureID = 0;
    uint32_t TextureBufferIndex = 0;
    std::string FileName;
    TextureType TypeOfTexture;
    VkImageLayout ImageLayout;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
    uint32_t MipMapLevels = 1;
    uint32_t LayerCount = 1;

    int Width;
    int Height;
    int Depth;

    Texture();
    Texture(TextureType textureType);

    Texture(glm::ivec2& TextureResolution, TextureType textureType);
    Texture(glm::ivec2& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType);
    Texture(glm::ivec2& TextureResolution, std::vector<glm::vec4>& PixelList, VkFormat format, TextureType textureType);

    Texture(glm::ivec3& TextureResolution, TextureType textureType);
    Texture(glm::ivec3& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType);

    Texture(std::string TextureLocation, VkFormat format, TextureType textureType);

    ~Texture();

    void UpdateImageLayout(VkImageLayout oldImageLayout, VkImageLayout newImageLayout, int facecount);
    void UpdateImageLayout(VkCommandBuffer& commandBuffer, VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
    void UpdateImageLayout(VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
    void UpdateDepthImageLayout(VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
    void UpdateCubeImageLayout(VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
    void UpdateCubeImageLayout(VkCommandBuffer& commandBuffer, VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
    void CopyTexture(VkCommandBuffer& commandBuffer, std::shared_ptr<Texture> CopyToTexture);
    void CopyTexture(VkCommandBuffer& commandBuffer, std::shared_ptr<Texture> CopyToTexture, int FaceCopy);
    virtual void Delete();
    VkImageView GetTextureView() { return View; }
    VkSampler GetTextureSampler() { return Sampler; }
};