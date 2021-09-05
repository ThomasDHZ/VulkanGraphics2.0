#pragma once
#include "Texture.h"

class RenderedGTBNTexture : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedGTBNTexture();
    RenderedGTBNTexture(std::shared_ptr<VulkanEngine> engine);
    RenderedGTBNTexture(glm::ivec2& TextureResolution);
    ~RenderedGTBNTexture();

    void RecreateRendererTexture();
};