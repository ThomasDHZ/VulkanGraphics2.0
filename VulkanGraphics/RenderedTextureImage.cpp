#include "RenderedTextureImage.h"

RenderedTextureImage::RenderedTextureImage()
{
}

RenderedTextureImage::RenderedTextureImage(glm::ivec2 textureResolution, VkFormat format)
{
    TextureResolution = textureResolution;
    Format = format;

    VkImageCreateInfo TextureInfo{};
    TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    TextureInfo.imageType = VK_IMAGE_TYPE_2D;
    TextureInfo.extent.width = TextureResolution.x;
    TextureInfo.extent.height = TextureResolution.y;
    TextureInfo.extent.depth = 1;
    TextureInfo.mipLevels = 1;
    TextureInfo.arrayLayers = 1;
    TextureInfo.format = Format;
    TextureInfo.tiling = VK_IMAGE_TILING_LINEAR;
    TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    vkCreateImage(VulkanPtr::GetDevice(), &TextureInfo, nullptr, &ScreenShotImage);

    vkGetImageMemoryRequirements(VulkanPtr::GetDevice(), ScreenShotImage, &ScreenShotMemoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = ScreenShotMemoryRequirements.size;
    allocInfo.memoryTypeIndex = EnginePtr::GetEnginePtr()->FindMemoryType(ScreenShotMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkAllocateMemory(VulkanPtr::GetDevice(), &allocInfo, nullptr, &ScreenShotImageMemory);
    vkBindImageMemory(VulkanPtr::GetDevice(), ScreenShotImage, ScreenShotImageMemory, 0);
}

RenderedTextureImage::~RenderedTextureImage()
{
}
