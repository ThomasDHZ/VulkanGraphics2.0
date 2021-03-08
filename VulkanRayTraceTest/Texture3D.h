#pragma once
#include "Texture.h"
#include <string>

class Texture3D : public Texture
{
private:
    void CreateTextureView(VulkanEngine& engine, VkFormat format);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    Texture3D();
    Texture3D(VulkanEngine& engine, const std::string TextureLocation, VkFormat format, unsigned int textureID);
    Texture3D(VulkanEngine& engine, int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format, unsigned int textureID);
    ~Texture3D();
};