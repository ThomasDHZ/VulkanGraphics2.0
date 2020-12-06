#pragma once
#include <vulkan/vulkan.h>
#include "VulkanEngine.h"
#include "VulkanBufferManager.h"
#include <TextureInfo.h>

enum TextureType
{
    vkTexture2D,
    vkTextureCube,
    vkRenderedTexture
};

class Texture
{
private:

protected:
    TextureInfo TextureData;
    VkDeviceMemory Memory = VK_NULL_HANDLE;
    VkImage Image = VK_NULL_HANDLE;
    VkSampler Sampler = VK_NULL_HANDLE;

    void KTXTransitionImageLayout(VulkanEngine& engine, VkImageLayout oldLayout, VkImageLayout newLayout);
    void KTXCopyBufferToImage(VulkanEngine& engine, VkBuffer buffer);
    void TransitionImageLayout(VulkanEngine& engine, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VulkanEngine& engine, VkBuffer buffer);


    virtual void LoadKTXTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format);
    virtual void LoadTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format);
    virtual void CreateTexture(VulkanEngine& engine, std::vector<Pixel>& Pixels, VkFormat format);
    virtual void CreateTextureImage(VulkanEngine& engine, VkImageCreateInfo TextureInfo);
    virtual void CreateTextureView(VulkanEngine& engine, VkImageViewCreateInfo TextureImageViewInfo);
    virtual void CreateTextureSampler(VulkanEngine& engine, VkSamplerCreateInfo TextureImageSamplerInfo);

public:
    VkImageView View = VK_NULL_HANDLE;
    VkDescriptorSet ImGuiDescriptorSet = VK_NULL_HANDLE;

    unsigned int TextureID = 0;
    std::string FileName;
    TextureType TypeOfTexture;

    int Width;
    int Height;

    Texture();
    Texture(VulkanEngine& engine, std::string TextureLocation, unsigned int textureID, TextureType textureType, VkFormat format);
    Texture(VulkanEngine& engine, std::string TextureLocation, TextureType textureType, VkFormat format);
    Texture(VulkanEngine& engine, unsigned int textureID, TextureType textureType);
    Texture(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, TextureType textureType, VkFormat format);
    Texture(VulkanEngine& engine, TextureType textureType);
    ~Texture();

    virtual void Delete(VulkanEngine& engine);

    VkImageView GetTextureView() { return View; }
    VkSampler GetTextureSampler() { return Sampler; }
};