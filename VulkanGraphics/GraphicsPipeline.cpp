#include "GraphicsPipeline.h"
#include <fstream>

GraphicsPipeline::GraphicsPipeline()
{
    VkSampler NullSampler;
    VkSamplerCreateInfo NullSamplerInfo = {};
    NullSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    NullSamplerInfo.magFilter = VK_FILTER_NEAREST;
    NullSamplerInfo.minFilter = VK_FILTER_NEAREST;
    NullSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    NullSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    NullSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    NullSamplerInfo.anisotropyEnable = VK_TRUE;
    NullSamplerInfo.maxAnisotropy = 16.0f;
    NullSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    NullSamplerInfo.unnormalizedCoordinates = VK_FALSE;
    NullSamplerInfo.compareEnable = VK_FALSE;
    NullSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    NullSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    NullSamplerInfo.minLod = 0;
    NullSamplerInfo.maxLod = 0;
    NullSamplerInfo.mipLodBias = 0;
    if (vkCreateSampler(EnginePtr::GetEnginePtr()->Device, &NullSamplerInfo, nullptr, &NullSampler))
    {
        throw std::runtime_error("Failed to create Sampler.");
    }

    nullBufferInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    nullBufferInfo.imageView = VK_NULL_HANDLE;
    nullBufferInfo.sampler = NullSampler;
}

GraphicsPipeline::~GraphicsPipeline()
{
}

void GraphicsPipeline::UpdateGraphicsPipeLine()
{
    vkDestroyPipeline(VulkanPtr::GetDevice(), ShaderPipeline, nullptr);
    vkDestroyPipelineLayout(VulkanPtr::GetDevice(), ShaderPipelineLayout, nullptr);
    vkDestroyDescriptorPool(VulkanPtr::GetDevice(), DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(VulkanPtr::GetDevice(), DescriptorSetLayout, nullptr);

    ShaderPipeline = VK_NULL_HANDLE;
    ShaderPipelineLayout = VK_NULL_HANDLE;
    DescriptorPool = VK_NULL_HANDLE;
    DescriptorSetLayout = VK_NULL_HANDLE;
}

void GraphicsPipeline::Destroy()
{
    vkDestroyPipeline(VulkanPtr::GetDevice(), ShaderPipeline, nullptr);
    vkDestroyPipelineLayout(VulkanPtr::GetDevice(), ShaderPipelineLayout, nullptr);
    vkDestroyDescriptorPool(VulkanPtr::GetDevice(), DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(VulkanPtr::GetDevice(), DescriptorSetLayout, nullptr);

    ShaderPipeline = VK_NULL_HANDLE;
    ShaderPipelineLayout = VK_NULL_HANDLE;
    DescriptorPool = VK_NULL_HANDLE;
    DescriptorSetLayout = VK_NULL_HANDLE;
}