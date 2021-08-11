#pragma once
#include "Texture.h"

class RenderedGTBNTexture : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedGTBNTexture();
    RenderedGTBNTexture(std::shared_ptr<VulkanEngine> engine, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGTBNTexture(glm::vec2& TextureResolution, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    RenderedGTBNTexture(int width, int Height, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED);
    ~RenderedGTBNTexture();

    void RecreateRendererTexture();
};