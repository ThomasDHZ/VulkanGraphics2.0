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
    RenderedCubeMapTexture(glm::vec2 TextureResolution);
    RenderedCubeMapTexture(int width, int Height);
    ~RenderedCubeMapTexture();

    void RecreateRendererTexture();
};
