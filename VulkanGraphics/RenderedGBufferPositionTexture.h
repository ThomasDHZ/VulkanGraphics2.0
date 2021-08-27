#pragma once
#include "Texture.h"

class RenderedGBufferPositionTexture : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedGBufferPositionTexture();
    RenderedGBufferPositionTexture(std::shared_ptr<VulkanEngine> engine);
    RenderedGBufferPositionTexture(glm::ivec2& TextureResolution);
    ~RenderedGBufferPositionTexture();

    void RecreateRendererTexture();
};