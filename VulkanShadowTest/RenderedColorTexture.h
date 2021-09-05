#pragma once
#include "Texture.h"

class RenderedColorTexture : public Texture
{
private:
    void CreateTextureImage(VkDevice device, VkPhysicalDevice physicalDevice);
    void CreateTextureView(VkDevice device);
    void CreateTextureSampler(VkDevice device);
public:
    RenderedColorTexture();
    RenderedColorTexture(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandpool, VkQueue queue);
    ~RenderedColorTexture();

    void RecreateRendererTexture(VkDevice device, VkPhysicalDevice physicalDevice);
};