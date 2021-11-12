#include "BaseRenderPass.h"

BaseRenderPass::BaseRenderPass()
{

}

BaseRenderPass::~BaseRenderPass()
{
}

void BaseRenderPass::Destroy()
{
    vkDestroyRenderPass(EnginePtr::GetEnginePtr()->Device, RenderPass, nullptr);
    RenderPass = VK_NULL_HANDLE;

    for (auto& framebuffer : SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(EnginePtr::GetEnginePtr()->Device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
}

void BaseRenderPass::SaveScreenShot(VkImage FrameBufferImage)
{
    VkImage ScreenShotImage = VK_NULL_HANDLE;
    VkMemoryRequirements ScreenShotMemoryRequirements;
    VkDeviceMemory ScreenShotImageMemory = VK_NULL_HANDLE;

    VkImageCreateInfo TextureInfo{};
    TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    TextureInfo.imageType = VK_IMAGE_TYPE_2D;
    TextureInfo.extent.width = RenderPassResolution.x;
    TextureInfo.extent.height = RenderPassResolution.y;
    TextureInfo.extent.depth = 1;
    TextureInfo.mipLevels = 1;
    TextureInfo.arrayLayers = 1;
    TextureInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    TextureInfo.tiling = VK_IMAGE_TILING_LINEAR;
    TextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    TextureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    TextureInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    vkCreateImage(VulkanPtr::GetDevice(), &TextureInfo, nullptr, &ScreenShotImage);

    vkGetImageMemoryRequirements(VulkanPtr::GetDevice(), ScreenShotImage, &ScreenShotMemoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = ScreenShotMemoryRequirements.size;
    allocInfo.memoryTypeIndex = 1;
    vkAllocateMemory(VulkanPtr::GetDevice(), &allocInfo, nullptr, &ScreenShotImageMemory);
    vkBindImageMemory(VulkanPtr::GetDevice(), ScreenShotImage, ScreenShotImageMemory, 0);

    VkCommandBuffer commandBuffer = EnginePtr::GetEnginePtr()->beginSingleTimeCommands();

    VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    VkImageMemoryBarrier dstImagBarrier = {};
    dstImagBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    dstImagBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    dstImagBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    dstImagBarrier.image = ScreenShotImage;
    dstImagBarrier.subresourceRange = subresourceRange;
    dstImagBarrier.srcAccessMask = 0;
    dstImagBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstImagBarrier);

    VkImageMemoryBarrier srcImagBarrier = {};
    srcImagBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    srcImagBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    srcImagBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    srcImagBarrier.image = ScreenShotImage;
    srcImagBarrier.subresourceRange = subresourceRange;
    srcImagBarrier.srcAccessMask = 0;
    srcImagBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcImagBarrier);

    VkImageCopy copyImage{};
    copyImage.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyImage.srcSubresource.layerCount = 1;
    copyImage.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyImage.dstSubresource.layerCount = 1;
    copyImage.extent.width = RenderPassResolution.x;
    copyImage.extent.height = RenderPassResolution.y;
    copyImage.extent.depth = 1;
    vkCmdCopyImage(commandBuffer, FrameBufferImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, ScreenShotImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyImage);
  
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.srcAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    imageMemoryBarrier.dstAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageMemoryBarrier.image = ScreenShotImage;
    imageMemoryBarrier.subresourceRange = subresourceRange;
    vkCmdPipelineBarrier(commandBuffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    VkImageMemoryBarrier imageMemoryBarrier2{};
    imageMemoryBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier2.srcAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    imageMemoryBarrier2.dstAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    imageMemoryBarrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    imageMemoryBarrier2.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageMemoryBarrier2.image = FrameBufferImage;
    imageMemoryBarrier2.subresourceRange = subresourceRange;
    vkCmdPipelineBarrier(commandBuffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier2);

    EnginePtr::GetEnginePtr()->endSingleTimeCommands(commandBuffer);

    VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
    VkSubresourceLayout subResourceLayout;
    vkGetImageSubresourceLayout(VulkanPtr::GetDevice(), ScreenShotImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    const char* data;
    vkMapMemory(VulkanPtr::GetDevice(), ScreenShotImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
    data += subResourceLayout.offset;

    std::ofstream file("ScreenShot.ppm", std::ios::out | std::ios::binary);

    // ppm header
    file << "P6\n" << RenderPassResolution.x << "\n" << RenderPassResolution.y << "\n" << 255 << "\n";

    for (uint32_t y = 0; y < RenderPassResolution.y; y++)
    {
        unsigned int* row = (unsigned int*)data;
        for (uint32_t x = 0; x < RenderPassResolution.x; x++)
        {
            file.write((char*)row, 3);
            row++;
        }
        data += subResourceLayout.rowPitch;
    }
    file.close();
    auto a = 34;
}

void BaseRenderPass::SaveScreenShot(std::shared_ptr<Texture> texture)
{
    VkImage ScreenShotImage = VK_NULL_HANDLE;
    VkMemoryRequirements ScreenShotMemoryRequirements;
    VkDeviceMemory ScreenShotImageMemory = VK_NULL_HANDLE;

    VkImageCreateInfo TextureInfo{};
    TextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    TextureInfo.imageType = VK_IMAGE_TYPE_2D;
    TextureInfo.extent.width = RenderPassResolution.x;
    TextureInfo.extent.height = RenderPassResolution.y;
    TextureInfo.extent.depth = 1;
    TextureInfo.mipLevels = 1;
    TextureInfo.arrayLayers = 1;
    TextureInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
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

    VkCommandBuffer commandBuffer = EnginePtr::GetEnginePtr()->beginSingleTimeCommands();

    VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    VkImageMemoryBarrier dstImagBarrier = {};
    dstImagBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    dstImagBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    dstImagBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    dstImagBarrier.image = ScreenShotImage;
    dstImagBarrier.subresourceRange = subresourceRange;
    dstImagBarrier.srcAccessMask = 0;
    dstImagBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstImagBarrier);

    VkImageMemoryBarrier srcImagBarrier = {};
    srcImagBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    srcImagBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    srcImagBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    srcImagBarrier.image = ScreenShotImage;
    srcImagBarrier.subresourceRange = subresourceRange;
    srcImagBarrier.srcAccessMask = 0;
    srcImagBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcImagBarrier);

    VkImageCopy copyImage{};
    copyImage.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyImage.srcSubresource.layerCount = 1;
    copyImage.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyImage.dstSubresource.layerCount = 1;
    copyImage.extent.width = RenderPassResolution.x;
    copyImage.extent.height = RenderPassResolution.y;
    copyImage.extent.depth = 1;
    vkCmdCopyImage(commandBuffer, texture->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, ScreenShotImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyImage);

    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.srcAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    imageMemoryBarrier.dstAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageMemoryBarrier.image = ScreenShotImage;
    imageMemoryBarrier.subresourceRange = subresourceRange;
    vkCmdPipelineBarrier(commandBuffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    VkImageMemoryBarrier imageMemoryBarrier2{};
    imageMemoryBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier2.srcAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    imageMemoryBarrier2.dstAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    imageMemoryBarrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    imageMemoryBarrier2.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageMemoryBarrier2.image = texture->Image;
    imageMemoryBarrier2.subresourceRange = subresourceRange;
    vkCmdPipelineBarrier(commandBuffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier2);

    EnginePtr::GetEnginePtr()->endSingleTimeCommands(commandBuffer);

    VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
    VkSubresourceLayout subResourceLayout;
    vkGetImageSubresourceLayout(VulkanPtr::GetDevice(), ScreenShotImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    const char* data;
    vkMapMemory(VulkanPtr::GetDevice(), ScreenShotImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
    data += subResourceLayout.offset;

    std::ofstream file("ScreenShot.ppm", std::ios::out | std::ios::binary);

    // ppm header
    file << "P6\n" << RenderPassResolution.x << "\n" << RenderPassResolution.y << "\n" << 255 << "\n";

    for (uint32_t y = 0; y < RenderPassResolution.y; y++)
    {
        unsigned int* row = (unsigned int*)data;
        for (uint32_t x = 0; x < RenderPassResolution.x; x++)
        {
            file.write((char*)row, 3);
            row++;
        }
        data += subResourceLayout.rowPitch;
    }
    file.close();
    auto a = 34;
}
