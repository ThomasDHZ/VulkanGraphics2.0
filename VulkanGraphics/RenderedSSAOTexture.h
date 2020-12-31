#pragma once
#include "Texture.h"

class RenderedSSAOTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedSSAOTexture();
    RenderedSSAOTexture(VulkanEngine& engine);
    ~RenderedSSAOTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};