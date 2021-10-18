#pragma once
#include "Texture.h"

class RenderedCubeMapDepthTexture : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedCubeMapDepthTexture();
    RenderedCubeMapDepthTexture(std::shared_ptr<VulkanEngine> engine, uint32_t mipLevels = 1);
    RenderedCubeMapDepthTexture(glm::ivec2 TextureResolution, uint32_t mipLevels = 1);
    ~RenderedCubeMapDepthTexture();

    void RecreateRendererTexture(glm::ivec2 TextureResolution);
};
