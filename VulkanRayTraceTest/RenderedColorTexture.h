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
    RenderedColorTexture(glm::vec2 TextureResolution);
    RenderedColorTexture(int width, int Height);
    ~RenderedColorTexture();

    void RecreateRendererTexture();
    void RecreateRendererTexture(glm::vec2 TextureResolution);
};