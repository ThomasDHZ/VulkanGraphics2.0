#pragma once
#include "GraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "SceneData.h"

class ComputeHelper
{
private:
	VkDescriptorSetLayout descriptorLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSets;
	VkPipelineLayout ShaderPipelineLayout;
	VkPipeline ShaderPipeline;
	VkPipelineCache PipelineCache;

	void SetUpDescriptorPool(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<UniformData<SceneDataBuffer>> sceneData, VulkanBuffer& buffer2, MeshPropertiesUniformBuffer& meshdata);
    void SetUpDescriptorLayout(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<UniformData<SceneDataBuffer>> sceneData, VulkanBuffer& buffer2, MeshPropertiesUniformBuffer& meshdata);
    void SetUpDescriptorSets(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<UniformData<SceneDataBuffer>> sceneData, VulkanBuffer& buffer2, MeshPropertiesUniformBuffer& meshdata);
	void CreateShaderPipeLine(VulkanEngine& engine);
  
public:
	ComputeHelper();
	ComputeHelper(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<UniformData<SceneDataBuffer>> sceneData, VulkanBuffer& buffer2, MeshPropertiesUniformBuffer& meshdata);
	ComputeHelper(VulkanEngine& engine, std::vector<VulkanBuffer>& buffer);
	~ComputeHelper();

	VkCommandBuffer commandBuffer;

	void Compute(VulkanEngine& engine, VulkanBuffer& buffer, uint32_t currentFrame);
	void Destroy(VulkanEngine& engine);
};

