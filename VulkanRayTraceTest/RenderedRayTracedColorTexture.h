#pragma once
#include "Texture2D.h"

class RenderedRayTracedColorTexture : public Texture2D
{
private:
    void CreateTextureImage(std::shared_ptr<VulkanEngine> engine);
    void CreateTextureView(std::shared_ptr<VulkanEngine> engine);
    void CreateTextureSampler(std::shared_ptr<VulkanEngine> engine);
    void SendTextureToGPU(std::shared_ptr<VulkanEngine> engine);
public:
    RenderedRayTracedColorTexture();
    RenderedRayTracedColorTexture(std::shared_ptr<VulkanEngine> engine);
    ~RenderedRayTracedColorTexture();

    void RecreateRendererTexture(std::shared_ptr<VulkanEngine> engine);
};