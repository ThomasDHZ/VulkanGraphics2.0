#include "Mesh.h"

Mesh::Mesh()
{
}


Mesh::Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, uint32_t meshID)
{
	MeshID = meshID;
	vertices = VertexList;
	indices = IndexList;
	material = Material();

	Transform = glm::mat4(1.0f);
	Transform = glm::transpose(Transform);

	VertexCount = vertices.size();
	IndexCount = indices.size();
	PrimitiveCount = static_cast<uint32_t>(indices.size()) / 3;

	SetUpMesh(engine, VertexList, IndexList);
}

Mesh::Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material MeshMaterial, uint32_t meshID)
{
	MeshID = meshID;
	vertices = VertexList;
	indices = IndexList;
	material = MeshMaterial;

	Transform = glm::mat4(1.0f);
	Transform = glm::transpose(Transform);

	VertexCount = vertices.size();
	IndexCount = indices.size();
	PrimitiveCount = static_cast<uint32_t>(indices.size()) / 3;

	SetUpMesh(engine, VertexList, IndexList);
}

Mesh::~Mesh()
{

}

void Mesh::MeshBottomLevelAccelerationStructure(VulkanEngine& engine)
{
}

void Mesh::SetUpMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());
	TransformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &Transform);

	VertexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(VertexBuffer.Buffer);
	IndexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(IndexBuffer.Buffer);
	TransformBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(TransformBuffer.Buffer);

	PrimitiveCount = IndexCount / 3;

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
	AccelerationStructureGeometry.geometry.triangles.transformData = TransformBufferDeviceAddress;

	AccelerationStructureBuildRangeInfo.primitiveCount = PrimitiveCount;
	AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
	AccelerationStructureBuildRangeInfo.firstVertex = 0;
	AccelerationStructureBuildRangeInfo.transformOffset = 0;

	MeshBottomLevelAccelerationStructure(engine);
}

void Mesh::Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline)
{
	MeshInfo meshInfo;
	meshInfo.MeshID = MeshID;
	meshInfo.MaterialID = 0;

	VkBuffer vertexBuffers[] = { VertexBuffer.Buffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdPushConstants(commandBuffer, pipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshInfo), &meshInfo);
	vkCmdDrawIndexed(commandBuffer, IndexCount, 1, 0, 0, 0);
}

void Mesh::Destory(VulkanEngine& engine)
{
	VertexBuffer.DestoryBuffer(engine.Device);
	IndexBuffer.DestoryBuffer(engine.Device);
	TransformBuffer.DestoryBuffer(engine.Device);
	MaterialBuffer.DestoryBuffer(engine.Device);
}