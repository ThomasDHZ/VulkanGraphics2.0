#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"
#include "WaterSurfacePipeline.h"

class WaterSurfaceMesh : public Mesh
{
	std::shared_ptr<WaterSurfacePipeline> waterSurfacePipeline;

public:
	WaterSurfaceMesh();
	WaterSurfaceMesh(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> SceneData, std::shared_ptr<Texture> reflecttexture, std::shared_ptr<Texture> refractiontexture);
	~WaterSurfaceMesh();

	void Update(VulkanEngine& engine, MaterialManager& materialManager) override;
	void UpdateGraphicsPipeLine(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> SceneData, std::shared_ptr<Texture> reflecttexture, std::shared_ptr<Texture> refractiontexture);
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID, int a);
	void Destory(VulkanEngine& engine) override;
};
