#include "ComputeHelper.h"
#include "Vertex.h"

ComputeHelper::ComputeHelper()
{
}

ComputeHelper::ComputeHelper(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<SceneDataStruct> sceneData, VulkanBuffer& buffer2)
{
	//std::vector<VulkanBuffer> bufferList{ buffer };

	SetUpDescriptorPool(engine, buffer, sceneData, buffer2);
	SetUpDescriptorLayout(engine, buffer, sceneData, buffer2);
	SetUpDescriptorSets(engine, buffer, sceneData, buffer2);
	CreateShaderPipeLine(engine);
}

ComputeHelper::ComputeHelper(VulkanEngine& engine, std::vector<VulkanBuffer>& bufferList)
{
 //   SetUpDescriptorPool(engine, bufferList);
 //   SetUpDescriptorLayout(engine, bufferList);
 //   SetUpDescriptorSets(engine, bufferList);
	//CreateShaderPipeLine(engine);
}

ComputeHelper::~ComputeHelper()
{
}

void ComputeHelper::SetUpDescriptorPool(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<SceneDataStruct> sceneData, VulkanBuffer& buffer2)
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
	DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));

    descriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
}

void ComputeHelper::SetUpDescriptorLayout(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<SceneDataStruct> sceneData, VulkanBuffer& buffer2)
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
	descriptorLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}

void ComputeHelper::SetUpDescriptorSets(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<SceneDataStruct> sceneData, VulkanBuffer& buffer2)
{
	descriptorSets = engine.CreateDescriptorSets(descriptorPool, descriptorLayout);

	VkDescriptorBufferInfo VertexBufferInfo = AddBufferDescriptor(engine, buffer.Buffer, buffer.BufferSize);
	VkDescriptorBufferInfo SceneDataBufferInfo = AddBufferDescriptor(engine, sceneData->SceneDataBuffer.Buffer, sceneData->SceneDataBuffer.BufferSize);
	VkDescriptorBufferInfo TransformDataBufferInfo = AddBufferDescriptor(engine, buffer2.Buffer, buffer2.BufferSize);

	std::vector<VkWriteDescriptorSet> DescriptorList;
	DescriptorList.emplace_back(AddWriteDescriptorSet(engine, 0, descriptorSets, VertexBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
	DescriptorList.emplace_back(AddWriteDescriptorSet(engine, 2, descriptorSets, SceneDataBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
	DescriptorList.emplace_back(AddWriteDescriptorSet(engine, 5, descriptorSets, TransformDataBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));

	vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void ComputeHelper::CreateShaderPipeLine(VulkanEngine& engine)
{
	auto ComputeShaderCode = engine.CreateShader("Shader/animate.spv", VK_SHADER_STAGE_COMPUTE_BIT);

	VkPipelineShaderStageCreateInfo ComputeShaderStages[] = { ComputeShaderCode };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorLayout;
	vkCreatePipelineLayout(engine.Device, &pipelineLayoutInfo, nullptr, &ShaderPipelineLayout);

	VkComputePipelineCreateInfo ComputePipelineInfo{};
	ComputePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	ComputePipelineInfo.layout = ShaderPipelineLayout;
	ComputePipelineInfo.flags = 0;

	VkPipelineCacheCreateInfo pipelineCacheInfo{};
	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkCreatePipelineCache(engine.Device, &pipelineCacheInfo, nullptr, &PipelineCache);

	//VkPipelineShaderStageCreateInfo pipelineShaderStageInfo {};
	//pipelineShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	//pipelineShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	//pipelineShaderStageInfo.module = &ComputeShaderStages;
	//pipelineShaderStageInfo.pName = "main";

	//if (vkCreateComputePipelines(engine.Device, PipelineCache, 1, &ComputePipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to create compute pipeline!");
	//}

	vkDestroyShaderModule(engine.Device, ComputeShaderStages->module, nullptr);
}

VkDescriptorBufferInfo ComputeHelper::AddBufferDescriptor(VulkanEngine& engine, VkBuffer Buffer, VkDeviceSize BufferSize)
{
	VkDescriptorBufferInfo BufferInfo = {};
	BufferInfo.buffer = Buffer;
	BufferInfo.offset = 0;
	BufferInfo.range = BufferSize;
	return BufferInfo;
}

VkWriteDescriptorSet ComputeHelper::AddWriteDescriptorSet(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo, VkDescriptorType descriptorType)
{
	VkWriteDescriptorSet BufferDescriptor = {};
	BufferDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	BufferDescriptor.dstSet = DescriptorSet;
	BufferDescriptor.dstBinding = BindingNumber;
	BufferDescriptor.dstArrayElement = 0;
	BufferDescriptor.descriptorType = descriptorType;
	BufferDescriptor.descriptorCount = 1;
	BufferDescriptor.pBufferInfo = &BufferInfo;
	return BufferDescriptor;
}