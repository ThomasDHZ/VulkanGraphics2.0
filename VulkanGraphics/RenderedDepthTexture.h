#pragma once
#include "Texture.h"

class RenderedDepthTexture : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedDepthTexture();
    RenderedDepthTexture(std::shared_ptr<VulkanEngine> engine);
    RenderedDepthTexture(std::shared_ptr<VulkanEngine> engine, VkSampleCountFlagBits sampleCount);
    RenderedDepthTexture(glm::ivec2& TextureResolution);
    RenderedDepthTexture(int width, int Height);
    ~RenderedDepthTexture();

    void RecreateRendererTexture();
};

