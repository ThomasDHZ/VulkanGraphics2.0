#pragma once
#include "Texture.h"

class RenderedColorTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedColorTexture();
    RenderedColorTexture(VulkanEngine& engine, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedColorTexture(VulkanEngine& engine, glm::vec2& TextureResolution, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedColorTexture(VulkanEngine& engine, int width, int Height, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    ~RenderedColorTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};