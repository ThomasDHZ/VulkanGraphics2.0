#pragma once
#include "Mesh.h"

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
	std::shared_ptr<Mesh> mesh;

	void SetUpDescriptorPool(VulkanEngine& engine);
	void SetUpDescriptorLayout(VulkanEngine& engine);
	void SetUpDescriptorSets(VulkanEngine& engine);
	void CreateShaderPipeLine(VulkanEngine& engine);

public:
	AnimatorCompute();
	AnimatorCompute(VulkanEngine& engine, std::shared_ptr<Mesh> meshptr);
	~AnimatorCompute();

	VkCommandBuffer commandBuffer;

	void Compute(VulkanEngine& engine, uint32_t currentFrame);
	void Destroy(VulkanEngine& engine);
};