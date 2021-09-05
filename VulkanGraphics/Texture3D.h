#pragma once
#include "Texture.h"
#include <string>

class Texture3D : public Texture
{
private:
    void CreateTextureView(VkFormat format);
    void CreateTextureSampler();
public:
    Texture3D();
    Texture3D(const std::string TextureLocation, VkFormat format);
    Texture3D(glm::ivec3& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format);
    ~Texture3D();
};