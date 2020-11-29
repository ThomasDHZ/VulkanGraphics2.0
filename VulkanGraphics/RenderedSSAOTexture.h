#pragma once
#include "Texture.h"

class RenderedSSAOTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& renderer);
    void CreateTextureView(VulkanEngine& renderer);
    void CreateTextureSampler(VulkanEngine& renderer);
public:
    RenderedSSAOTexture();
    RenderedSSAOTexture(VulkanEngine& renderer);
    ~RenderedSSAOTexture();

    void RecreateRendererTexture(VulkanEngine& renderer);
};