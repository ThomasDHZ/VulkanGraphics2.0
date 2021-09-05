#pragma once
#include <vulkan/vulkan.h>
#include <string>

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
    VkImage Image = VK_NULL_HANDLE;
    VkSampler Sampler = VK_NULL_HANDLE;

    void TransitionImageLayout(VkDevice device, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool commandPool, VkQueue queue);
    void CopyBufferToImage(VkDevice device, VkBuffer buffer, VkCommandPool commandPool, VkQueue queue);


    virtual void LoadTexture(VkDevice device, std::string TextureLocation, VkFormat format, VkPhysicalDevice physicalDevice, VkCommandPool commandpool, VkQueue queue);
    virtual void CreateTextureImage(VkDevice device, VkImageCreateInfo TextureInfo, VkPhysicalDevice PhysicalDevice);
    virtual void CreateTextureView(VkDevice device, VkImageViewCreateInfo TextureImageViewInfo);
    virtual void CreateTextureSampler(VkDevice device, VkSamplerCreateInfo TextureImageSamplerInfo);

public:
    VkImageView View = VK_NULL_HANDLE;
    VkDescriptorSet ImGuiDescriptorSet = VK_NULL_HANDLE;

    unsigned int TextureID = 0;
    std::string FileName;
    TextureType TypeOfTexture;

    int Width;
    int Height;

    Texture();
    Texture(VkDevice device, std::string TextureLocation, VkFormat format, VkPhysicalDevice physicalDevice, VkCommandPool commandpool, VkQueue queue);
    Texture(VkDevice device, TextureType textureType);
    ~Texture();

    virtual void Delete(VkDevice device);

    VkImageView GetTextureView() { return View; }
    VkSampler GetTextureSampler() { return Sampler; }
};