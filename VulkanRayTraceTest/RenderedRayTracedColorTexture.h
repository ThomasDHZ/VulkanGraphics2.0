#pragma once
#include "Texture2D.h"

class RenderedRayTracedColorTexture : public Texture2D
{
private:
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
    void SendTextureToGPU(VulkanEngine& engine);
public:
    RenderedRayTracedColorTexture();
    RenderedRayTracedColorTexture(VulkanEngine& engine);
    ~RenderedRayTracedColorTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};