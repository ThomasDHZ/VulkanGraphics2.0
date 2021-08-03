#pragma once
#include "Texture.h"
#include <string>

class Texture3D : public Texture
{
private:
    void CreateTextureView(std::shared_ptr<VulkanEngine> engine, VkFormat format);
    void CreateTextureSampler(std::shared_ptr<VulkanEngine> engine);
public:
    Texture3D();
    Texture3D(std::shared_ptr<VulkanEngine> engine, const std::string TextureLocation, VkFormat format);
    Texture3D(std::shared_ptr<VulkanEngine> engine, int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format);
    ~Texture3D();
};