#include "GraphicsPipeline.h"
#include <fstream>

GraphicsPipeline::GraphicsPipeline()
{
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

void GraphicsPipeline::SubmitDescriptorSet()
{
    {
        std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
        for (auto& DescriptorBinding : DescriptorBindingList)
        {
            DescriptorPoolList.emplace_back(EnginePtr::GetEnginePtr()->AddDsecriptorPoolBinding(DescriptorBinding.DescriptorType, DescriptorBinding.Count));
        }
        DescriptorPool = EnginePtr::GetEnginePtr()->CreateDescriptorPool(DescriptorPoolList);
    }
    {
        std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
        for (auto& DescriptorBinding : DescriptorBindingList)
        {
            LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ DescriptorBinding.DescriptorSlotNumber, DescriptorBinding.DescriptorType, DescriptorBinding.StageFlags, DescriptorBinding.Count });
        }
        DescriptorSetLayout = EnginePtr::GetEnginePtr()->CreateDescriptorSetLayout(LayoutBindingInfo);
    }
    {
        DescriptorSet = EnginePtr::GetEnginePtr()->CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);
        std::vector<VkWriteDescriptorSet> DescriptorList;
        for (auto& DescriptorBinding : DescriptorBindingList)
        {
         
           
             if (DescriptorBinding.BufferDescriptor.size() > 0)
            {
                DescriptorList.emplace_back(AddBufferDescriptorSet(DescriptorBinding.DescriptorSlotNumber, DescriptorSet, DescriptorBinding.BufferDescriptor, DescriptorBinding.DescriptorType));
            }
            else if (DescriptorBinding.TextureDescriptor.size() > 0)
            {
                DescriptorList.emplace_back(AddTextureDescriptorSet(DescriptorBinding.DescriptorSlotNumber, DescriptorSet, DescriptorBinding.TextureDescriptor, DescriptorBinding.DescriptorType));
            }
            else
             {
                 DescriptorList.emplace_back(AddAccelerationBuffer(DescriptorBinding.DescriptorSlotNumber, DescriptorSet, DescriptorBinding.AccelerationStructureDescriptor));
             }
        }
        vkUpdateDescriptorSets(VulkanPtr::GetDevice(), static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
    }
}

void GraphicsPipeline::UpdateGraphicsPipeLine()
{
    DescriptorPoolList.clear();
    LayoutBindingInfo.clear();
    DescriptorList.clear();
    DescriptorBindingList.clear();

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
    vkDestroySampler(VulkanPtr::GetDevice(), NullSampler, nullptr);
    vkDestroyPipeline(VulkanPtr::GetDevice(), ShaderPipeline, nullptr);
    vkDestroyPipelineLayout(VulkanPtr::GetDevice(), ShaderPipelineLayout, nullptr);
    vkDestroyDescriptorPool(VulkanPtr::GetDevice(), DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(VulkanPtr::GetDevice(), DescriptorSetLayout, nullptr);

    NullSampler = VK_NULL_HANDLE;
    ShaderPipeline = VK_NULL_HANDLE;
    ShaderPipelineLayout = VK_NULL_HANDLE;
    DescriptorPool = VK_NULL_HANDLE;
    DescriptorSetLayout = VK_NULL_HANDLE;
}

VkWriteDescriptorSet GraphicsPipeline::AddAccelerationBuffer(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure)
{
    VkWriteDescriptorSet AccelerationDesciptorSet = {};
    AccelerationDesciptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    AccelerationDesciptorSet.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    AccelerationDesciptorSet.dstSet = DescriptorSet;
    AccelerationDesciptorSet.dstBinding = BindingNumber;
    AccelerationDesciptorSet.descriptorCount = 1;
    AccelerationDesciptorSet.pNext = &accelerationStructure;
    return AccelerationDesciptorSet;
}

VkWriteDescriptorSet GraphicsPipeline::AddTextureDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo, VkDescriptorType descriptorType)
{
    VkWriteDescriptorSet TextureDescriptor = {};
    TextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    TextureDescriptor.dstSet = DescriptorSet;
    TextureDescriptor.dstBinding = BindingNumber;
    TextureDescriptor.dstArrayElement = 0;
    TextureDescriptor.descriptorType = descriptorType;
    TextureDescriptor.descriptorCount = 1;
    TextureDescriptor.pImageInfo = &TextureImageInfo;
    return TextureDescriptor;
}

