#include "Mesh.h"
#include "GraphicsPipeline.h"

Mesh::Mesh()
{
}

Mesh::Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList)
{
	std::vector<uint32_t> indices{};

	MeshID = engine.GenerateID();
	MeshProperties = MeshPropertiesUniformBuffer(engine);

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = VertexList.size();
	IndexCount = indices.size();
	PrimitiveCount = static_cast<uint32_t>(indices.size()) / 3;

	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
	SetUpMesh(engine, VertexList, indices);
}

Mesh::Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
	MeshID = engine.GenerateID();
	MeshProperties = MeshPropertiesUniformBuffer(engine);

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = VertexList.size();
	IndexCount = IndexList.size();
	PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;

	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
	SetUpMesh(engine, VertexList, IndexList);
}

Mesh::Mesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, uint32_t materialID)
{
	MeshID = engine.GenerateID();
	MaterialID = materialID;

	MeshProperties = MeshPropertiesUniformBuffer(engine);

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
	if (IndexCount != 0)
	{
		IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());
		IndexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(IndexBuffer.Buffer);
	}
	TransformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
	TransformInverseBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
	
	VertexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(VertexBuffer.Buffer);
	IndexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(IndexBuffer.Buffer);
	TransformInverseBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(TransformInverseBuffer.Buffer);

	PrimitiveCount = IndexCount / 3;

	AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	AccelerationStructureGeometry.flags = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;
	AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	AccelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	AccelerationStructureGeometry.geometry.triangles.vertexData = VertexBufferDeviceAddress;
	AccelerationStructureGeometry.geometry.triangles.maxVertex = VertexCount;
	AccelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
	AccelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	if (IndexCount != 0)
	{
		AccelerationStructureGeometry.geometry.triangles.indexData = IndexBufferDeviceAddress;
	}
	AccelerationStructureGeometry.geometry.triangles.transformData = TransformInverseBufferDeviceAddress;

	AccelerationStructureBuildRangeInfo.primitiveCount = PrimitiveCount;
	AccelerationStructureBuildRangeInfo.primitiveOffset = 0;
	AccelerationStructureBuildRangeInfo.firstVertex = 0;
	AccelerationStructureBuildRangeInfo.transformOffset = 0;

	if (IndexCount != 0)
	{
		MeshBottomLevelAccelerationStructure(engine);
	}
}

void Mesh::Update(VulkanEngine& engine, MaterialManager& materialManager)
{
	MeshProperties.UniformDataInfo.MaterialIndex = materialManager.GetMaterialBufferIDByMaterialID(MaterialID);

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	MeshTransform = glm::scale(MeshTransform, MeshScale);

	MeshProperties.UniformDataInfo.ModelTransform = glm::mat4(1.0f);
	glm::mat4 FinalTransform = MeshTransform;
	glm::mat4 transformMatrix2 = glm::transpose(MeshTransform);

	VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

	TransformBuffer.CopyBufferToMemory(engine.Device, &FinalTransform, sizeof(FinalTransform));
	TransformInverseBuffer.CopyBufferToMemory(engine.Device, &transformMatrix, sizeof(transformMatrix));
	MeshProperties.Update(engine);

	if (IndexCount != 0)
	{
		MeshBottomLevelAccelerationStructure(engine);
	}
}

void Mesh::Update(VulkanEngine& engine, const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList, MaterialManager& materialManager)
{
	MeshProperties.UniformDataInfo.MaterialIndex = materialManager.GetMaterialBufferIDByMaterialID(MaterialID);

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	MeshTransform = glm::scale(MeshTransform, MeshScale);

	if (BoneList.size() != 0)
	{
		for (auto bone : BoneList)
		{
			MeshProperties.UniformDataInfo.BoneTransform[bone->BoneID] = bone->FinalTransformMatrix;
		}
	}

	MeshProperties.UniformDataInfo.ModelTransform = ModelMatrix;
	glm::mat4 FinalTransform =  MeshTransform;
	glm::mat4 transformMatrix2 = glm::transpose(MeshTransform);

	VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

	TransformBuffer.CopyBufferToMemory(engine.Device, &FinalTransform, sizeof(FinalTransform));
	TransformInverseBuffer.CopyBufferToMemory(engine.Device, &transformMatrix, sizeof(transformMatrix));
	MeshProperties.Update(engine);

	if (IndexCount != 0)
	{
		MeshBottomLevelAccelerationStructure(engine);
	}
}

void Mesh::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	if (ShowMesh)
	{
		ConstMeshInfo meshInfo;
		meshInfo.MeshIndex = MeshBufferIndex;

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VertexBuffer.Buffer, offsets);
		vkCmdPushConstants(commandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ConstMeshInfo), &meshInfo);
		if (IndexCount == 0)
		{
			vkCmdDraw(commandBuffer, VertexCount, 1, 0, 0);
		}
		else
		{
			vkCmdBindIndexBuffer(commandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32); 
			vkCmdDrawIndexed(commandBuffer, IndexCount, 1, 0, 0, 0);
		}
	}
}

void Mesh::Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo)
{
	if (ShowMesh)
	{
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VertexBuffer.Buffer, offsets);
		if (IndexCount == 0)
		{
			vkCmdDraw(commandBuffer, VertexCount, 1, 0, 0);
		}
		else
		{
			vkCmdBindIndexBuffer(commandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(commandBuffer, IndexCount, 1, 0, 0, 0);
		}
	}
}

void Mesh::Destory(VulkanEngine& engine)
{
	VertexBuffer.DestoryBuffer(engine.Device);
	IndexBuffer.DestoryBuffer(engine.Device);
	TransformBuffer.DestoryBuffer(engine.Device);
	TransformInverseBuffer.DestoryBuffer(engine.Device);
	MeshProperties.Destroy(engine);
	BottomLevelAccelerationBuffer.Destroy(engine);
}