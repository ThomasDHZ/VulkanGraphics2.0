#include "ComputeHelper.h"
#include "Vertex.h"

ComputeHelper::ComputeHelper()
{
}

ComputeHelper::ComputeHelper(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<UniformData<SceneDataBuffer>> sceneData, VulkanBuffer& buffer2, MeshPropertiesUniformBuffer& meshdata)
{
	//std::vector<VulkanBuffer> bufferList{ buffer };

	SetUpDescriptorPool(engine, buffer, sceneData, buffer2, meshdata);
	SetUpDescriptorLayout(engine, buffer, sceneData, buffer2, meshdata);
	SetUpDescriptorSets(engine, buffer, sceneData, buffer2, meshdata);
	CreateShaderPipeLine(engine);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = engine.CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(engine.Device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
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

void ComputeHelper::SetUpDescriptorPool(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<UniformData<SceneDataBuffer>> sceneData, VulkanBuffer& buffer2, MeshPropertiesUniformBuffer& meshdata)
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
	DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
	DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));

    descriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
}

void ComputeHelper::SetUpDescriptorLayout(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<UniformData<SceneDataBuffer>> sceneData, VulkanBuffer& buffer2, MeshPropertiesUniformBuffer& meshdata)
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
	LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1 });
	descriptorLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}

void ComputeHelper::SetUpDescriptorSets(VulkanEngine& engine, VulkanBuffer& buffer, std::shared_ptr<UniformData<SceneDataBuffer>> sceneData, VulkanBuffer& buffer2, MeshPropertiesUniformBuffer& meshdata)
{
	descriptorSets = engine.CreateDescriptorSets(descriptorPool, descriptorLayout);

	VkDescriptorBufferInfo VertexBufferInfo = engine.AddBufferDescriptor(buffer);
	VkDescriptorBufferInfo SceneDataBufferInfo = engine.AddBufferDescriptor(sceneData->VulkanBufferData);
	VkDescriptorBufferInfo MeshDataufferInfo = engine.AddBufferDescriptor(meshdata.VulkanBufferData);
	VkDescriptorBufferInfo TransformDataBufferInfo = engine.AddBufferDescriptor(buffer2);

	std::vector<VkWriteDescriptorSet> DescriptorList;
	DescriptorList.emplace_back(engine.AddBufferDescriptorSet(0, descriptorSets, VertexBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
	DescriptorList.emplace_back(engine.AddBufferDescriptorSet(2, descriptorSets, SceneDataBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
	DescriptorList.emplace_back(engine.AddBufferDescriptorSet(3, descriptorSets, MeshDataufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
	DescriptorList.emplace_back(engine.AddBufferDescriptorSet(6, descriptorSets, TransformDataBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
	vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void ComputeHelper::CreateShaderPipeLine(VulkanEngine& engine)
{
	auto ComputeShaderCode = engine.CreateShader("Shader/animate.spv", VK_SHADER_STAGE_COMPUTE_BIT);
	
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(ConstMeshInfo);

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

void ComputeHelper::Compute(VulkanEngine& engine, VulkanBuffer& buffer, uint32_t currentFrame)
{
	ConstMeshInfo meshInfo;
	meshInfo.MeshID = 0;
	meshInfo.ModelID = 0;
	meshInfo.MaterialID = 0;

	VkCommandBufferBeginInfo CommandBufferBeginInfo{};
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
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
	vkCmdPushConstants(commandBuffer, ShaderPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT , 0, sizeof(ConstMeshInfo), &meshInfo);
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
}

void ComputeHelper::Destroy(VulkanEngine& engine)
{
	vkDestroyPipeline(engine.Device, ShaderPipeline, nullptr);
	vkDestroyPipelineLayout(engine.Device, ShaderPipelineLayout, nullptr);
	vkDestroyPipelineCache(engine.Device, PipelineCache, nullptr);
	vkDestroyDescriptorSetLayout(engine.Device, descriptorLayout, nullptr);
	vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);
	
	ShaderPipeline = VK_NULL_HANDLE;
	ShaderPipelineLayout = VK_NULL_HANDLE;
	PipelineCache = VK_NULL_HANDLE;
	descriptorLayout = VK_NULL_HANDLE;
	descriptorPool = VK_NULL_HANDLE;
}