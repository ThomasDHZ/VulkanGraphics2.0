#pragma once
#include "Texture.h"

class RenderedDepthTexture : public Texture
{
private:
    void CreateTextureImage(VkDevice device, VkPhysicalDevice physicalDevice);
    void CreateTextureView(VkDevice device);
    void CreateTextureSampler(VkDevice device);
public:
    RenderedDepthTexture();
    RenderedDepthTexture(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandpool, VkQueue queue);
    ~RenderedDepthTexture();

    void RecreateRendererTexture(VkDevice device, VkPhysicalDevice physicalDevice);
};


