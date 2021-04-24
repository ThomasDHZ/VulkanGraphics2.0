#pragma once
#include "Texture.h"
class RenderedCubeMapTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RenderedCubeMapTexture();
    RenderedCubeMapTexture(VulkanEngine& engine);
    ~RenderedCubeMapTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};
