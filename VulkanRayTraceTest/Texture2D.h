#pragma once
#include "Texture.h"
#include <string>

class Texture2D : public Texture
{
protected:
    void CreateTextureView(VkFormat format);
    void CreateTextureSampler();
public:
    Texture2D();
    Texture2D(std::shared_ptr<VulkanEngine> engine, TextureType textureType);
    Texture2D(const std::string TextureLocation, VkFormat format);
    Texture2D(unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format);
    Texture2D(unsigned int width, unsigned int height, std::vector<glm::vec4>& PixelList, VkFormat format);
    ~Texture2D();
};