#pragma once
#include "Texture.h"

class RenderedDepthTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedDepthTexture();
    RenderedDepthTexture(VulkanEngine& engine, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedDepthTexture(VulkanEngine& engine, glm::vec2& TextureResolution, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedDepthTexture(VulkanEngine& engine, int width, int Height, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    ~RenderedDepthTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};

