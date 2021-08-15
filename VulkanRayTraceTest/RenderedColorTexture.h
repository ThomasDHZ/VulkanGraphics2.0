#pragma once
#include "Texture.h"

class RenderedColorTexture : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedColorTexture();
    RenderedColorTexture(std::shared_ptr<VulkanEngine> engine);
    RenderedColorTexture(glm::ivec2 TextureResolution);
    ~RenderedColorTexture();

    void RecreateRendererTexture();
    void RecreateRendererTexture(glm::vec2 TextureResolution);
};