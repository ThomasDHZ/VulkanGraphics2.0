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
	WaterSurfaceMesh(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> SceneData);
	~WaterSurfaceMesh();

	void Update(VulkanEngine& engine, AssetManager& assetManager, SceneDataUniformBuffer& copysceneData, std::shared_ptr<PerspectiveCamera> camera);
	void UpdateSwapChain(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> sceneData);
	void DrawWaterTexture(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex, Skybox skybox);
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID, int a);
	void SubmitToCMDBuffer(VulkanEngine& engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex);
	void Destory(VulkanEngine& engine) override;
};
