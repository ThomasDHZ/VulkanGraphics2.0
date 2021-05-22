#pragma once
#include "Texture2D.h"
class HeightMapTexture : public Texture
{
private:
    void LoadTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format) override;
    void CreateTextureView(VulkanEngine& engine, VkFormat format);
    void CreateTextureSampler(VulkanEngine& engine);
public:
    std::vector<Pixel> HeightMap;

    HeightMapTexture();
    HeightMapTexture(VulkanEngine& engine, const std::string TextureLocation);
    HeightMapTexture(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList);
    ~HeightMapTexture();

    Pixel GetPixel(uint32_t x, uint32_t y);
};

