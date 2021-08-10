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

	void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine);
	void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine);
	void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine);
	void CreateShaderPipeLine(std::shared_ptr<VulkanEngine> engine);

public:
	AnimatorCompute();
	AnimatorCompute(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<Mesh> meshptr);
	~AnimatorCompute();

	VkCommandBuffer commandBuffer;

	void Compute(std::shared_ptr<VulkanEngine> engine);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};