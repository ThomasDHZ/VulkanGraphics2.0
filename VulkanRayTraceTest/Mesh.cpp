#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList)
{
}

Mesh::Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList, const std::vector<uint32_t>& IndexList)
{
}

Mesh::Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList, Material material)
{
}

Mesh::Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList, const std::vector<uint32_t>& IndexList, Material material)
{
}

Mesh::~Mesh()
{

}

void Mesh::SetUpDescriptorPool(VulkanEngine& engine)
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};

    DescriptorPoolList.emplace_back(AddDsecriptorPoolBinding(engine, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    DescriptorPoolList.emplace_back(AddDsecriptorPoolBinding(engine, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(DescriptorPoolList.size());
    poolInfo.pPoolSizes = DescriptorPoolList.data();
    poolInfo.maxSets = static_cast<uint32_t>(engine.SwapChain.GetSwapChainImageCount());

    if (vkCreateDescriptorPool(engine.Device, &poolInfo, nullptr, &DescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void Mesh::SetUpDescriptorSets(VulkanEngine& engine, GraphicsPipeline& pipeline, std::shared_ptr<Texture> texture)
{
    std::vector<VkDescriptorSetLayout> layouts(engine.SwapChain.GetSwapChainImageCount(), pipeline.ShaderPipelineDescriptorLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(engine.SwapChain.GetSwapChainImageCount());
    allocInfo.pSetLayouts = layouts.data();

    DescriptorSets.resize(engine.SwapChain.GetSwapChainImageCount());
    if (vkAllocateDescriptorSets(engine.Device, &allocInfo, DescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    VkDescriptorBufferInfo PositionInfo = AddBufferDescriptorInfo(engine, UniformBufferList);
    VkDescriptorImageInfo DiffuseMap = AddImageDescriptorInfo(engine, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture);

    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkWriteDescriptorSet> DescriptorList;
        DescriptorList.emplace_back(AddDescriptorSetBufferInfo(engine, 0, DescriptorSets[i], PositionInfo));
        DescriptorList.emplace_back(AddDescriptorSetTextureInfo(engine, 1, DescriptorSets[i], DiffuseMap));

        vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
    }
}

VkDescriptorPoolSize Mesh::AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType)
{
	VkDescriptorPoolSize DescriptorPoolBinding = {};
	DescriptorPoolBinding.type = descriptorType;
	DescriptorPoolBinding.descriptorCount = static_cast<uint32_t>(engine.SwapChain.GetSwapChainImageCount());

	return DescriptorPoolBinding;
}

VkDescriptorImageInfo Mesh::AddImageDescriptorInfo(VulkanEngine& engine, VkImageLayout ImageLayout, std::shared_ptr<Texture> texture)
{
	VkDescriptorImageInfo DescriptorImage = {};
	DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	DescriptorImage.imageView = texture->GetTextureView();
	DescriptorImage.sampler = texture->GetTextureSampler();
	return DescriptorImage;
}


VkDescriptorBufferInfo Mesh::AddBufferDescriptorInfo(VulkanEngine& engine, VulkanBuffer buffer)
{
	VkDescriptorBufferInfo BufferInfo = {};
	BufferInfo.buffer = buffer.Buffer;
	BufferInfo.offset = 0;
	BufferInfo.range = buffer.BufferSize;
	return BufferInfo;
}

VkWriteDescriptorSet Mesh::AddDescriptorSetBufferInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo)
{
	VkWriteDescriptorSet BufferDescriptor = {};
	BufferDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	BufferDescriptor.dstSet = DescriptorSet;
	BufferDescriptor.dstBinding = BindingNumber;
	BufferDescriptor.dstArrayElement = 0;
	BufferDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	BufferDescriptor.descriptorCount = 1;
	BufferDescriptor.pBufferInfo = &BufferInfo;
	return BufferDescriptor;
}

VkWriteDescriptorSet Mesh::AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo)
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