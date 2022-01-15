#pragma once
#include "Texture2D.h"

class RenderedDepthTexture : public Texture2D
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedDepthTexture();
    RenderedDepthTexture(glm::ivec2& TextureResolution);
    RenderedDepthTexture(glm::ivec2& TextureResolution, VkSampleCountFlagBits sampleCount);
    ~RenderedDepthTexture();

    void RecreateRendererTexture(glm::ivec2& TextureResolution);
};

