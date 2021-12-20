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

    DescriptorPoolList.clear();
    LayoutBindingInfo.clear();
    DescriptorList.clear();
    DescriptorBindingList.clear();
}

GraphicsPipeline::~GraphicsPipeline()
{
}

void GraphicsPipeline::SubmitDescriptorSet()
{
    if (DescriptorBindingList.size() > 0)
    {
        {
            std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
            for (auto& DescriptorBinding : DescriptorBindingList)
            {
                DescriptorPoolList.emplace_back(AddDsecriptorPoolBinding(DescriptorBinding.DescriptorType, DescriptorBinding.Count));
            }
            DescriptorPool = CreateDescriptorPool(DescriptorPoolList);
        }
        {
            std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
            for (auto& DescriptorBinding : DescriptorBindingList)
            {
                LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ DescriptorBinding.DescriptorSlotNumber, DescriptorBinding.DescriptorType, DescriptorBinding.StageFlags, DescriptorBinding.Count });
            }
            DescriptorSetLayout = CreateDescriptorSetLayout(LayoutBindingInfo);
        }
        {
            DescriptorSet = CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);

            std::vector<VkWriteDescriptorSet> DescriptorList;

            for (auto& DescriptorBinding : DescriptorBindingList)
            {


                if (DescriptorBinding.BufferDescriptor.size() > 0)
                {
                    DescriptorList.emplace_back(AddBufferDescriptorSet(DescriptorBinding.DescriptorSlotNumber, DescriptorBinding.BufferDescriptor, DescriptorBinding.DescriptorType));
                }
                else if (DescriptorBinding.TextureDescriptor.size() > 0)
                {
                    DescriptorList.emplace_back(AddTextureDescriptorSet(DescriptorBinding.DescriptorSlotNumber, DescriptorBinding.TextureDescriptor, DescriptorBinding.DescriptorType));
                }
                else
                {
                    DescriptorList.emplace_back(AddAccelerationBuffer(DescriptorBinding.DescriptorSlotNumber, DescriptorBinding.AccelerationStructureDescriptor));
                }
            }
            vkUpdateDescriptorSets(VulkanPtr::GetDevice(), static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
        }
    }
    else
    {
        DescriptorPoolList.emplace_back(AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));
        DescriptorPool = CreateDescriptorPool(DescriptorPoolList);

        LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 1 });
        DescriptorSetLayout = CreateDescriptorSetLayout(LayoutBindingInfo);
   
        DescriptorSet = CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);
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
    vkDestroyPipelineCache(VulkanPtr::GetDevice(), PipelineCache, nullptr);

    ShaderPipeline = VK_NULL_HANDLE;
    ShaderPipelineLayout = VK_NULL_HANDLE;
    DescriptorPool = VK_NULL_HANDLE;
    DescriptorSetLayout = VK_NULL_HANDLE;
    PipelineCache = VK_NULL_HANDLE;
}

