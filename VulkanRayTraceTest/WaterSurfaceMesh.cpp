#include "WaterSurfaceMesh.h"

WaterSurfaceMesh::WaterSurfaceMesh() : Mesh()
{
}

WaterSurfaceMesh::WaterSurfaceMesh(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> SceneData, std::shared_ptr<Texture> reflecttexture) : Mesh()
{
	std::vector<Vertex> WaterVertices =
	{
		{{ 0.0f, 0.0f, 0.0f }, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, 0.0f, 0.0f }, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 1.0f, 1.0f, 0.0f }, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 0.0f, 1.0f, 0.0f }, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}}
	};

	std::vector<uint32_t> WaterIndices =
	{
		0, 1, 3,
		1, 2, 3
	};

	MeshID = engine.GenerateID();
	MeshProperties = MeshPropertiesUniformBuffer(engine);
	DrawFlags = MeshDrawFlags::Mesh_Skip_Water_Renderer;

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = WaterVertices.size();
	IndexCount = WaterIndices.size();
	PrimitiveCount = static_cast<uint32_t>(WaterIndices.size()) / 3;

	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
	SetUpMesh(engine, WaterVertices, WaterIndices);
	waterSurfacePipeline = std::make_shared<WaterSurfacePipeline>(WaterSurfacePipeline(engine, assetManager, SceneData, RenderPass, reflecttexture));
}

WaterSurfaceMesh::~WaterSurfaceMesh()
{
}

void WaterSurfaceMesh::UpdateGraphicsPipeLine(VulkanEngine& engine, VkRenderPass& RenderPass)
{
	waterSurfacePipeline->UpdateGraphicsPipeLine(engine, RenderPass);
}

void WaterSurfaceMesh::Update(VulkanEngine& engine, MaterialManager& materialManager)
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

	TransformBuffer.CopyBufferToMemory(engine.Device, &FinalTransform, sizeof(FinalTransform));
	MeshProperties.Update(engine);
}

void WaterSurfaceMesh::Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID, int a)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, waterSurfacePipeline->ShaderPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, waterSurfacePipeline->ShaderPipelineLayout, 0, 1, &waterSurfacePipeline->DescriptorSets, 0, nullptr);
	Mesh::Draw(commandBuffer, renderPassInfo, RendererID);
}

void WaterSurfaceMesh::Destory(VulkanEngine& engine)
{
	waterSurfacePipeline->Destroy(engine);
	Mesh::Destory(engine);
}