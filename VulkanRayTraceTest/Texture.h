#pragma once
#include <vulkan/vulkan.h>
#include "VulkanEngine.h"
#include "DDSTextureLoader.h"

enum TextureType
{
    vkTexture2D,
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
    void TransitionImageLayout(VulkanEngine& engine, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VulkanEngine& engine, VkBuffer buffer);

    virtual void LoadKTXTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format);
    virtual void LoadDDSTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format);
    virtual void LoadTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format);
    virtual void CreateTexture(VulkanEngine& engine, std::vector<Pixel>& Pixels, VkFormat format);
    virtual void CreateTexture3D(VulkanEngine& engine, std::vector<Pixel>& Pixels, VkFormat format);
    virtual void CreateTextureImage(VulkanEngine& engine, VkImageCreateInfo TextureInfo);
   
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

    int Width;
    int Height;
    int Depth;

    Texture();
    Texture(VulkanEngine& engine, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    Texture(VulkanEngine& engine, glm::vec2& TextureResolution, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    Texture(VulkanEngine& engine, unsigned int width, unsigned int height, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    Texture(VulkanEngine& engine, glm::vec3& TextureResolution, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    Texture(VulkanEngine& engine, unsigned int width, unsigned int height, unsigned int depth, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    Texture(VulkanEngine& engine, glm::vec2& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    Texture(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    Texture(VulkanEngine& engine, glm::vec3& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    Texture(VulkanEngine& engine, int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    Texture(VulkanEngine& engine, std::string TextureLocation, VkFormat format, TextureType textureType, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    ~Texture();

    void Update(VulkanEngine& engine, uint32_t NewTextureBufferIndex);
    void UpdateImageLayout(VulkanEngine& engine, VkCommandBuffer buffer, VkImageLayout newImageLayout);
    virtual void Delete(VulkanEngine& engine);
    VkImageView GetTextureView() { return View; }
    VkSampler GetTextureSampler() { return Sampler; }
};