VkWriteDescriptorSet GraphicsPipeline::AddTextureDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo>& TextureImageInfo, VkDescriptorType descriptorType)
{
    VkWriteDescriptorSet TextureDescriptor = {};
    TextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    TextureDescriptor.dstSet = DescriptorSet;
    TextureDescriptor.dstBinding = BindingNumber;
    TextureDescriptor.dstArrayElement = 0;
    TextureDescriptor.descriptorType = descriptorType;
    TextureDescriptor.descriptorCount = static_cast<uint32_t>(TextureImageInfo.size());
    TextureDescriptor.pImageInfo = TextureImageInfo.data();
    return TextureDescriptor;
}

VkWriteDescriptorSet GraphicsPipeline::AddBufferDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo, VkDescriptorType descriptorType)
{
    VkWriteDescriptorSet BufferDescriptor = {};
    BufferDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    BufferDescriptor.dstSet = DescriptorSet;
    BufferDescriptor.dstBinding = BindingNumber;
    BufferDescriptor.dstArrayElement = 0;
    BufferDescriptor.descriptorType = descriptorType;
    BufferDescriptor.descriptorCount = 1;
    BufferDescriptor.pBufferInfo = &BufferInfo;
    return BufferDescriptor;
}

VkWriteDescriptorSet GraphicsPipeline::AddBufferDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorBufferInfo>& BufferInfoList, VkDescriptorType descriptorType)
{
    VkWriteDescriptorSet BufferDescriptor = {};
    BufferDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    BufferDescriptor.dstSet = DescriptorSet;
    BufferDescriptor.dstBinding = BindingNumber;
    BufferDescriptor.dstArrayElement = 0;
    BufferDescriptor.descriptorType = descriptorType;
    BufferDescriptor.descriptorCount = static_cast<uint32_t>(BufferInfoList.size());
    BufferDescriptor.pBufferInfo = BufferInfoList.data();
    return BufferDescriptor;
}

void GraphicsPipeline::AddDescriptorSetBinding(unsigned int BindingNumber, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    DescriptorSetBinding.AccelerationStructureDescriptor = accelerationStructure;
    DescriptorSetBinding.Count = 1;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddDescriptorSetBinding(unsigned int BindingNumber, std::vector<VkDescriptorImageInfo>& TextureImageInfo, VkShaderStageFlags StageFlags, VkDescriptorType BufferType, uint32_t DescriptorCount)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = BufferType;
    DescriptorSetBinding.TextureDescriptor = TextureImageInfo;
    DescriptorSetBinding.Count = DescriptorCount;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddDescriptorSetBinding(unsigned int BindingNumber, VkDescriptorImageInfo& TextureImageInfo, VkShaderStageFlags StageFlags, VkDescriptorType BufferType)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = BufferType;
    DescriptorSetBinding.TextureDescriptor = std::vector<VkDescriptorImageInfo>{ TextureImageInfo };
    DescriptorSetBinding.Count = 1;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddDescriptorSetBinding(unsigned int BindingNumber, VkDescriptorBufferInfo& BufferInfo, VkDescriptorType BufferType, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = BufferType;
    DescriptorSetBinding.BufferDescriptor = std::vector<VkDescriptorBufferInfo>{ BufferInfo };
    DescriptorSetBinding.Count = 1;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddDescriptorSetBinding(unsigned int BindingNumber, std::vector<VkDescriptorBufferInfo>& BufferInfo, VkDescriptorType BufferType, VkShaderStageFlags StageFlags, uint32_t DescriptorCount)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = BufferType;
    DescriptorSetBinding.BufferDescriptor = BufferInfo;
    DescriptorSetBinding.Count = DescriptorCount;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}