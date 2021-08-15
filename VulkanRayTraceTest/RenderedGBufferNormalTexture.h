#pragma once
#include "Texture.h"

class RenderedGBufferNormalTexture : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedGBufferNormalTexture();
    RenderedGBufferNormalTexture(std::shared_ptr<VulkanEngine> engine);
    RenderedGBufferNormalTexture(glm::ivec2& TextureResolution);
    ~RenderedGBufferNormalTexture();

    void RecreateRendererTexture();
};