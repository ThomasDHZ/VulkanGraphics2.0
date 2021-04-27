#pragma once
#include "Texture.h"

class RenderedGBufferAlbedoTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedGBufferAlbedoTexture();
    RenderedGBufferAlbedoTexture(VulkanEngine& engine, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGBufferAlbedoTexture(VulkanEngine& engine, glm::vec2& TextureResolution, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGBufferAlbedoTexture(VulkanEngine& engine, int width, int Height, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    ~RenderedGBufferAlbedoTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};