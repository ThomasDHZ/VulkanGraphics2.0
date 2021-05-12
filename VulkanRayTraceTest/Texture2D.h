#pragma once
#include "Texture.h"
#include <string>

class Texture2D : public Texture
{
protected:
    void CreateTextureView(VulkanEngine& engine, VkFormat format);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    Texture2D();
    Texture2D(VulkanEngine& engine, TextureType textureType);
    Texture2D(VulkanEngine& engine, const std::string TextureLocation, VkFormat format);
    Texture2D(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format);
    Texture2D(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<glm::vec4>& PixelList, VkFormat format);
    ~Texture2D();
};