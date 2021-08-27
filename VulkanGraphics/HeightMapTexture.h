#pragma once
#include "Texture2D.h"
class HeightMapTexture : public Texture
{
private:
    void LoadTexture(std::string TextureLocation, VkFormat format) override;
    void CreateTextureView(VkFormat format);
    void CreateTextureSampler();
public:
    std::vector<Pixel> HeightMap;

    HeightMapTexture();
    HeightMapTexture(const std::string TextureLocation);
    HeightMapTexture(unsigned int width, unsigned int height, std::vector<Pixel>& PixelList);
    ~HeightMapTexture();

    Pixel GetPixel(uint32_t x, uint32_t y);
};

