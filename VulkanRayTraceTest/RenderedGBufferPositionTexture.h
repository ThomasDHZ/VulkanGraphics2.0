#pragma once
#include "Texture.h"

class RenderedGBufferPositionTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedGBufferPositionTexture();
    RenderedGBufferPositionTexture(VulkanEngine& engine, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGBufferPositionTexture(VulkanEngine& engine, glm::vec2& TextureResolution, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGBufferPositionTexture(VulkanEngine& engine, int width, int Height, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    ~RenderedGBufferPositionTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};