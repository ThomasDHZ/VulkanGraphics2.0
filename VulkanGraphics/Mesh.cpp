#include "Mesh.h"
#include "GraphicsPipeline.h"

Mesh::Mesh()
{
}

Mesh::Mesh(std::vector<Vertex>& VertexList, int MeshDrawFlags)
{
	std::vector<uint32_t> indices{};

	MeshID = EnginePtr::GetEnginePtr()->GenerateID();
	MeshProperties = MeshPropertiesUniformBuffer(EnginePtr::GetEnginePtr());
	DrawFlags = MeshDrawFlags;

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = VertexList.size();
	IndexCount = indices.size();
	PrimitiveCount = static_cast<uint32_t>(indices.size()) / 3;
	BoneCount = 0;
	MeshMaterial = MaterialManagerPtr::GetMaterialManagerPtr()->GetDefaultMaterial();
	MeshTimer = std::make_shared<Timer>(Timer());

	BottomLevelAccelerationBuffer = AccelerationStructure(EnginePtr::GetEnginePtr());
	SetUpMesh(VertexList, indices);
}

Mesh::Mesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, int MeshDrawFlags)
{
	MeshID = EnginePtr::GetEnginePtr()->GenerateID();
	MeshProperties = MeshPropertiesUniformBuffer(EnginePtr::GetEnginePtr());
	DrawFlags = MeshDrawFlags;

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = VertexList.size();
	IndexCount = IndexList.size();
	PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;
	BoneCount = 0;
	MeshMaterial = MaterialManagerPtr::GetMaterialManagerPtr()->GetDefaultMaterial();
	MeshTimer = std::make_shared<Timer>(Timer());

	BottomLevelAccelerationBuffer = AccelerationStructure(EnginePtr::GetEnginePtr());
	SetUpMesh(VertexList, IndexList);
}

Mesh::Mesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, int MeshDrawFlags)
{
	MeshID = EnginePtr::GetEnginePtr()->GenerateID();
	MeshMaterial = material;
	DrawFlags = MeshDrawFlags;

	MeshProperties = MeshPropertiesUniformBuffer(EnginePtr::GetEnginePtr());

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = VertexList.size();
	IndexCount = IndexList.size();
	PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;
	BoneCount = 0;
	MeshTimer = std::make_shared<Timer>(Timer());

	BottomLevelAccelerationBuffer = AccelerationStructure(EnginePtr::GetEnginePtr());
	SetUpMesh(VertexList, IndexList);
}

Mesh::Mesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, std::vector<MeshBoneWeights>& boneWeights, uint32_t boneCount, int MeshDrawFlags)
{
	MeshID = EnginePtr::GetEnginePtr()->GenerateID();
	MeshMaterial = material;
	DrawFlags = MeshDrawFlags;

	MeshProperties = MeshPropertiesUniformBuffer(EnginePtr::GetEnginePtr());

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = VertexList.size();
	IndexCount = IndexList.size();
	PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;
	BoneCount = boneCount;
	BoneTransform.resize(BoneCount, glm::mat4(1.0f));
	BoneWeightList = boneWeights;
	MeshTimer = std::make_shared<Timer>(Timer());

	BottomLevelAccelerationBuffer = AccelerationStructure(EnginePtr::GetEnginePtr());
	SetUpMesh(VertexList, IndexList);
}

Mesh::~Mesh()
{

}

void Mesh::MeshBottomLevelAccelerationStructure()
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
	EnginePtr::GetEnginePtr()->vkGetAccelerationStructureBuildSizesKHR(VulkanPtr::GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, PrimitiveCountList.data(), &AccelerationStructureBuildSizesInfo);

	if (BottomLevelAccelerationBuffer.handle == VK_NULL_HANDLE)
	{
		BottomLevelAccelerationBuffer.CreateAccelerationStructure(EnginePtr::GetEnginePtr(), VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, AccelerationStructureBuildSizesInfo);
	}

	VulkanBuffer scratchBuffer = VulkanBuffer(AccelerationStructureBuildSizesInfo.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	scratchBuffer.BufferDeviceAddress = EnginePtr::GetEnginePtr()->GetBufferDeviceAddress(scratchBuffer.Buffer);

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

	BottomLevelAccelerationBuffer.AcclerationCommandBuffer(EnginePtr::GetEnginePtr(), AccelerationBuildGeometryInfo, AccelerationBuildStructureRangeInfos);

	scratchBuffer.DestoryBuffer();
}

void Mesh::SetUpMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
	VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress;
	VkDeviceOrHostAddressConstKHR IndexBufferDeviceAddress;
	VkDeviceOrHostAddressConstKHR TransformInverseBufferDeviceAddress;

	VertexBuffer.CreateBuffer(VertexList.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
	if (IndexCount != 0)
	{
		IndexBuffer.CreateBuffer(IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());
		IndexBufferDeviceAddress.deviceAddress = EnginePtr::GetEnginePtr()->GetBufferDeviceAddress(IndexBuffer.Buffer);
	}
	if (BoneCount != 0)
	{
		BoneWeightBuffer.CreateBuffer(sizeof(MeshBoneWeights) * BoneWeightList.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, BoneWeightList.data());
		BoneTransformBuffer.CreateBuffer(sizeof(glm::mat4) * BoneTransform.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, BoneTransform.data());
	}
	TransformBuffer.CreateBuffer(sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
	TransformInverseBuffer.CreateBuffer(sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MeshTransform);
	
	if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible)
	{
		VertexBufferDeviceAddress.deviceAddress = EnginePtr::GetEnginePtr()->GetBufferDeviceAddress(VertexBuffer.Buffer);
		IndexBufferDeviceAddress.deviceAddress = EnginePtr::GetEnginePtr()->GetBufferDeviceAddress(IndexBuffer.Buffer);
		TransformInverseBufferDeviceAddress.deviceAddress = EnginePtr::GetEnginePtr()->GetBufferDeviceAddress(TransformInverseBuffer.Buffer);

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
			MeshBottomLevelAccelerationStructure();
		}
	}
}

