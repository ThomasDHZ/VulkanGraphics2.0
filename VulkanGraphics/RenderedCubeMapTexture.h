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
    RenderedCubeMapTexture(std::shared_ptr<VulkanEngine> engine);
    RenderedCubeMapTexture(glm::ivec2 TextureResolution);
    ~RenderedCubeMapTexture();

    void RecreateRendererTexture();
};
