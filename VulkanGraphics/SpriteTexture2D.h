#pragma once
#include "Texture.h"

class SpriteTexture2D : public Texture
{
private:
    void CreateTextureView(VulkanEngine& engine, VkFormat format);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    SpriteTexture2D();
    SpriteTexture2D(VulkanEngine& engine, VkFormat format, std::string TextureLocation, unsigned int textureID);
    ~SpriteTexture2D();
};