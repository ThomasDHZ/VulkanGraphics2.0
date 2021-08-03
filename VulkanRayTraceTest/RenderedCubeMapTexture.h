#pragma once
#include "Texture.h"
class RenderedCubeMapTexture : public Texture
{
private:
    void CreateTextureImage(std::shared_ptr<VulkanEngine> engine);
    void CreateTextureView(std::shared_ptr<VulkanEngine> engine);
    void CreateTextureSampler(std::shared_ptr<VulkanEngine> engine);
public:
    RenderedCubeMapTexture();
    RenderedCubeMapTexture(std::shared_ptr<VulkanEngine> engine);
    RenderedCubeMapTexture(std::shared_ptr<VulkanEngine> engine, glm::vec2 TextureResolution);
    RenderedCubeMapTexture(std::shared_ptr<VulkanEngine> engine, int width, int Height);
    ~RenderedCubeMapTexture();

    void RecreateRendererTexture(std::shared_ptr<VulkanEngine> engine);
};
