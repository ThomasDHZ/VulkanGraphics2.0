#include "Mesh.h"

Mesh::Mesh()
{
}


Mesh::Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList, const std::vector<uint32_t>& IndexList, std::shared_ptr<GraphicsPipeline> pipeline, std::shared_ptr<Texture> texture)
{
	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.data());
	UniformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	SetUpDescriptorPool(engine);
	SetUpDescriptorSets(engine, pipeline, texture);
}

Mesh::Mesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList, const std::vector<uint32_t>& IndexList, Material MeshMaterial, std::shared_ptr<GraphicsPipeline> pipeline, std::shared_ptr<Texture> texture)
{
	material = MeshMaterial;
	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.data());
	UniformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	SetUpDescriptorPool(engine);
	SetUpDescriptorSets(engine, pipeline, texture);
}

Mesh::~Mesh()
{

}

void Mesh::SetUpMesh(VulkanEngine& engine, const std::vector<Vertex>& VertexList, const std::vector<uint32_t>& IndexList)
{
	VkAccelerationStructureGeometryKHR AccelerationStructureGeometry = {};
	AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	AccelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	AccelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	AccelerationStructureGeometry.geometry.triangles.vertexData = VertexBufferDeviceAddress;
	AccelerationStructureGeometry.geometry.triangles.maxVertex = VertexCount;
	AccelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
	AccelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	AccelerationStructureGeometry.geometry.triangles.indexData = IndexBufferDeviceAddress;

	VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo{};
	AccelerationStructureBuildRangeInfo.primitiveCount = PrimitiveCountList;
	AccelerationStructureBuildRangeInfo.primitiveOffset = FirstIndex * sizeof(uint32_t);
	AccelerationStructureBuildRangeInfo.firstVertex = VertexOffset;
	AccelerationStructureBuildRangeInfo.transformOffset = 0;

	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.data());
	UniformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	//VulkanBuffer IndexBuffer;
	//VulkanBuffer VertexBuffer;
	//VulkanBuffer UniformBuffer;
	//VulkanBuffer TransformBuffer;
	//VulkanBuffer MaterialBuffer;
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

void Mesh::SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<GraphicsPipeline> pipeline, std::shared_ptr<Texture> texture)
{
    //std::vector<VkDescriptorSetLayout> layouts(engine.SwapChain.GetSwapChainImageCount(), pipeline->ShaderPipelineDescriptorLayout);

    //VkDescriptorSetAllocateInfo allocInfo{};
    //allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    //allocInfo.descriptorPool = DescriptorPool;
    //allocInfo.descriptorSetCount = static_cast<uint32_t>(engine.SwapChain.GetSwapChainImageCount());
    //allocInfo.pSetLayouts = layouts.data();

    //DescriptorSets.resize(engine.SwapChain.GetSwapChainImageCount());
    //if (vkAllocateDescriptorSets(engine.Device, &allocInfo, DescriptorSets.data()) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to allocate descriptor sets!");
    //}

    //VkDescriptorBufferInfo PositionInfo = AddBufferDescriptorInfo(engine, UniformBuffer);
    //VkDescriptorImageInfo DiffuseMap = AddImageDescriptorInfo(engine, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture);

    //for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++)
    //{
    //    std::vector<VkWriteDescriptorSet> DescriptorList;
    //    DescriptorList.emplace_back(AddDescriptorSetBufferInfo(engine, 0, DescriptorSets[i], PositionInfo));
    //    DescriptorList.emplace_back(AddDescriptorSetTextureInfo(engine, 1, DescriptorSets[i], DiffuseMap));

    //    vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
    //}
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

VkWriteDescriptorSet Mesh::AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo> TextureImageList)
{
	VkWriteDescriptorSet TextureDescriptor = {};
	TextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	TextureDescriptor.dstSet = DescriptorSet;
	TextureDescriptor.dstBinding = BindingNumber;
	TextureDescriptor.dstArrayElement = 0;
	TextureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	TextureDescriptor.descriptorCount = static_cast<uint32_t>(TextureImageList.size());
	TextureDescriptor.pImageInfo = TextureImageList.data();
	return TextureDescriptor;
}

void Mesh::Draw(std::vector<VkCommandBuffer> commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline, int index)
{
	VkBuffer vertexBuffers[] = { VertexBuffer.Buffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindPipeline(commandBuffer[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->ShaderPipeline);
	vkCmdBindVertexBuffers(commandBuffer[index], 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer[index], IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandBuffer[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->ShaderPipelineLayout, 0, 1, &DescriptorSets[index], 0, nullptr);
	vkCmdDrawIndexed(commandBuffer[index], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void Mesh::Destory(VulkanEngine& engine)
{
	VertexBuffer.DestoryBuffer(engine.Device);
	IndexBuffer.DestoryBuffer(engine.Device);
	TransformBuffer.DestoryBuffer(engine.Device);
	UniformBuffer.DestoryBuffer(engine.Device);
	MaterialBuffer.DestoryBuffer(engine.Device);
}