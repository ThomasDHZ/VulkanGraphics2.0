#pragma once
#include "Texture2D.h"
class HeightMapTexture : public Texture
{
private:
    void LoadTexture(std::shared_ptr<VulkanEngine> engine, std::string TextureLocation, VkFormat format) override;
    void CreateTextureView(std::shared_ptr<VulkanEngine> engine, VkFormat format);
    void CreateTextureSampler(std::shared_ptr<VulkanEngine> engine);
public:
    std::vector<Pixel> HeightMap;

    HeightMapTexture();
    HeightMapTexture(std::shared_ptr<VulkanEngine> engine, const std::string TextureLocation);
    HeightMapTexture(std::shared_ptr<VulkanEngine> engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList);
    ~HeightMapTexture();

    Pixel GetPixel(uint32_t x, uint32_t y);
};

