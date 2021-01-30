#pragma once
#include <vulkan/vulkan.h>
#include <iostream>

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
    VkDeviceMemory Memory = VK_NULL_HANDLE;
    VkSampler Sampler = VK_NULL_HANDLE;

    void TransitionImageLayout(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer buffer);


    virtual void LoadTexture(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, std::string TextureLocation, VkFormat format);
    virtual void CreateTextureImage(VkDevice& device, VkPhysicalDevice& physcialDevice, VkImageCreateInfo TextureInfo);

public:
    VkImage Image = VK_NULL_HANDLE;
    VkImageView View = VK_NULL_HANDLE;
    VkDescriptorSet ImGuiDescriptorSet = VK_NULL_HANDLE;

    unsigned int TextureID = 0;
    std::string FileName;
    TextureType TypeOfTexture;

    int Width;
    int Height;

    Texture();
    Texture(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string TextureLocation, TextureType textureType, VkFormat format, unsigned int textureID);
    Texture(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, TextureType textureType, unsigned int textureID);
    ~Texture();

    virtual void Delete(VkDevice& device);

    VkImageView GetTextureView() { return View; }
    VkSampler GetTextureSampler() { return Sampler; }
};