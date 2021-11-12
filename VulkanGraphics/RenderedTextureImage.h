#pragma once
#include "Texture.h"

class RenderedTextureImage
{
private:
public:
    glm::ivec2 TextureResolution;
    VkFormat Format;
    VkImage ScreenShotImage = VK_NULL_HANDLE;
    VkMemoryRequirements ScreenShotMemoryRequirements;
    VkDeviceMemory ScreenShotImageMemory = VK_NULL_HANDLE;

    RenderedTextureImage();
    RenderedTextureImage(glm::ivec2 textureResolution, VkFormat format);
    ~RenderedTextureImage();
};