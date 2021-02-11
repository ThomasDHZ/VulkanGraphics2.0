#pragma once
#include "Texture.h"

class RenderedRayTracedColorTexture : public Texture
{
private:
    void CreateTextureImage(VulkanEngine& renderer);
    void CreateTextureView(VulkanEngine& renderer);
    void CreateTextureSampler(VulkanEngine& renderer);
    void SendTextureToGPU(VulkanEngine& renderer);
public:
    RenderedRayTracedColorTexture();
    RenderedRayTracedColorTexture(VulkanEngine& renderer);
    ~RenderedRayTracedColorTexture();

    void RecreateRendererTexture(VulkanEngine& renderer);
};