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
    RenderedColorTexture(VulkanEngine& engine);
    RenderedColorTexture(VulkanEngine& engine, glm::vec2 TextureResolution);
    RenderedColorTexture(VulkanEngine& engine, int width, int Height);
    ~RenderedColorTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};