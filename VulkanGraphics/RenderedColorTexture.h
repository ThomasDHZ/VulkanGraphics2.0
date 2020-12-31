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
    ~RenderedColorTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};