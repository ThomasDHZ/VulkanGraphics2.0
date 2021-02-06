#pragma once
#include "Texture.h"
#include <string>

class Texture2D : public Texture
{
private:
    void CreateTextureView(VkDevice& device, VkFormat format);
    void CreateTextureSampler(VkDevice& device);
public:
    Texture2D();
    Texture2D(VkDevice& device, VkPhysicalDevice& physcialDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string TextureLocation, VkFormat format, unsigned int textureID);
    ~Texture2D();
};
