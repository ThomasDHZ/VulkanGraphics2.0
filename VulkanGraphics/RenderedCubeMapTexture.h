#pragma once
#include "Texture.h"
class RenderedCubeMapTexture : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedCubeMapTexture();
    RenderedCubeMapTexture(std::shared_ptr<VulkanEngine> engine, uint32_t mipLevels = 1);
    RenderedCubeMapTexture(glm::ivec2 TextureResolution, uint32_t mipLevels = 1);
    ~RenderedCubeMapTexture();

    void RecreateRendererTexture();
};
