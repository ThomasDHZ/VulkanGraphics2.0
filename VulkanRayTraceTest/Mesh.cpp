//#include "Mesh.h"
//
//Mesh::Mesh()
//{
//}
//
//
//Mesh::Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
//{
//	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetBufferDeviceAddressKHR"));
//
//	VertexCount = VertexList.size();
//	IndexCount = IndexList.size();
//
//	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexCount * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
//	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexCount * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());
//	UniformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//}
//
//Mesh::Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material MeshMaterial)
//{
//	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetBufferDeviceAddressKHR"));
//
//	material = MeshMaterial;
//	VertexCount = VertexList.size();
//	IndexCount = IndexList.size();
//
//	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexCount * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
//	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexCount * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());
//	UniformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//}
//
//Mesh::~Mesh()
//{
//
//}
//
//void Mesh::SetUpMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
//{
//	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexCount * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
//	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexCount * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());
//	TransformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Transform);
//	MaterialBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(Material), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &material);
//
//	VertexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(engine.Device, VertexBuffer.Buffer);
//	IndexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(engine.Device, IndexBuffer.Buffer);
//	TransformBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(engine.Device, TransformBuffer.Buffer);
//
//	PrimitiveCountList = IndexCount / 3;
//
//	AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
//	AccelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
//	AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
//	AccelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
//	AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
//	AccelerationStructureGeometry.geometry.triangles.vertexData = VertexBufferDeviceAddress;
//	AccelerationStructureGeometry.geometry.triangles.maxVertex = VertexCount;
//	AccelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
//	AccelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
//	AccelerationStructureGeometry.geometry.triangles.indexData = IndexBufferDeviceAddress;
//	AccelerationStructureGeometry.geometry.triangles.transformData = TransformBufferDeviceAddress;
//
//	AccelerationStructureBuildRangeInfo.primitiveCount = PrimitiveCountList;
//	AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
//	AccelerationStructureBuildRangeInfo.firstVertex = 0;
//	AccelerationStructureBuildRangeInfo.transformOffset = 0;
//}
//
//VkDescriptorPoolSize Mesh::AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType)
//{
//	VkDescriptorPoolSize DescriptorPoolBinding = {};
//	DescriptorPoolBinding.type = descriptorType;
//	DescriptorPoolBinding.descriptorCount = static_cast<uint32_t>(engine.SwapChain.GetSwapChainImageCount());
//
//	return DescriptorPoolBinding;
//}
//
//VkDescriptorImageInfo Mesh::AddImageDescriptorInfo(VulkanEngine& engine, VkImageLayout ImageLayout, std::shared_ptr<Texture> texture)
//{
//	VkDescriptorImageInfo DescriptorImage = {};
//	DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//	DescriptorImage.imageView = texture->GetTextureView();
//	DescriptorImage.sampler = texture->GetTextureSampler();
//	return DescriptorImage;
//}
//
//
//VkDescriptorBufferInfo Mesh::AddBufferDescriptorInfo(VulkanEngine& engine, VulkanBuffer buffer)
//{
//	VkDescriptorBufferInfo BufferInfo = {};
//	BufferInfo.buffer = buffer.Buffer;
//	BufferInfo.offset = 0;
//	BufferInfo.range = buffer.BufferSize;
//	return BufferInfo;
//}
//
//VkWriteDescriptorSet Mesh::AddDescriptorSetBufferInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo)
//{
//	VkWriteDescriptorSet BufferDescriptor = {};
//	BufferDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//	BufferDescriptor.dstSet = DescriptorSet;
//	BufferDescriptor.dstBinding = BindingNumber;
//	BufferDescriptor.dstArrayElement = 0;
//	BufferDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	BufferDescriptor.descriptorCount = 1;
//	BufferDescriptor.pBufferInfo = &BufferInfo;
//	return BufferDescriptor;
//}
//
//VkWriteDescriptorSet Mesh::AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo> TextureImageList)
//{
//	VkWriteDescriptorSet TextureDescriptor = {};
//	TextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//	TextureDescriptor.dstSet = DescriptorSet;
//	TextureDescriptor.dstBinding = BindingNumber;
//	TextureDescriptor.dstArrayElement = 0;
//	TextureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	TextureDescriptor.descriptorCount = static_cast<uint32_t>(TextureImageList.size());
//	TextureDescriptor.pImageInfo = TextureImageList.data();
//	return TextureDescriptor;
//}
//
//void Mesh::Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline, int index)
//{
//	struct MeshInfo
//	{
//		int MeshID = 0;
//		int MaterialID = 0;
//	} meshInfo;
//	meshInfo.MeshID = index;
//	meshInfo.MaterialID = index;
//
//	VkBuffer vertexBuffers[] = { VertexBuffer.Buffer };
//	VkDeviceSize offsets[] = { 0 };
//
//	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
//	vkCmdBindIndexBuffer(commandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
//	vkCmdPushConstants(commandBuffer, pipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshInfo), &meshInfo);
//	vkCmdDrawIndexed(commandBuffer, IndexCount, 1, 0, 0, 0);
//}
//
//void Mesh::Destory(VulkanEngine& engine)
//{
//	VertexBuffer.DestoryBuffer(engine.Device);
//	IndexBuffer.DestoryBuffer(engine.Device);
//	TransformBuffer.DestoryBuffer(engine.Device);
//	UniformBuffer.DestoryBuffer(engine.Device);
//	MaterialBuffer.DestoryBuffer(engine.Device);
//}
//
//uint64_t Mesh::getBufferDeviceAddress(VkDevice& device, VkBuffer buffer)
//{
//	VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
//	bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
//	bufferDeviceAI.buffer = buffer;
//	return vkGetBufferDeviceAddressKHR(device, &bufferDeviceAI);
//}