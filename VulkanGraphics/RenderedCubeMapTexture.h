#pragma once
#include "CubeMapTexture.h"
class RenderedCubeMapTexture : public CubeMapTexture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    RenderedCubeMapTexture();
    RenderedCubeMapTexture(std::shared_ptr<VulkanEngine> engine, VkSampleCountFlagBits sampleCount, uint32_t mipLevels = 1);
    RenderedCubeMapTexture(glm::ivec2 TextureResolution, VkSampleCountFlagBits sampleCount, uint32_t mipLevels = 1);
    ~RenderedCubeMapTexture();

    void RecreateRendererTexture(glm::ivec2 TextureResolution);
};
