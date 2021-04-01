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
	VkRenderPass RenderPass;
	VkPipelineLayout ShaderPipelineLayout;
	VkPipeline ShaderPipeline;

	void CreateRenderPass(VulkanEngine& engine);
	void CreateRendererFramebuffers(VulkanEngine& engine);
	void CreateDescriptorSetLayout(VulkanEngine& engine, AssetManager& assetManager);
	void CreateShaderPipeLine(VulkanEngine& engine);
	void CreateDescriptorPool(VulkanEngine& engine);
	void CreateDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> SceneData);

public:
	Skybox();
	Skybox(VulkanEngine& engine, AssetManager& assetManager);
	~Skybox();

	void Update(VulkanEngine& engine, MaterialManager& materialManager);
};