void GraphicsPipeline::Destroy()
{
    vkDestroySampler(VulkanPtr::GetDevice(), NullSampler, nullptr);
    vkDestroyPipeline(VulkanPtr::GetDevice(), ShaderPipeline, nullptr);
    vkDestroyPipelineLayout(VulkanPtr::GetDevice(), ShaderPipelineLayout, nullptr);
    vkDestroyDescriptorPool(VulkanPtr::GetDevice(), DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(VulkanPtr::GetDevice(), DescriptorSetLayout, nullptr);
    vkDestroyPipelineCache(VulkanPtr::GetDevice(), PipelineCache, nullptr);

    NullSampler = VK_NULL_HANDLE;
    ShaderPipeline = VK_NULL_HANDLE;
    ShaderPipelineLayout = VK_NULL_HANDLE;
    DescriptorPool = VK_NULL_HANDLE;
    DescriptorSetLayout = VK_NULL_HANDLE;
    PipelineCache = VK_NULL_HANDLE;
}

VkWriteDescriptorSet GraphicsPipeline::AddAccelerationBuffer(uint32_t BindingNumber, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure)
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

VkWriteDescriptorSet GraphicsPipeline::AddTextureDescriptorSet(uint32_t BindingNumber, VkDescriptorImageInfo& TextureImageInfo, VkDescriptorType descriptorType)
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

VkWriteDescriptorSet GraphicsPipeline::AddTextureDescriptorSet(uint32_t BindingNumber,std::vector<VkDescriptorImageInfo>& TextureImageInfo, VkDescriptorType descriptorType)
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

VkWriteDescriptorSet GraphicsPipeline::AddBufferDescriptorSet(uint32_t BindingNumber, VkDescriptorBufferInfo& BufferInfo, VkDescriptorType descriptorType)
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

VkWriteDescriptorSet GraphicsPipeline::AddBufferDescriptorSet(uint32_t BindingNumber, std::vector<VkDescriptorBufferInfo>& BufferInfoList, VkDescriptorType descriptorType)
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

void GraphicsPipeline::AddAccelerationDescriptorSetBinding(uint32_t BindingNumber, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    DescriptorSetBinding.AccelerationStructureDescriptor = accelerationStructure;
    DescriptorSetBinding.Count = 1;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddStorageTextureSetBinding(uint32_t BindingNumber, VkDescriptorImageInfo& TextureImageInfo, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    DescriptorSetBinding.TextureDescriptor = std::vector<VkDescriptorImageInfo>{ TextureImageInfo };
    DescriptorSetBinding.Count = 1;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddTextureDescriptorSetBinding(uint32_t BindingNumber, std::vector<VkDescriptorImageInfo>& TextureImageInfo, uint32_t DescriptorCount, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    DescriptorSetBinding.TextureDescriptor = TextureImageInfo;
    DescriptorSetBinding.Count = DescriptorCount;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddTextureDescriptorSetBinding(uint32_t BindingNumber, VkDescriptorImageInfo& TextureImageInfo, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    DescriptorSetBinding.TextureDescriptor = std::vector<VkDescriptorImageInfo>{ TextureImageInfo };
    DescriptorSetBinding.Count = 1;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddUniformBufferDescriptorSetBinding(uint32_t BindingNumber, VkDescriptorBufferInfo& BufferInfo, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    DescriptorSetBinding.BufferDescriptor = std::vector<VkDescriptorBufferInfo>{ BufferInfo };
    DescriptorSetBinding.Count = 1;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddUniformBufferDescriptorSetBinding(uint32_t BindingNumber, std::vector<VkDescriptorBufferInfo>& BufferInfo, uint32_t DescriptorCount, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    DescriptorSetBinding.BufferDescriptor = BufferInfo;
    DescriptorSetBinding.Count = DescriptorCount;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddStorageBufferDescriptorSetBinding(uint32_t BindingNumber, VkDescriptorBufferInfo& BufferInfo, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    DescriptorSetBinding.BufferDescriptor = std::vector<VkDescriptorBufferInfo>{ BufferInfo };
    DescriptorSetBinding.Count = 1;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddStorageBufferDescriptorSetBinding(uint32_t BindingNumber, std::vector<VkDescriptorBufferInfo>& BufferInfo, uint32_t DescriptorCount, VkShaderStageFlags StageFlags)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = StageFlags;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    DescriptorSetBinding.BufferDescriptor = BufferInfo;
    DescriptorSetBinding.Count = DescriptorCount;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

void GraphicsPipeline::AddNullDescriptorSetBinding(uint32_t BindingNumber)
{
    DescriptorSetBindingStruct DescriptorSetBinding{};
    DescriptorSetBinding.DescriptorSlotNumber = BindingNumber;
    DescriptorSetBinding.StageFlags = VK_SHADER_STAGE_ALL;
    DescriptorSetBinding.DescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    DescriptorSetBinding.Count = 0;
    DescriptorBindingList.emplace_back(DescriptorSetBinding);
}

VkDescriptorPoolSize GraphicsPipeline::AddDsecriptorPoolBinding(VkDescriptorType descriptorType, uint32_t descriptorCount)
{
    VkDescriptorPoolSize DescriptorPoolBinding = {};
    DescriptorPoolBinding.type = descriptorType;
    DescriptorPoolBinding.descriptorCount = descriptorCount;

    return DescriptorPoolBinding;
}

VkDescriptorPool GraphicsPipeline::CreateDescriptorPool(std::vector<VkDescriptorPoolSize> DescriptorPoolInfo)
{
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(DescriptorPoolInfo.size());
    poolInfo.pPoolSizes = DescriptorPoolInfo.data();
    poolInfo.maxSets = static_cast<uint32_t>(EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainImageCount());

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(EnginePtr::GetEnginePtr()->Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    return descriptorPool;
}

VkDescriptorSetLayout GraphicsPipeline::CreateDescriptorSetLayout(std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo)
{
    std::vector<VkDescriptorSetLayoutBinding> LayoutBindingList = {};

    for (auto Binding : LayoutBindingInfo)
    {
        VkDescriptorSetLayoutBinding LayoutBinding = {};
        LayoutBinding.binding = Binding.Binding;
        LayoutBinding.descriptorCount = Binding.Count;
        LayoutBinding.descriptorType = Binding.DescriptorType;
        LayoutBinding.pImmutableSamplers = nullptr;
        LayoutBinding.stageFlags = Binding.StageFlags;

        LayoutBindingList.emplace_back(LayoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(LayoutBindingList.size());
    layoutInfo.pBindings = LayoutBindingList.data();

    VkDescriptorSetLayout descriptorSet;
    if (vkCreateDescriptorSetLayout(EnginePtr::GetEnginePtr()->Device, &layoutInfo, nullptr, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    return descriptorSet;
}

VkDescriptorSetLayout GraphicsPipeline::CreateDescriptorSetLayout(std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo, VkDescriptorSetLayoutBindingFlagsCreateInfoEXT& DescriptorSetLayoutBindingFlags)
{
    std::vector<VkDescriptorSetLayoutBinding> LayoutBindingList = {};

    for (auto Binding : LayoutBindingInfo)
    {
        VkDescriptorSetLayoutBinding LayoutBinding = {};
        LayoutBinding.binding = Binding.Binding;
        LayoutBinding.descriptorCount = Binding.Count;
        LayoutBinding.descriptorType = Binding.DescriptorType;
        LayoutBinding.pImmutableSamplers = nullptr;
        LayoutBinding.stageFlags = Binding.StageFlags;

        LayoutBindingList.emplace_back(LayoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(LayoutBindingList.size());
    layoutInfo.pBindings = LayoutBindingList.data();
    layoutInfo.pNext = &DescriptorSetLayoutBindingFlags;

    VkDescriptorSetLayout descriptorSet;
    if (vkCreateDescriptorSetLayout(EnginePtr::GetEnginePtr()->Device, &layoutInfo, nullptr, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    return descriptorSet;
}

VkDescriptorSet GraphicsPipeline::CreateDescriptorSets(VkDescriptorPool descriptorPool, VkDescriptorSetLayout layout)
{

    uint32_t variableDescCounts[] = { 1 };

    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT VariableDescriptorCountAllocateInfo{};
    VariableDescriptorCountAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    VariableDescriptorCountAllocateInfo.descriptorSetCount = 1;
    VariableDescriptorCountAllocateInfo.pDescriptorCounts = variableDescCounts;

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;
    allocInfo.pNext = &VariableDescriptorCountAllocateInfo;

    VkDescriptorSet DescriptorSets;
    if (vkAllocateDescriptorSets(EnginePtr::GetEnginePtr()->Device, &allocInfo, &DescriptorSets) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    return DescriptorSets;
}

VkDescriptorBufferInfo GraphicsPipeline::AddBufferDescriptor(VulkanBuffer& buffer)
{
    VkDescriptorBufferInfo BufferInfo = {};
    BufferInfo.buffer = buffer.Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = buffer.BufferSize;
    return BufferInfo;
}

VkWriteDescriptorSet GraphicsPipeline::AddBufferDescriptorSet(uint32_t BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo, VkDescriptorType descriptorType)
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

VkWriteDescriptorSet GraphicsPipeline::AddBufferDescriptorSet(uint32_t BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorBufferInfo>& BufferInfoList, VkDescriptorType descriptorType)
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

VkWriteDescriptorSet GraphicsPipeline::AddTextureDescriptorSet(uint32_t BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo)
{
    VkWriteDescriptorSet TextureDescriptor = {};
    TextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    TextureDescriptor.dstSet = DescriptorSet;
    TextureDescriptor.dstBinding = BindingNumber;
    TextureDescriptor.dstArrayElement = 0;
    TextureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    TextureDescriptor.descriptorCount = 1;
    TextureDescriptor.pImageInfo = &TextureImageInfo;
    return TextureDescriptor;
}

VkWriteDescriptorSet GraphicsPipeline::AddTextureDescriptorSet(uint32_t BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo>& TextureImageInfo)
{
    VkWriteDescriptorSet TextureDescriptor = {};
    TextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    TextureDescriptor.dstSet = DescriptorSet;
    TextureDescriptor.dstBinding = BindingNumber;
    TextureDescriptor.dstArrayElement = 0;
    TextureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    TextureDescriptor.descriptorCount = static_cast<uint32_t>(TextureImageInfo.size());
    TextureDescriptor.pImageInfo = TextureImageInfo.data();
    return TextureDescriptor;
}

VkWriteDescriptorSet GraphicsPipeline::AddStorageImageBuffer(uint32_t BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo)
{
    VkWriteDescriptorSet ImageDescriptor{};
    ImageDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    ImageDescriptor.dstSet = DescriptorSet;
    ImageDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    ImageDescriptor.dstBinding = BindingNumber;
    ImageDescriptor.pImageInfo = &TextureImageInfo;
    ImageDescriptor.descriptorCount = 1;
    return ImageDescriptor;
}

VkWriteDescriptorSet GraphicsPipeline::AddAccelerationBuffer(uint32_t BindingNumber, VkDescriptorSet& DescriptorSet, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure)
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

VkWriteDescriptorSetAccelerationStructureKHR GraphicsPipeline::AddAcclerationStructureBinding(VkAccelerationStructureKHR& handle)
{
    VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = {};
    AccelerationDescriptorStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    AccelerationDescriptorStructure.accelerationStructureCount = 1;
    AccelerationDescriptorStructure.pAccelerationStructures = &handle;
    return AccelerationDescriptorStructure;
}

VkDescriptorImageInfo GraphicsPipeline::AddRayTraceReturnImageDescriptor(VkImageLayout ImageLayout, VkImageView& ImageView)
{
    VkDescriptorImageInfo RayTraceImageDescriptor{};
    RayTraceImageDescriptor.imageView = ImageView;
    RayTraceImageDescriptor.imageLayout = ImageLayout;
    return RayTraceImageDescriptor;
}

VkDescriptorImageInfo GraphicsPipeline::AddTextureDescriptor(VkImageView view, VkSampler sampler)
{
    VkDescriptorImageInfo DescriptorImage{};
    DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    DescriptorImage.imageView = view;
    DescriptorImage.sampler = sampler;
    return DescriptorImage;
}

VkShaderModule GraphicsPipeline::ReadShaderFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(EnginePtr::GetEnginePtr()->Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

VkPipelineShaderStageCreateInfo GraphicsPipeline::CreateShader(const std::string& filename, VkShaderStageFlagBits shaderStages)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = shaderStages;
    vertShaderStageInfo.module = ReadShaderFile(filename);
    vertShaderStageInfo.pName = "main";

    return vertShaderStageInfo;
}