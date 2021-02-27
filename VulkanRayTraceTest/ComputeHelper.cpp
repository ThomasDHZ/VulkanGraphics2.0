#include "ComputeHelper.h"

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
	DescriptorList.emplace_back(AddStorageBuffer(engine, 0, descriptorSets, VertexBufferInfo));
	DescriptorList.emplace_back(AddStorageBuffer(engine, 2, descriptorSets, SceneDataBufferInfo));
	DescriptorList.emplace_back(AddStorageBuffer(engine, 5, descriptorSets, TransformDataBufferInfo));

	vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void ComputeHelper::CreateShaderPipeLine(VulkanEngine& engine)
{
	auto ComputeShaderCode = engine.CreateShader("Shader/animate.spv", VK_SHADER_STAGE_COMPUTE_BIT);

	VkPipelineShaderStageCreateInfo FrameBufferShaderStages[] = { ComputeShaderCode };

	VkPipelineLayoutCreateInfo FrameBufferPipelineLayoutInfo = {};
	FrameBufferPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	FrameBufferPipelineLayoutInfo.setLayoutCount = 1;
	FrameBufferPipelineLayoutInfo.pSetLayouts = &descriptorLayout;

	if (vkCreatePipelineLayout(engine.Device, &FrameBufferPipelineLayoutInfo, nullptr, &ShaderPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo FrameBufferPipelineInfo = {};
	FrameBufferPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	FrameBufferPipelineInfo.stageCount = 1;
	FrameBufferPipelineInfo.pStages = FrameBufferShaderStages;
	FrameBufferPipelineInfo.layout = ShaderPipelineLayout;

	if (vkCreateGraphicsPipelines(engine.Device, VK_NULL_HANDLE, 1, &FrameBufferPipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(engine.Device, FrameBufferShaderStages->module, nullptr);
}

VkDescriptorBufferInfo ComputeHelper::AddBufferDescriptor(VulkanEngine& engine, VkBuffer Buffer, VkDeviceSize BufferSize)
{
	VkDescriptorBufferInfo BufferInfo = {};
	BufferInfo.buffer = Buffer;
	BufferInfo.offset = 0;
	BufferInfo.range = BufferSize;
	return BufferInfo;
}

VkWriteDescriptorSet ComputeHelper::AddStorageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo)
{
	VkWriteDescriptorSet BufferDescriptor = {};
	BufferDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	BufferDescriptor.dstSet = DescriptorSet;
	BufferDescriptor.dstBinding = BindingNumber;
	BufferDescriptor.dstArrayElement = 0;
	BufferDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	BufferDescriptor.descriptorCount = 1;
	BufferDescriptor.pBufferInfo = &BufferInfo;
	return BufferDescriptor;
}