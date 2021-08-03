#pragma once
#include "Texture.h"

class RenderedColorTexture : public Texture
{
private:
    void CreateTextureImage(std::shared_ptr<VulkanEngine> engine);
    void CreateTextureView(std::shared_ptr<VulkanEngine> engine);
    void CreateTextureSampler(std::shared_ptr<VulkanEngine> engine);
public:
    RenderedColorTexture();
    RenderedColorTexture(std::shared_ptr<VulkanEngine> engine);
    RenderedColorTexture(std::shared_ptr<VulkanEngine> engine, glm::vec2 TextureResolution);
    RenderedColorTexture(std::shared_ptr<VulkanEngine> engine, int width, int Height);
    ~RenderedColorTexture();

    void RecreateRendererTexture(std::shared_ptr<VulkanEngine> engine);
    void RecreateRendererTexture(std::shared_ptr<VulkanEngine> engine, glm::vec2 TextureResolution);
};