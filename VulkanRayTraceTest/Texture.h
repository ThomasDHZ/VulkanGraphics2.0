#pragma once
#include <vulkan/vulkan.h>
#include "VulkanEngine.h"
#include "DDSTextureLoader.h"

enum TextureType
{
    vkTexture2D,
    vkTexture3D,
    vkTextureCube,
    vkRenderedTexture
};

class Texture
{
private:

protected:


    void KTXTransitionImageLayout(VulkanEngine& engine, VkImageLayout oldLayout, VkImageLayout newLayout);
    void KTXCopyBufferToImage(VulkanEngine& engine, VkBuffer buffer);
    void TransitionImageLayout(VulkanEngine& engine, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VulkanEngine& engine, VkBuffer buffer);

    virtual void LoadKTXTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format);
    virtual void LoadDDSTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format);
    virtual void LoadTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format);
    virtual void CreateTexture(VulkanEngine& engine, std::vector<Pixel>& Pixels, VkFormat format);
    virtual void CreateTexture3D(VulkanEngine& engine, std::vector<Pixel>& Pixels, VkFormat format);
    virtual void CreateTextureImage(VulkanEngine& engine, VkImageCreateInfo TextureInfo);
    void UpdateColorFormat(VulkanEngine& engine, VkCommandBuffer buffer, VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
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

    int Width;
    int Height;
    int Depth;

    Texture();
    Texture(uint32_t textureID, TextureType textureType);
    Texture(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format, uint32_t textureID, TextureType textureType);
    Texture(VulkanEngine& engine, int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format, uint32_t textureID, TextureType textureType);
    Texture(VulkanEngine& engine, std::string TextureLocation, uint32_t textureID, VkFormat format, TextureType textureType);
    Texture(VulkanEngine& engine, std::string TextureLocation, VkFormat format, TextureType textureType);
    Texture(VulkanEngine& engine, uint32_t textureID, TextureType textureType);
    Texture(VulkanEngine& engine, TextureType textureType);
    ~Texture();

    virtual void Delete(VulkanEngine& engine);
    VkImageView GetTextureView() { return View; }
    VkSampler GetTextureSampler() { return Sampler; }
};