#pragma once
#include "Model.h"
#include "AssetManager.h"

class AnimatorCompute
{
private:
	VkDescriptorSetLayout descriptorLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSets;
	VkPipelineLayout ShaderPipelineLayout;
	VkPipeline ShaderPipeline;
	VkPipelineCache PipelineCache;

	std::shared_ptr<VulkanBuffer> VertexBufferCopy;
	std::shared_ptr<Model> model;

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorLayout(VulkanEngine& engine);
	void SetUpDescriptorSets(VulkanEngine& engine);
	void CreateShaderPipeLine(VulkanEngine& engine);

public:
	AnimatorCompute();
	AnimatorCompute(VulkanEngine& engine, std::shared_ptr<Model> model);
	~AnimatorCompute();

	VkCommandBuffer commandBuffer;

	void Compute(VulkanEngine& engine, uint32_t currentFrame);
	void Destroy(VulkanEngine& engine);
};