#pragma once
#include "Model.h"

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

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorLayout(VulkanEngine& engine);
	void SetUpDescriptorSets(VulkanEngine& engine, std::vector<std::shared_ptr<Model>> modelList);
	void CreateShaderPipeLine(VulkanEngine& engine);

public:
	AnimatorCompute();
	AnimatorCompute(VulkanEngine& engine, std::vector<std::shared_ptr<Model>> modelList);
	~AnimatorCompute();

	VkCommandBuffer commandBuffer;

	void Compute(VulkanEngine& engine, uint32_t currentFrame);
	void Destroy(VulkanEngine& engine);
};