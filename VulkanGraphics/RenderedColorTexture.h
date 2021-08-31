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
    RenderedColorTexture(std::shared_ptr<VulkanEngine> engine, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
    RenderedColorTexture(glm::ivec2 TextureResolution);
    ~RenderedColorTexture();

    void RecreateRendererTexture();
    void RecreateRendererTexture(glm::vec2 TextureResolution);
};