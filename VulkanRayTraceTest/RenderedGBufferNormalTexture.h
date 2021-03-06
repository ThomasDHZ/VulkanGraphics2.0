#pragma once
#include "Texture.h"

class RenderedGBufferNormalTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedGBufferNormalTexture();
    RenderedGBufferNormalTexture(VulkanEngine& engine, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGBufferNormalTexture(VulkanEngine& engine, glm::vec2& TextureResolution, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGBufferNormalTexture(VulkanEngine& engine, int width, int Height, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    ~RenderedGBufferNormalTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};