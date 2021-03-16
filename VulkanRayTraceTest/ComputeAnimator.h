#pragma once
#include "model.h"

class ComputeAnimator
{
private:
	VkDescriptorSetLayout descriptorLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSets;
	VkPipelineLayout ShaderPipelineLayout;
	VkPipeline ShaderPipeline;
	VkPipelineCache PipelineCache;

	std::shared_ptr<VulkanBuffer> VertexBufferCopy;

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorLayout(VulkanEngine& engine);
	void SetUpDescriptorSets(VulkanEngine& engine, std::vector<Model>& modelList);
	void CreateShaderPipeLine(VulkanEngine& engine);

public:
	ComputeAnimator();
	ComputeAnimator(VulkanEngine& engine, std::vector<Model>& modelList);
	~ComputeAnimator();

	VkCommandBuffer commandBuffer;

	void Compute(VulkanEngine& engine, uint32_t currentFrame);
	void Destroy(VulkanEngine& engine);
};
