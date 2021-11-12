#pragma once
#include "VulkanEngine.h"
#include "Texture2D.h"
#include "RenderedTextureImage.h"

static class ImageProccessor
{
public:
    static void CopyTexture(std::shared_ptr<Texture> SrcTexture, std::shared_ptr<RenderedTextureImage> DstTexture)
    {
        VkCommandBuffer commandBuffer = EnginePtr::GetEnginePtr()->beginSingleTimeCommands();

        VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        VkImageMemoryBarrier dstImagBarrier = {};
        dstImagBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        dstImagBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        dstImagBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstImagBarrier.image = DstTexture->ScreenShotImage;
        dstImagBarrier.subresourceRange = subresourceRange;
        dstImagBarrier.srcAccessMask = 0;
        dstImagBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstImagBarrier);

        VkImageMemoryBarrier srcImagBarrier = {};
        srcImagBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        srcImagBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        srcImagBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        srcImagBarrier.image = DstTexture->ScreenShotImage;
        srcImagBarrier.subresourceRange = subresourceRange;
        srcImagBarrier.srcAccessMask = 0;
        srcImagBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcImagBarrier);

        VkImageCopy copyImage{};
        copyImage.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyImage.srcSubresource.layerCount = 1;
        copyImage.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyImage.dstSubresource.layerCount = 1;
        copyImage.extent.width = SrcTexture->Width;
        copyImage.extent.height = SrcTexture->Height;
        copyImage.extent.depth = 1;
        vkCmdCopyImage(commandBuffer, SrcTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, DstTexture->ScreenShotImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyImage);

        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.srcAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        imageMemoryBarrier.dstAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.image = DstTexture->ScreenShotImage;
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
        imageMemoryBarrier2.image = SrcTexture->Image;
        imageMemoryBarrier2.subresourceRange = subresourceRange;
        vkCmdPipelineBarrier(commandBuffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier2);

        EnginePtr::GetEnginePtr()->endSingleTimeCommands(commandBuffer);
    }

    static void CopyCubeMapToTexture(std::shared_ptr<Texture> SrcTexture, std::shared_ptr<RenderedTextureImage> DstTexture, uint32_t face)
    {
        VkCommandBuffer commandBuffer = EnginePtr::GetEnginePtr()->beginSingleTimeCommands();

        VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        VkImageMemoryBarrier dstImagBarrier = {};
        dstImagBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        dstImagBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        dstImagBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        dstImagBarrier.image = DstTexture->ScreenShotImage;
        dstImagBarrier.subresourceRange = subresourceRange;
        dstImagBarrier.srcAccessMask = 0;
        dstImagBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &dstImagBarrier);

        VkImageMemoryBarrier srcImagBarrier = {};
        srcImagBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        srcImagBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        srcImagBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        srcImagBarrier.image = DstTexture->ScreenShotImage;
        srcImagBarrier.subresourceRange = subresourceRange;
        srcImagBarrier.srcAccessMask = 0;
        srcImagBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &srcImagBarrier);

        VkImageCopy copyImage{};
        copyImage.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyImage.srcSubresource.layerCount = 1;
        copyImage.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyImage.dstSubresource.layerCount = 1;
        copyImage.srcSubresource.baseArrayLayer = face;
        copyImage.extent.width = SrcTexture->Width;
        copyImage.extent.height = SrcTexture->Height;
        copyImage.extent.depth = 1;
        vkCmdCopyImage(commandBuffer, SrcTexture->Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, DstTexture->ScreenShotImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyImage);

        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.srcAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        imageMemoryBarrier.dstAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageMemoryBarrier.image = DstTexture->ScreenShotImage;
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
        imageMemoryBarrier2.image = SrcTexture->Image;
        imageMemoryBarrier2.subresourceRange = subresourceRange;
        vkCmdPipelineBarrier(commandBuffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier2);

        EnginePtr::GetEnginePtr()->endSingleTimeCommands(commandBuffer);
    }

	static void RenderTexture(std::shared_ptr<Texture> texture)
	{
        std::shared_ptr<RenderedTextureImage> ScreenShotTexture = std::make_shared<RenderedTextureImage>(RenderedTextureImage(glm::ivec2(texture->Width, texture->Height), VK_FORMAT_R8G8B8A8_UNORM));
        CopyTexture(texture, ScreenShotTexture);

        VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
        VkSubresourceLayout subResourceLayout;
        vkGetImageSubresourceLayout(VulkanPtr::GetDevice(), ScreenShotTexture->ScreenShotImage, &subResource, &subResourceLayout);

        // Map image memory so we can start copying from it
        const char* data;
        vkMapMemory(VulkanPtr::GetDevice(), ScreenShotTexture->ScreenShotImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
        data += subResourceLayout.offset;

        std::ofstream file("ScreenShot.ppm", std::ios::out | std::ios::binary);

        // ppm header
        file << "P6\n" << texture->Width << "\n" << texture->Height << "\n" << 255 << "\n";

        for (uint32_t y = 0; y < texture->Height; y++)
        {
            unsigned int* row = (unsigned int*)data;
            for (uint32_t x = 0; x < texture->Width; x++)
            {
                file.write((char*)row, 3);
                row++;
            }
            data += subResourceLayout.rowPitch;
        }
        file.close();
	}

    static void RenderCubeMapTexture(std::shared_ptr<Texture> texture)
    {
        for (int x = 0; x < 6; x++)
        {
            std::shared_ptr<RenderedTextureImage> ScreenShotTexture = std::make_shared<RenderedTextureImage>(RenderedTextureImage(glm::ivec2(texture->Width, texture->Height), VK_FORMAT_R8G8B8A8_UNORM));
            CopyCubeMapToTexture(texture, ScreenShotTexture, x);

            VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
            VkSubresourceLayout subResourceLayout;
            vkGetImageSubresourceLayout(VulkanPtr::GetDevice(), ScreenShotTexture->ScreenShotImage, &subResource, &subResourceLayout);

            // Map image memory so we can start copying from it
            const char* data;
            vkMapMemory(VulkanPtr::GetDevice(), ScreenShotTexture->ScreenShotImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
            data += subResourceLayout.offset;

            std::ofstream file("ScreenShotFace" + std::to_string(x) + ".ppm", std::ios::out | std::ios::binary);

            // ppm header
            file << "P6\n" << texture->Width << "\n" << texture->Height << "\n" << 255 << "\n";

            for (uint32_t y = 0; y < texture->Height; y++)
            {
                unsigned int* row = (unsigned int*)data;
                for (uint32_t x = 0; x < texture->Width; x++)
                {
                    file.write((char*)row, 3);
                    row++;
                }
                data += subResourceLayout.rowPitch;
            }
            file.close();
        }
    }
};