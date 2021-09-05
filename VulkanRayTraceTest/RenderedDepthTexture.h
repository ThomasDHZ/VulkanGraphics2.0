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
    RenderedDepthTexture(VulkanEngine& engine);
    ~RenderedDepthTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};

