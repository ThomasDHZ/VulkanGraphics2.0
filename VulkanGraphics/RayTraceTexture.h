#pragma once
#include "Texture.h"

class RayTraceTexture : public Texture
{
    void CreateTextureImage(VulkanEngine& engine);
    void CreateTextureView(VulkanEngine& engine);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    RayTraceTexture();
    RayTraceTexture(VulkanEngine& engine);
    ~RayTraceTexture();

    void RecreateRendererTexture(VulkanEngine& engine);
};
