#pragma once
#include "Texture.h"
#include <string>

class Texture2DArray : public Texture
{
private:
    void CreateTextureImage();
    void CreateTextureView();
    void CreateTextureSampler();
public:
    Texture2DArray();
    Texture2DArray(glm::ivec2& TextureResolution);
    Texture2DArray(glm::ivec2& TextureResolution, uint32_t layerCount);
    ~Texture2DArray();

    void RecreateRendererTexture(glm::ivec2& TextureResolution, uint32_t layerCount);
};

