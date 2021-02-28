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
	
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(MeshInfo);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorLayout;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	vkCreatePipelineLayout(engine.Device, &pipelineLayoutInfo, nullptr, &ShaderPipelineLayout);

	VkPipelineCacheCreateInfo pipelineCacheInfo{};
	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkCreatePipelineCache(engine.Device, &pipelineCacheInfo, nullptr, &PipelineCache);

	VkComputePipelineCreateInfo ComputePipelineInfo{};
	ComputePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	ComputePipelineInfo.layout = ShaderPipelineLayout;
	ComputePipelineInfo.flags = 0;
	ComputePipelineInfo.stage = ComputeShaderCode;

	if (vkCreateComputePipelines(engine.Device, PipelineCache, 1, &ComputePipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline!");
	}

	vkDestroyShaderModule(engine.Device, ComputeShaderCode.module, nullptr);
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

void ComputeHelper::Compute(VulkanEngine& engine, VkCommandBuffer& commandBuffer, VulkanBuffer& buffer, uint32_t currentFrame)
{
	MeshInfo meshInfo;
	meshInfo.MeshID = 0;
	meshInfo.ModelID = 0;
	meshInfo.MaterialID = 0;

	VkCommandBufferBeginInfo CommandBufferBeginInfo{};
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(commandBuffer, &CommandBufferBeginInfo);

	VkBufferMemoryBarrier BufferMemoryBarrier{};
	BufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	BufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	BufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	BufferMemoryBarrier.buffer = buffer.Buffer;
	BufferMemoryBarrier.size = VK_WHOLE_SIZE;
	BufferMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	BufferMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	BufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	BufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		0, nullptr,
		1, &BufferMemoryBarrier,
		0, nullptr);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ShaderPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ShaderPipelineLayout, 0, 1, &descriptorSets, 0, 0);
	vkCmdPushConstants(commandBuffer, ShaderPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT , 0, sizeof(MeshInfo), &meshInfo);
	vkCmdDispatch(commandBuffer, buffer.BufferSize, 1, 1);

	BufferMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	BufferMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	BufferMemoryBarrier.buffer = buffer.Buffer;
	BufferMemoryBarrier.size = VK_WHOLE_SIZE;
	BufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	BufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		1, &BufferMemoryBarrier,
		0, nullptr);

	vkEndCommandBuffer(commandBuffer);

	vkResetFences(engine.Device, 1, &engine.inFlightFences[currentFrame]);
	const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo computeSubmitInfo{};
	computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	computeSubmitInfo.pWaitDstStageMask = &waitStageMask;
	computeSubmitInfo.commandBufferCount = 1;
	computeSubmitInfo.pCommandBuffers = &commandBuffer;
	vkQueueSubmit(engine.GraphicsQueue, 1, &computeSubmitInfo, engine.inFlightFences[currentFrame]);
	vkWaitForFences(engine.Device, 1, &engine.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
}

void ComputeHelper::Destroy(VulkanEngine& engine)
{
}