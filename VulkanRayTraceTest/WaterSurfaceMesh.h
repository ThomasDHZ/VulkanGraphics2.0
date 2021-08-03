#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"
#include "WaterSurfacePipeline.h"
#include "WaterRenderPass.h"

class WaterSurfaceMesh : public Mesh
{
	WaterRenderToTextureRenderPass waterReflectionRenderPass;
	WaterRenderToTextureRenderPass waterRefractionRenderPass;

	std::shared_ptr<WaterSurfacePipeline> waterSurfacePipeline;

public:
	WaterSurfaceMesh();
	WaterSurfaceMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> SceneData, std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer);
	~WaterSurfaceMesh();

	void Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, SceneDataUniformBuffer& copysceneData, std::shared_ptr<PerspectiveCamera> camera);
	void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void DrawWaterTexture(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex, Skybox skybox);
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID, int a);
	void SubmitToCMDBuffer(std::shared_ptr<VulkanEngine> engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex);
	void Destory(std::shared_ptr<VulkanEngine> engine) override;
};
