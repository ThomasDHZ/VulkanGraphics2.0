#pragma once
#include "Texture.h"

class Texture2D : public Texture
{
private:
    void CreateTextureView(VulkanEngine& engine, VkFormat format);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    Texture2D();
    Texture2D(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, TextureType textureType, VkFormat format);
    Texture2D(VulkanEngine& engine, VkFormat format, std::string TextureLocation, unsigned int textureID);
    ~Texture2D();
};
