#include "WaterSurfaceMesh.h"

WaterSurfaceMesh::WaterSurfaceMesh() : Mesh()
{
}

WaterSurfaceMesh::WaterSurfaceMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> SceneData, std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer) : Mesh()
{
	std::vector<Vertex> WaterVertices =
	{
		{{ 0.0f, 0.0f, 0.0f }, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
		{{ 5.0f, 0.0f, 0.0f }, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
		{{ 5.0f, 5.0f, 0.0f }, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
		{{ 0.0f, 5.0f, 0.0f }, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }}
	};

	std::vector<uint32_t> WaterIndices =
	{
		0, 1, 3,
		1, 2, 3
	};

	MeshID = engine->GenerateID();
	MeshProperties = MeshPropertiesUniformBuffer(engine);
	DrawFlags = MeshDrawFlags::Mesh_Skip_Water_Renderer;
	MeshType = MeshTypeFlag::Mesh_Type_Water;

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	VertexCount = WaterVertices.size();
	IndexCount = WaterIndices.size();
	PrimitiveCount = static_cast<uint32_t>(WaterIndices.size()) / 3;

	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
	SetUpMesh(engine, WaterVertices, WaterIndices);

	waterReflectionRenderPass = WaterRenderToTextureRenderPass(SceneData, SkyUniformBuffer);
	waterRefractionRenderPass = WaterRenderToTextureRenderPass(SceneData, SkyUniformBuffer);

	waterSurfacePipeline = std::make_shared<WaterSurfacePipeline>(WaterSurfacePipeline(RenderPass, waterReflectionRenderPass.RenderedTexture, waterRefractionRenderPass.RenderedTexture));
}

WaterSurfaceMesh::~WaterSurfaceMesh()
{
}

void WaterSurfaceMesh::DrawWaterTexture(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex, Skybox skybox)
{
	waterReflectionRenderPass.Draw(skybox);
	waterRefractionRenderPass.Draw(skybox);
}

void WaterSurfaceMesh::Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, SceneDataUniformBuffer& copysceneData, std::shared_ptr<PerspectiveCamera> camera)
{
	waterReflectionRenderPass.Update(copysceneData, camera);
	waterRefractionRenderPass.Update(copysceneData, camera);

	MeshProperties.UniformDataInfo.MaterialBufferIndex = MeshMaterial->MaterialBufferIndex;

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::translate(MeshTransform, MeshPosition);
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	MeshTransform = glm::scale(MeshTransform, MeshScale);

	MeshProperties.UniformDataInfo.ModelTransform = glm::mat4(1.0f);
	glm::mat4 FinalTransform = MeshTransform;
	glm::mat4 transformMatrix2 = glm::transpose(MeshTransform);

	TransformBuffer.CopyBufferToMemory(&FinalTransform, sizeof(FinalTransform));
	MeshProperties.Update();
}

void WaterSurfaceMesh::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> sceneData)
{
	waterReflectionRenderPass.RebuildSwapChain(sceneData);
	waterRefractionRenderPass.RebuildSwapChain(sceneData);
	waterSurfacePipeline->UpdateGraphicsPipeLine(RenderPass, waterReflectionRenderPass.RenderedTexture, waterRefractionRenderPass.RenderedTexture);
}

void WaterSurfaceMesh::Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo,  int a)
{
	//vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, waterSurfacePipeline->ShaderPipeline);
	//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, waterSurfacePipeline->ShaderPipelineLayout, 0, 1, &waterSurfacePipeline->DescriptorSets, 0, nullptr);
	//Mesh::Draw(commandBuffer);
}

void WaterSurfaceMesh::SubmitToCMDBuffer(std::shared_ptr<VulkanEngine> engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex)
{
	CMDBufferList.emplace_back(waterReflectionRenderPass.CommandBuffer);
	CMDBufferList.emplace_back(waterRefractionRenderPass.CommandBuffer);
}

void WaterSurfaceMesh::Destory(std::shared_ptr<VulkanEngine> engine)
{
	waterSurfacePipeline->Destroy();
	waterReflectionRenderPass.Destroy();
	waterRefractionRenderPass.Destroy();
	Mesh::Destory(engine);
}