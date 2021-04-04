#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"
#include "AssetManager.h"

class Skybox : public Mesh
{
private:

	VkDescriptorSetLayout DescriptorLayout;
	VkDescriptorPool DescriptorPool;
	VkDescriptorSet DescriptorSets;
	VkPipelineLayout ShaderPipelineLayout;
	VkPipeline ShaderPipeline;

	void CreateDescriptorSetLayout(VulkanEngine& engine, AssetManager& assetManager);
	void CreateShaderPipeLine(VulkanEngine& engine, VkRenderPass& RenderPass);
	void CreateDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
	void CreateDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> SceneData);

public:
	Skybox();
	Skybox(VulkanEngine& engine, AssetManager& assetManager, VkRenderPass& RenderPass, std::shared_ptr<SceneDataUniformBuffer> SceneData);
	~Skybox();

	void Update(VulkanEngine& engine, MaterialManager& materialManager) override;
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo) override;
	void Destory(VulkanEngine& engine) override;
};

