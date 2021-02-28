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

	void SetUpDescriptorPool(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<SceneDataStruct> sceneData, VulkanBuffer& buffer2);
    void SetUpDescriptorLayout(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<SceneDataStruct> sceneData, VulkanBuffer& buffer2);
    void SetUpDescriptorSets(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<SceneDataStruct> sceneData, VulkanBuffer& buffer2);
	void CreateShaderPipeLine(VulkanEngine& engine);

	VkDescriptorBufferInfo AddBufferDescriptor(VulkanEngine& engine, VkBuffer Buffer, VkDeviceSize BufferSize);
	VkWriteDescriptorSet AddWriteDescriptorSet(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo, VkDescriptorType descriptorType);
  
public:
	ComputeHelper();
	ComputeHelper(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<SceneDataStruct> sceneData, VulkanBuffer& buffer2);
	ComputeHelper(VulkanEngine& engine, std::vector<VulkanBuffer>& buffer);
	~ComputeHelper();
};

