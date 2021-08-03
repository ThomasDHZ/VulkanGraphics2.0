#pragma once
#include "Texture.h"

class RenderedGBufferAlbedoTexture : public Texture
{
private:
    void CreateTextureImage(std::shared_ptr<VulkanEngine> engine);
    void CreateTextureView(std::shared_ptr<VulkanEngine> engine);
    void CreateTextureSampler(std::shared_ptr<VulkanEngine> engine);
public:
    RenderedGBufferAlbedoTexture();
    RenderedGBufferAlbedoTexture(std::shared_ptr<VulkanEngine> engine, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGBufferAlbedoTexture(std::shared_ptr<VulkanEngine> engine, glm::vec2& TextureResolution, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGBufferAlbedoTexture(std::shared_ptr<VulkanEngine> engine, int width, int Height, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    ~RenderedGBufferAlbedoTexture();

    void RecreateRendererTexture(std::shared_ptr<VulkanEngine> engine);
};