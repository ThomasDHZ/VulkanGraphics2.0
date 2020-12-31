#pragma once
#include "Texture.h"

class RenderedHDRColorTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedHDRColorTexture();
    RenderedHDRColorTexture(VulkanEngine& engine);
    ~RenderedHDRColorTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};