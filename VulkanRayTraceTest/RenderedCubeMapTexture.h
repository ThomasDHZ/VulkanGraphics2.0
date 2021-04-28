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
    RenderedCubeMapTexture(VulkanEngine& engine, glm::vec2 TextureResolution);
    RenderedCubeMapTexture(VulkanEngine& engine, int width, int Height);
    ~RenderedCubeMapTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};
