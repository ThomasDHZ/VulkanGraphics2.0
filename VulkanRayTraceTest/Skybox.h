#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"
#include "SkyBoxRenderingPipeline.h"

class Skybox : public Mesh
{
private:

	std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer;
	std::shared_ptr<SkyBoxRenderingPipeline> SkyboxRenderingPipeline;

public:
	Skybox();
	Skybox(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass);
	~Skybox();

	void Update(VulkanEngine& engine, MaterialManager& materialManager, std::shared_ptr<PerspectiveCamera> camera, int imageView);
	void UpdateGraphicsPipeLine(VulkanEngine& engine, VkRenderPass& RenderPass);
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID) override;
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID, int a);
	void Destory(VulkanEngine& engine) override;
};

