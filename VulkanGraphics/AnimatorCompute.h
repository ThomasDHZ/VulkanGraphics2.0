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

	void SetUpDescriptorPool();
	void SetUpDescriptorLayout();
	void SetUpDescriptorSets();
	void CreateShaderPipeLine();

public:
	AnimatorCompute();
	AnimatorCompute(std::shared_ptr<Mesh> meshptr);
	~AnimatorCompute();

	VkCommandBuffer commandBuffer;

	void Compute();
	void Destroy();
};