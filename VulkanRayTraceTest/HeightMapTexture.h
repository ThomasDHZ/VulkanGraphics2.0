#pragma once
#include "Texture2D.h"
class HeightMapTexture : public Texture2D
{
private:
    void LoadTexture(VulkanEngine& engine, std::string TextureLocation, VkFormat format) override;

public:
    std::vector<Pixel> HeightMap;

    HeightMapTexture();
    HeightMapTexture(VulkanEngine& engine, const std::string TextureLocation, unsigned int textureID);
    HeightMapTexture(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList,  unsigned int textureID);
    ~HeightMapTexture();

    Pixel GetPixel(uint32_t x, uint32_t y);
};
