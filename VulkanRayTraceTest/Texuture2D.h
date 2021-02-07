#pragma once
#include "Texture.h"
#include <string>

class Texture2D : public Texture
{
private:
    void CreateTextureView(VulkanEngine& engine, VkFormat format);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    Texture2D();
    Texture2D(VulkanEngine& engine, const std::string TextureLocation, VkFormat format, unsigned int textureID);
    ~Texture2D();
};