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
    RenderedColorTexture(glm::ivec2 TextureResolution);
    RenderedColorTexture(glm::ivec2 TextureResolution, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
    ~RenderedColorTexture();

    void RecreateRendererTexture(glm::vec2 TextureResolution);
};