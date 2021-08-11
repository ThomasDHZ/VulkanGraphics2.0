#pragma once
#include "Texture2D.h"

class RenderedRayTracedColorTexture : public Texture2D
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
    void SendTextureToGPU();
public:
    RenderedRayTracedColorTexture();
    RenderedRayTracedColorTexture(std::shared_ptr<VulkanEngine> engine);
    ~RenderedRayTracedColorTexture();

    void RecreateRendererTexture();
};