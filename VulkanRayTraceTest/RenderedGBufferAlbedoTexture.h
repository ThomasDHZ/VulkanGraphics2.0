#pragma once
#include "Texture.h"

class RenderedGBufferAlbedoTexture : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedGBufferAlbedoTexture();
    RenderedGBufferAlbedoTexture(std::shared_ptr<VulkanEngine> engine);
    RenderedGBufferAlbedoTexture(glm::ivec2& TextureResolution);
    RenderedGBufferAlbedoTexture(int width, int Height);
    ~RenderedGBufferAlbedoTexture();

    void RecreateRendererTexture();
};