void Mesh::Update()
{
	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	MeshTransform = glm::scale(MeshTransform, MeshScale);

	MeshProperties.UniformDataInfo.material = MeshMaterial->MaterialBufferData;
	MeshProperties.UniformDataInfo.MaterialBufferIndex = MeshMaterial->GetMaterialBufferIndex();
	MeshProperties.UniformDataInfo.MeshTransform = MeshTransform;
	MeshProperties.UniformDataInfo.UVOffset = UVOffset;
	MeshProperties.UniformDataInfo.UVScale = UVScale;
	MeshProperties.UniformDataInfo.UVFlip = UVFlip;
	MeshProperties.UniformDataInfo.ModelTransform = glm::mat4(1.0f);

	glm::mat4 FinalTransform = MeshTransform;
	glm::mat4 transformMatrix2 = glm::transpose(MeshTransform);

	VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

	TransformBuffer.CopyBufferToMemory(&FinalTransform, sizeof(FinalTransform));
	TransformInverseBuffer.CopyBufferToMemory(&transformMatrix, sizeof(transformMatrix));

	MeshProperties.Update();

	if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible &&
		EnginePtr::GetEnginePtr()->RayTraceFlag &&
		IndexCount != 0)
	{
		MeshBottomLevelAccelerationStructure();
	}
}

void Mesh::Update(const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList)
{
	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	MeshTransform = glm::scale(MeshTransform, MeshScale);

	MeshProperties.UniformDataInfo.material = MeshMaterial->MaterialBufferData;
	MeshProperties.UniformDataInfo.MaterialBufferIndex = MeshMaterial->GetMaterialBufferIndex();
	MeshProperties.UniformDataInfo.ModelTransform = ModelMatrix;
	MeshProperties.UniformDataInfo.MeshTransform = MeshTransform;
	MeshProperties.UniformDataInfo.UVOffset = UVOffset;
	MeshProperties.UniformDataInfo.UVScale = UVScale;
	MeshProperties.UniformDataInfo.UVFlip = UVFlip;

	if (BoneList.size() != 0)
	{
		for (auto bone : BoneList)
		{
			BoneTransform[bone->BoneID] = bone->FinalTransformMatrix;
		}
		BoneTransformBuffer.CopyBufferToMemory(BoneTransform.data(), sizeof(glm::mat4) * BoneTransform.size());
	}

	glm::mat4 FinalTransform =  MeshTransform;
	glm::mat4 transformMatrix2 = glm::transpose(MeshTransform);

	VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

	TransformBuffer.CopyBufferToMemory(&FinalTransform, sizeof(FinalTransform));
	TransformInverseBuffer.CopyBufferToMemory(&transformMatrix, sizeof(transformMatrix));

	MeshProperties.Update();

	if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible &&
		EnginePtr::GetEnginePtr()->RayTraceFlag &&
		IndexCount != 0)
	{
		MeshBottomLevelAccelerationStructure();
	}
}

void Mesh::Draw(VkCommandBuffer& commandBuffer)
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

void Mesh::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& ShaderLayout, std::shared_ptr<Camera> CameraView)
{
    if (ShowMesh)
	{
		ConstMeshInfo meshInfo;
		meshInfo.MeshIndex = MeshBufferIndex;
		meshInfo.CameraPos = CameraView->GetPosition();
		meshInfo.view = CameraView->GetViewMatrix();
		meshInfo.proj = CameraView->GetProjectionMatrix();
		meshInfo.proj[1][1] *= -1;

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VertexBuffer.Buffer, offsets);
		vkCmdPushConstants(commandBuffer, ShaderLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ConstMeshInfo), &meshInfo);
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

void Mesh::DepthDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout& ShaderLayout, std::shared_ptr<Camera> lightViewCamera)
{
	if (ShowMesh)
	{
		LightSceneInfo lightSceneInfo;
		lightSceneInfo.MeshIndex = MeshBufferIndex;
		lightSceneInfo.lightSpaceMatrix = lightViewCamera->LightSpaceMatrix;

		VkDeviceSize offsets[] = { 0 };

		vkCmdPushConstants(commandBuffer, ShaderLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(LightSceneInfo), &lightSceneInfo);
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VertexBuffer.Buffer, offsets);
		if(IndexCount == 0)
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

void Mesh::Destory()
{
	VertexBuffer.DestoryBuffer();
	IndexBuffer.DestoryBuffer();
	TransformBuffer.DestoryBuffer();
	TransformInverseBuffer.DestoryBuffer();
	MeshProperties.Destroy();

	if (BoneTransformBuffer.BufferMemory != nullptr &&
		BoneTransformBuffer.Buffer != nullptr)
	{
		BoneWeightBuffer.DestoryBuffer();
		BoneTransformBuffer.DestoryBuffer();
	}
	if (BottomLevelAccelerationBuffer.handle != VK_NULL_HANDLE)
	{
		BottomLevelAccelerationBuffer.Destroy(EnginePtr::GetEnginePtr());
	}
}