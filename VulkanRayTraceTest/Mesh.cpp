#include "Mesh.h"

Mesh::Mesh()
{
}


Mesh::Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, uint32_t meshID)
{
	MeshID = meshID;
	material = Material();

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = VertexList.size();
	IndexCount = IndexList.size();
	PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;

	SetUpMesh(engine, VertexList, IndexList);
}

Mesh::Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material MeshMaterial, uint32_t meshID)
{
	MeshID = meshID;
	material = MeshMaterial;

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = VertexList.size();
	IndexCount = IndexList.size();
	PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;

	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
	SetUpMesh(engine, VertexList, IndexList);
}

Mesh::~Mesh()
{

}

void Mesh::MeshBottomLevelAccelerationStructure(VulkanEngine& engine)
{
	std::vector<uint32_t> PrimitiveCountList{ PrimitiveCount };
	std::vector<VkAccelerationStructureGeometryKHR> AccelerationStructureGeometryList{ AccelerationStructureGeometry };
	std::vector<VkAccelerationStructureBuildRangeInfoKHR> AccelerationBuildStructureRangeInfos{ AccelerationStructureBuildRangeInfo };

	VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo = {};
	AccelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	AccelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;;
	AccelerationStructureBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
	AccelerationStructureBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();

	PrimitiveCountList.resize(AccelerationBuildStructureRangeInfos.size());
	for (auto x = 0; x < AccelerationBuildStructureRangeInfos.size(); x++)
	{
		PrimitiveCountList[x] = AccelerationBuildStructureRangeInfos[x].primitiveCount;
	}

	VkAccelerationStructureBuildSizesInfoKHR AccelerationStructureBuildSizesInfo = {};
	AccelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	engine.vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, PrimitiveCountList.data(), &AccelerationStructureBuildSizesInfo);

	if (BottomLevelAccelerationBuffer.handle == VK_NULL_HANDLE)
	{
		BottomLevelAccelerationBuffer.CreateAccelerationStructure(engine, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, AccelerationStructureBuildSizesInfo);
	}

	VulkanBuffer scratchBuffer = VulkanBuffer(engine.Device, engine.PhysicalDevice, AccelerationStructureBuildSizesInfo.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	scratchBuffer.BufferDeviceAddress = engine.GetBufferDeviceAddress(scratchBuffer.Buffer);

	VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
	AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
	AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	AccelerationBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
	AccelerationBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();
	AccelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.BufferDeviceAddress;
	if (BottomLevelAccelerationBuffer.handle == VK_NULL_HANDLE)
	{
		AccelerationBuildGeometryInfo.dstAccelerationStructure = BottomLevelAccelerationBuffer.handle;
	}
	else
	{
		AccelerationBuildGeometryInfo.srcAccelerationStructure = BottomLevelAccelerationBuffer.handle;
		AccelerationBuildGeometryInfo.dstAccelerationStructure = BottomLevelAccelerationBuffer.handle;
	}

	BottomLevelAccelerationBuffer.AcclerationCommandBuffer(engine, AccelerationBuildGeometryInfo, AccelerationBuildStructureRangeInfos);

	scratchBuffer.DestoryBuffer(engine.Device);
}

void Mesh::SetUpMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
	VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress;
	VkDeviceOrHostAddressConstKHR IndexBufferDeviceAddress;
	VkDeviceOrHostAddressConstKHR TransformInverseBufferDeviceAddress;

	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());
	TransformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
	TransformInverseBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
	MaterialBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(Material), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &material);

	VertexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(VertexBuffer.Buffer);
	IndexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(IndexBuffer.Buffer);
	TransformInverseBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(TransformInverseBuffer.Buffer);

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
	AccelerationStructureGeometry.geometry.triangles.transformData = TransformInverseBufferDeviceAddress;

	AccelerationStructureBuildRangeInfo.primitiveCount = PrimitiveCount;
	AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
	AccelerationStructureBuildRangeInfo.firstVertex = 0;
	AccelerationStructureBuildRangeInfo.transformOffset = 0;

	MeshBottomLevelAccelerationStructure(engine);

	MeshBottomLevelAccelerationStructure(engine);
}

void Mesh::Update(VulkanEngine& engine)
{
	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	MeshTransform = glm::scale(MeshTransform, MeshScale);

	glm::mat4 FinalTransform = MeshTransform;
	glm::mat4 transformMatrix2 = glm::transpose(MeshTransform);
	VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

	TransformBuffer.CopyBufferToMemory(engine.Device, &MeshTransform, sizeof(MeshTransform));
	TransformInverseBuffer.CopyBufferToMemory(engine.Device, &transformMatrix, sizeof(transformMatrix));

	MeshBottomLevelAccelerationStructure(engine);
}

void Mesh::Update(VulkanEngine& engine, const std::vector<std::shared_ptr<Bone>>& BoneList, std::shared_ptr<SceneDataStruct> scenedata)
{
	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	MeshTransform = glm::scale(MeshTransform, MeshScale);

	for (auto bone : BoneList)
	{
		scenedata->SceneData.BoneTransform[bone->BoneID] = bone->FinalTransformMatrix;
	}

	glm::mat4 FinalTransform = MeshTransform;
	glm::mat4 transformMatrix2 = glm::transpose(MeshTransform);
	VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

	TransformBuffer.CopyBufferToMemory(engine.Device, &MeshTransform, sizeof(MeshTransform));
	TransformInverseBuffer.CopyBufferToMemory(engine.Device, &transformMatrix, sizeof(transformMatrix));

	std::vector<Vertex> newVertexList = VertexList;
	for (auto& vertex : newVertexList)
	{
		glm::mat4 BoneTransform = glm::mat4(1.0f);
		BoneTransform = scenedata->SceneData.BoneTransform[vertex.BoneID[0]] * vertex.BoneWeights[0];
		BoneTransform += scenedata->SceneData.BoneTransform[vertex.BoneID[1]] * vertex.BoneWeights[1];
		BoneTransform += scenedata->SceneData.BoneTransform[vertex.BoneID[2]] * vertex.BoneWeights[2];
		BoneTransform += scenedata->SceneData.BoneTransform[vertex.BoneID[3]] * vertex.BoneWeights[3];
		vertex.Position = glm::vec3(BoneTransform * glm::vec4(vertex.Position, 1.0));
	}
	VertexBuffer.CopyBufferToMemory(engine.Device, &newVertexList[0], sizeof(Vertex) * newVertexList.size());
	MeshBottomLevelAccelerationStructure(engine);
}

void Mesh::Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline)
{
	if (ShowMesh)
	{
		MeshInfo meshInfo;
		meshInfo.MeshID = MeshID;
		meshInfo.ModelID = 0;
		meshInfo.MaterialID = 0;


		VkBuffer vertexBuffers[] = { VertexBuffer.Buffer };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdPushConstants(commandBuffer, pipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshInfo), &meshInfo);
		vkCmdDrawIndexed(commandBuffer, IndexCount, 1, 0, 0, 0);
	}
}

void Mesh::Destory(VulkanEngine& engine)
{
	VertexBuffer.DestoryBuffer(engine.Device);
	IndexBuffer.DestoryBuffer(engine.Device);
	TransformBuffer.DestoryBuffer(engine.Device);
	TransformInverseBuffer.DestoryBuffer(engine.Device);
	MaterialBuffer.DestoryBuffer(engine.Device);
	BottomLevelAccelerationBuffer.Destroy(engine);
}